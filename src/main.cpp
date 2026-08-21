#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <exception>

#include "client/ollama_client.h"

#include "tools/tool_registry.h"
#include "tools/exec_tool.h"
#include "tools/file_tool.h"
#include "tools/calculator_tool.h"
#include "tools/web_search_tool.h"
#include "tools/memory_tool.h"

#include "agent/agent_loop.h"
#include "agent/skill_loader.h"

#include "harness/harness_runner.h"
#include "harness/taskloader.h"

using namespace std;
using namespace agent::llm;
using namespace agent::tools;
using namespace agent::skills;

int main() {
    try {
        cout << "\033[1;36m==================================================\033[0m\n";
        cout << "\033[1;36m     KHỞI CHẠY HỆ THỐNG AI AGENT BENCHMARK         \033[0m\n";
        cout << "\033[1;36m==================================================\033[0m\n\n";

        Options options;
        options.modelName = "qwen2.5:1.5b"; 
        options.temperature = 0.0;
        options.maxTokens = 512;

        auto client = make_shared<OllamaClient>("http://localhost:11434", options);

        auto registry = make_shared<ToolRegistry>();
        registry->registerTool(make_shared<ExecTool>(5));
        registry->registerTool(make_shared<FileTool>("./workspace"));
        registry->registerTool(make_shared<CalculatorTool>());
        registry->registerTool(make_shared<WebSearchTool>(5));
        registry->registerTool(make_shared<MemoryTool>("agent_memory.db"));

        auto skillLoader = make_shared<SkillLoader>("skills");
        skillLoader->loadSkillsFromDirectory();

        auto agent = make_shared<AgentLoop>(client, registry, skillLoader, 6);

        string benchmarkPath = "benchmark/tasks.json";
        vector<BenchmarkTask> tasks = TaskLoader::loadFromJsonFile(benchmarkPath);
        
        if (tasks.empty()) {
            cerr << "\033[31m[LỖI]: Không tải được danh sách task từ " << benchmarkPath << "\033[0m\n";
            return 1;
        }

        HarnessRunner runner;
        runner.setAgent(agent);
        runner.setTasks(tasks);

        BenchmarkReport report = runner.runBenchmarkSuite("benchmark_report.json");

    } catch (const exception& e) {
        cerr << "\n\033[31m[LỖI HỆ THỐNG]: " << e.what() << "\033[0m\n";
        return 1;
    }

    return 0;
}