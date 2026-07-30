#include <iostream>
#include "harness/harness_runner.h"

int main() {
    HarnessRunner runner;

    BenchmarkTask task1{
        "TASK_01",
        "Tính phép toán",
        "30",
        {"Kết quả", "30"},
        "keyword"
    };

    runner.addTask(task1);
    runner.runMockBenchmark();

    return 0;
}