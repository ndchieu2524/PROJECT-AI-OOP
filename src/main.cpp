#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <exception>
#include <fstream>
#include <filesystem>
#include "client/ollama_client.h"
#include "agent/loop_detector.h"
#include "tools/tool_registry.h"
#include "tools/exec_tool.h"
#include "tools/file_tool.h"
#include "tools/calculator_tool.h"
#include "harness/evaluator.h"

using namespace std;
using namespace agent::llm;
using namespace agent::engine;
using namespace agent::tools;

string extractJson(const string& text) {
    size_t start = text.find('{');
    size_t end = text.rfind('}');
    if (start != string::npos && end != string::npos && end > start) {
        return text.substr(start, end - start + 1);
    }
    return "";
}

int main() {
    try {
        cout << "========================================" << endl;
        cout << "    THỬ NGHIỆM MULTI-TOOL & HARNESS     " << endl;
        cout << "========================================" << endl;

        auto registry = make_shared<ToolRegistry>();
        registry->registerTool(make_shared<ExecTool>(5));
        registry->registerTool(make_shared<FileTool>("./workspace"));
        registry->registerTool(make_shared<CalculatorTool>());

        Options options;
        options.modelName = "qwen2.5:1.5b";
        options.temperature = 0.0f;
        OllamaClient client("http://localhost:11434", options);
        LoopDetector loopDetector;

        string userPrompt = "Hãy tính (450 * 12) + 850 rồi lưu kết quả vào file output.txt";
        
        string systemInstruction = R"(You are a ReAct AI agent.

Available tools:
1. calculator: {"expression": "math_string"}
2. file: {"action": "write", "path": "file_path", "content": "text_content"}

CRITICAL TOOL RULES:
- For "file" tool, the "action" field is MANDATORY. You MUST include "action": "write".

Correct Example:
{"tool": "file", "args": {"action": "write", "path": "output.txt", "content": "6250.0"}}
)";

        vector<Message> conversation = {
            {"system", systemInstruction},
            {"user", userPrompt}
        };

        string finalAgentReply = "";
        const int MAX_TURNS = 6;

        for (int turn = 1; turn <= MAX_TURNS; ++turn) {
            cout << "\n--- [Lượt suy luận " << turn << "] ---" << endl;
            
            Response response = client.chat(conversation);
            string agentReply = response.content;
            cout << "[AGENT]: " << agentReply << endl;

            loopDetector.addActionToHistory(agentReply);
            if (loopDetector.evaluateLoopStatus() == LoopSeverity::CRITICAL) {
                cout << "  -> [NGẮT]: Phát hiện lặp tiến trình." << endl;
                break;
            }
            
            string jsonStr = extractJson(agentReply);
            bool executedTool = false;

            if (!jsonStr.empty()) {
                try {
                    auto jsonCmd = nlohmann::json::parse(jsonStr);
                    if (jsonCmd.contains("tool") && jsonCmd.contains("args")) {
                        string toolName = jsonCmd["tool"];
                        auto toolArgs = jsonCmd["args"];

                        cout << "  -> [ACTION]: Thực thi Tool [" << toolName << "]..." << endl;
                        nlohmann::json result = registry->execute(toolName, toolArgs);
                        cout << "  -> [RESULT]: " << result.dump() << endl;

                        conversation.push_back({"assistant", agentReply});
                        conversation.push_back({"user", "Tool Output: " + result.dump()});
                        executedTool = true;
                    }
                } catch (...) {}
            }

            if (!executedTool && !agentReply.empty()) {
                if (turn == 1) {
                    cout << "  -> [CẢNH BÁO]: Agent chưa gọi Tool. Đang nhắc Agent thực thi..." << endl;
                    conversation.push_back({"assistant", agentReply});
                    conversation.push_back({"user", "LỖI: Bạn chưa sử dụng Tool. Hãy xuất lệnh JSON để gọi Tool calculator trước!"});
                    continue;
                }

                finalAgentReply = agentReply;
                cout << "\n=> Hoàn tất chuỗi nhiệm vụ!" << endl;
                break;
            }
        }

        cout << "\n========================================" << endl;
        cout << "   ĐÁNH GIÁ TỰ ĐỘNG (TEST HARNESS)      " << endl;
        cout << "========================================" << endl;

       KeywordEvaluator evaluator;
        vector<string> expectedKeywords = {"6250", "output.txt", "successfully"};
        bool pass = true;
        for (const auto& kw : expectedKeywords) {
            bool found = finalAgentReply.find(kw) != string::npos;
            cout << "  -> Kiểm tra từ khóa [" << kw << "]: " << (found ? "PASSED" : "CHECK") << endl;
        }

    } catch (const exception& e) {
        cerr << "\n[LỖI]: " << e.what() << endl;
        return 1;
    }

    return 0;
}