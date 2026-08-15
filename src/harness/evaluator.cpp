#include "evaluator.h"
#include <algorithm>
#include <regex>
#include <cmath>
using namespace std;

static string toLower(string data) {
    transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return tolower(c); });
    return data;
}

EvalResult KeywordEvaluator::evaluate(const BenchmarkTask& task, 
                                       const string& actual_output, 
                                       const Trajectory& trajectory) {
    if (task.keywords.empty()) {
        bool match = (toLower(actual_output).find(toLower(task.expected_output)) != string::npos);
        return {match, match ? 1.0 : 0.0, match ? "Khớp chính xác chuỗi" : "Không khớp chuỗi"};
    }

    int found_count = 0;
    string actual_lower = toLower(actual_output);

    for (const auto& kw : task.keywords) {
        if (actual_lower.find(toLower(kw)) != string::npos) {
            found_count++;
        }
    }

    double score = static_cast<double>(found_count) / task.keywords.size();
    bool passed = (score >= 0.8);

    return {
        passed, 
        score, 
        "Tìm thấy " + to_string(found_count) + "/" + to_string(task.keywords.size()) + " từ khóa yêu cầu"
    };
}

string FunctionalEvaluator::trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

EvalResult FunctionalEvaluator::evaluate(const BenchmarkTask& task, 
                                         const string& actual_output, 
                                         const Trajectory& trajectory) {
    string expected_clean = trim(task.expected_output);
    string actual_clean = trim(actual_output);

    try {
        double exp_val = stod(expected_clean);
        smatch match;
        regex num_regex(R"(-?\d+(\.\d+)?)");
        if (regex_search(actual_clean, match, num_regex)) {
            double act_val = stod(match.str());
            if (abs(act_val - exp_val) <= 0.001) {
                return {true, 1.0, "Kết quả số chính xác (<= 0.001)"};
            }
        }
    } catch (...) {}

    bool match = (actual_clean.find(expected_clean) != string::npos);
    return {match, match ? 1.0 : 0.0, match ? "Kết quả chức năng CHÍNH XÁC" : "Kết quả chức năng SAI"};
}

EvalResult RegexEvaluator::evaluate(const BenchmarkTask& task, 
                                     const string& actual_output, 
                                     const Trajectory& trajectory) {
    try {
        regex reg(task.expected_output);
        bool match = regex_search(actual_output, reg);
        return {match, match ? 1.0 : 0.0, match ? "Khớp định dạng Regex yêu cầu!" : "Không khớp định dạng Regex"};
    } catch (const regex_error& e) {
        return {false, 0.0, string("Lỗi biểu thức Regex: ") + e.what()};
    }
}