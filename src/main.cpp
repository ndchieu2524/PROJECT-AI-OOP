#include <iostream>
#include <memory>
#include <string>
#include "harness/harness_runner.h"
#include "harness/taskloader.h"
#include "harness/mockagent.h"
using namespace std;

int main(int argc, char* argv[]) {
    string task_file = "tasks.json";
    string report_file = "benchmark_report.json";
    if (argc > 1) {
        task_file = argv[1];
    }
    if (argc > 2) {
        report_file = argv[2];
    }
    cout << Color::CYAN << "[CẤU HÌNH] Sử dụng file test: " << task_file << Color::RESET << endl;

    vector<BenchmarkTask> tasks = TaskLoader::loadFromJsonFile("tasks.json");
    if (tasks.empty()) {
        cerr << Color::RED << "LỖI: Không có task json nào!\n";
        return 1;
    }
    auto mockAgent = make_shared<MockAgent>();
    
    HarnessRunner runner;
    runner.setAgent(mockAgent);
    runner.setTasks(tasks);
    runner.runBenchmarkSuite(report_file);

    return 0;
}