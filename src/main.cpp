#include <iostream>
#include <memory>
#include <filesystem>
#include "harness/harness_runner.h"
#include "harness/taskloader.h"
#include "harness/mockagent.h"
#include "agent/skill_loader.h"

using namespace std;
using namespace agent::skills;

int main(int argc, char* argv[]) {
    string task_file = "tasks.json";
    string report_file = "benchmark_report.json";

    if (argc > 1) task_file = argv[1];
    if (argc > 2) report_file = argv[2];

    string skillsDir = "./skills";
    if (!std::filesystem::exists(skillsDir)) {
        std::filesystem::create_directory(skillsDir);
    }
    
    auto skillLoader = make_shared<SkillLoader>(skillsDir);
    skillLoader->loadSkillsFromDirectory();

    vector<BenchmarkTask> tasks = TaskLoader::loadFromJsonFile(task_file);
    if (tasks.empty()) {
        cerr << Color::RED << "[LỖI] Không thể nạp bài test từ " << task_file << Color::RESET << endl;
        return 1;
    }

    auto agent = make_shared<MockAgent>(skillLoader);

    HarnessRunner runner;
    runner.setAgent(agent);
    runner.setTasks(tasks);

    runner.runBenchmarkSuite(report_file);

    return 0;
}