#pragma once
#include "trajectory.h"
#include <memory>

struct EvalResult {
    bool passed;
    double score;
    std::string reason;
};

// Interface
class Evaluator {
public:
    virtual ~Evaluator() = default;
    virtual EvalResult evaluate(const BenchmarkTask& task, const Trajectory& trajectory, const std::string& agent_final_output) = 0;
};

// Strategy 1: Chấm điểm dựa trên từ khóa
class KeywordEvaluator : public Evaluator {
public:
    EvalResult evaluate(const BenchmarkTask& task, const Trajectory& trajectory, const std::string& agent_final_output) override;
};

// Strategy 2: Chấm điểm dựa trên kiểm tra thực tế (File/Shell)
class FunctionalEvaluator : public Evaluator {
public:
    EvalResult evaluate(const BenchmarkTask& task, const Trajectory& trajectory, const std::string& agent_final_output) override;
};