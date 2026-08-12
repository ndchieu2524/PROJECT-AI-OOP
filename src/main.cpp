#include <iostream>
#include <memory>
#include "harness/harness_runner.h"
#include "harness/taskloader.h"
#include "harness/mockagent.h"
using namespace std;

int main() {
    vector<BenchmarkTask> tasks = TaskLoader::loadFromJsonFile("tasks.json");
    if (tasks.empty()) {
        cout << "LỖI: Không có task json nào!\n";
        return 1;
    }
    auto mockAgent = make_shared<MockAgent>();
    
    HarnessRunner runner;
    runner.setAgent(mockAgent);
    runner.setTasks(tasks);
    runner.runBenchmarkSuite();

    return 0;
}