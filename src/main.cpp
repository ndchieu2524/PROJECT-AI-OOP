#include <iostream>
#include "harness/harness_runner.h"

using namespace std;

int main() {
    HarnessRunner runner;

    // Nạp bài test 1: Functional Evaluation (So sánh giá trị số)
    BenchmarkTask task1{
        "TASK_01",
        "Tính phép toán 10 + 20",
        "30",
        {"30"},
        "functional"
    };

    // Nạp bài test 2: Keyword Evaluation (So sánh từ khóa)
    BenchmarkTask task2{
        "TASK_02",
        "Tra cứu thủ đô của Việt Nam",
        "Hà Nội",
        {"Hà Nội", "thủ đô"},
        "keyword"
    };

    runner.addTask(task1);
    runner.addTask(task2);

    runner.runBenchmarkSuite();

    return 0;
}