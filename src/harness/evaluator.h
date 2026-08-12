#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include <cmath>
#include "trajectory.h"
using namespace std;

struct EvalResult {
    bool passed;
    double score;
    string feedback;
};

class Evaluator {
public:
    virtual ~Evaluator() = default;
    virtual EvalResult evaluate(const BenchmarkTask& task, 
                               const string& actual_output, 
                               const Trajectory& trajectory) = 0;
};

class KeywordEvaluator : public Evaluator {
public:
    EvalResult evaluate(const BenchmarkTask& task, 
                        const string& actual_output, 
                        const Trajectory& trajectory) override {
        if (task.keywords.empty()) {
            bool match = (actual_output.find(task.expected_output) != string::npos);
            return {match, match ? 1.0 : 0.0, match ? "Trùng khớp" : "Không khớp"};
        }

        int found_count = 0;
        for (const auto& kw : task.keywords) {
            if (actual_output.find(kw) != string::npos) {
                found_count++;
            }
        }

        double score = static_cast<double>(found_count) / task.keywords.size();
        bool passed = (score >= 0.8);

        return {
            passed, 
            score, 
            "Tìm thấy " + to_string(found_count) + "/" + to_string(task.keywords.size()) + " keywords"
        };
    }
};

class FunctionalEvaluator : public Evaluator {
private:
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (string::npos == first) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

public:
    EvalResult evaluate(const BenchmarkTask& task, 
                        const string& actual_output, 
                        const Trajectory& trajectory) override {
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
        } catch (...) {
        }

        bool match = (actual_clean.find(expected_clean) != string::npos);

        return {
            match,
            match ? 1.0 : 0.0,
            match ? "Kết quả chức năng chính xác" : "Kết quả chức năng SAI!"
        };
    }
};

class RegexEvaluator : public Evaluator {
public:
    EvalResult evaluate(const BenchmarkTask& task, 
                        const string& actual_output, 
                        const Trajectory& trajectory) override {
        try {
            regex reg(task.expected_output);
            bool match = regex_search(actual_output, reg);
            return {
                match,
                match ? 1.0 : 0.0,
                match ? "Khớp định dạng Regex yêu cầu!" : "Không khớp định dạng Regex."
            };
        } catch (const regex_error& e) {
            return {false, 0.0, string("Lỗi biểu thức Regex: ") + e.what()};
        }
    }
};