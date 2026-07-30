#pragma once
#include <string>
#include <vector>
#include "trajectory.h"

struct EvalResult {
    bool passed;
    double score;
    std::string feedback;
};

class Evaluator {
public:
    virtual ~Evaluator() = default;
    virtual EvalResult evaluate(const BenchmarkTask& task, 
                               const std::string& actual_output, 
                               const Trajectory& trajectory) = 0;
};

class KeywordEvaluator : public Evaluator {
public:
    EvalResult evaluate(const BenchmarkTask& task, 
                        const std::string& actual_output, 
                        const Trajectory& trajectory) override {
        if (task.keywords.empty()) {
            bool match = (actual_output.find(task.expected_output) != std::string::npos);
            return {match, match ? 1.0 : 0.0, match ? "Trùng khớp" : "Không khớp"};
        }

        int found_count = 0;
        for (const auto& kw : task.keywords) {
            if (actual_output.find(kw) != std::string::npos) {
                found_count++;
            }
        }

        double score = static_cast<double>(found_count) / task.keywords.size();
        bool passed = (score >= 0.8);

        return {
            passed, 
            score, 
            "Matched " + std::to_string(found_count) + "/" + std::to_string(task.keywords.size()) + " keywords"
        };
    }
};