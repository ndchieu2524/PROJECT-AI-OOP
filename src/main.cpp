#include <iostream>
#include <memory>
#include <string>
#include <exception>

// 1. MODULE CLIENT & AGENT (Member A)
#include "client/ollama_client.h"
#include "agent/loop_detector.h"
#include "agent/skill_loader.h"
using namespace agent::llm;
using namespace agent::skills;
using namespace agent::engine;

// 2. MODULE TOOLS (Member B)
#include "tools/tool_registry.h"
#include "tools/exec_tool.h"
#include "tools/file_tool.h"
#include "tools/calculator_tool.h"

// 3. MODULE HARNESS (Member C)
#include "harness/harness_runner.h"
#include "harness/evaluator.h"
#include "harness/taskloader.h"

using namespace std;
using namespace agent::tools;

int main() {
    try {
        cout << "========================================" << endl;
        cout << "    KHỞI CHẠY HỆ THỐNG AI AGENT!!!      " << endl;
        cout << "========================================" << endl;

        cout << "\n[1] Đang khởi tạo Tool Registry..." << endl;
        auto registry = make_shared<ToolRegistry>();
        
        registry->registerTool(make_shared<ExecTool>(/*timeoutSeconds=*/5));
        registry->registerTool(make_shared<FileTool>("./workspace"));
        registry->registerTool(make_shared<CalculatorTool>());

        auto calc = registry->getTool("calculator");
        if (calc) {
            nlohmann::json args = {{"expression", "2 + 3 * 5"}};
            
            nlohmann::json result = calc->execute(args);
            
            cout << "  -> [TEST TOOL] Kết quả: " << result.dump(2) << endl;
        }

        cout << "  -> Đăng ký thành công: ExecTool, FileTool, CalculatorTool." << endl;

        cout << "\n[2] Đang khởi tạo Agent Core & LLM Client..." << endl;

        Options options;
        options.modelName = "llama3";
        OllamaClient client("http://localhost:11434", options);
        LoopDetector loopDetector;
        SkillLoader skillLoader("./skills");

        cout << "  -> Module Agent & Client đã sẵn sàng." << endl;

        cout << "\n[3] Đang khởi tạo Test Harness..." << endl;

        TaskLoader taskLoader;
        KeywordEvaluator evaluator; 
        // FunctionalEvaluator evaluator;
        // RegexEvaluator evaluator;
        HarnessRunner runner;

        cout << "  -> Module Harness Evaluation đã sẵn sàng." << endl;

        cout << "\n========================================" << endl;
        cout << " HỆ THỐNG ĐÃ KHỞI TẠO & BIÊN DỊCH THÀNH CÔNG " << endl;
        cout << "========================================" << endl;

    } catch (const exception& e) {
        cerr << "\n[LỖI NGHIÊM TRỌNG]: " << e.what() << endl;
        return 1;
    }

    return 0;
}