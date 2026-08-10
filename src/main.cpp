#include <iostream>
#include <memory>
#include "harness/harness_runner.h"
#include "harness/taskloader.h"
using namespace std;

int main() {
    HarnessRunner runner;
    vector<BenchmarkTask> tasks = TaskLoader::loadFromJsonFile("tasks.json");
    if (tasks.empty()) {
        cout << "LỖI: Không có task json nào!\n";
        return 1;
    }

    runner.setTasks(tasks);
    runner.runBenchmarkSuite();

    return 0;
}