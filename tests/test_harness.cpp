#include <iostream>
#include <cassert>
#include "harness/evaluator.h"
#include "harness/trajectory.h"

using namespace std;

void testKeywordEvaluator() {
    BenchmarkTask task;
    task.id = "TEST_KW";
    task.keywords = {"Hà Nội", "thủ đô"};

    KeywordEvaluator evaluator;
    Trajectory traj("TEST_KW");

    EvalResult res1 = evaluator.evaluate(task, "Thủ đô của Việt Nam là Hà Nội", traj);
    assert(res1.passed == true);
    assert(res1.score == 1.0);

    EvalResult res2 = evaluator.evaluate(task, "Hà Nội là một thành phố lớn", traj);
    assert(res2.score == 0.5);

    cout << "[UNIT TEST PASSED] KeywordEvaluator" << endl;
}

void testFunctionalEvaluator() {
    BenchmarkTask task;
    task.id = "TEST_FUNC";
    task.expected_output = "40.0";

    FunctionalEvaluator evaluator;
    Trajectory traj("TEST_FUNC");

    EvalResult res1 = evaluator.evaluate(task, "Kết quả tính được là 40.0005", traj);
    assert(res1.passed == true);

    EvalResult res2 = evaluator.evaluate(task, "Kết quả là 42.5", traj);
    assert(res2.passed == false);

    cout << "[UNIT TEST PASSED] FunctionalEvaluator" << endl;
}

void testRegexEvaluator() {
    BenchmarkTask task;
    task.id = "TEST_REGEX";
    task.expected_output = R"(\d{4}-\d{2}-\d{2})";

    RegexEvaluator evaluator;
    Trajectory traj("TEST_REGEX");

    EvalResult res1 = evaluator.evaluate(task, "Ngày tạo báo cáo: 2026-08-12", traj);
    assert(res1.passed == true);

    cout << "[UNIT TEST PASSED] RegexEvaluator" << endl;
}

int main() {
    cout << "CHẠY UNIT TESTS CHO HARNESS MODULE" << endl;
    testKeywordEvaluator();
    testFunctionalEvaluator();
    testRegexEvaluator();
    cout << "UNIT TESTS ĐÃ THÀNH CÔNG" << endl;
    return 0;
}