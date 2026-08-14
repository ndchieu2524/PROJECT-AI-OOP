#pragma once
#include <string>
#include "trajectory.h"

struct EvalResult {
    bool passed{false};
    double score{0.0};
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
                        const Trajectory& trajectory) override;
};

class FunctionalEvaluator : public Evaluator {
private:
    std::string trim(const std::string& str);
public:
    EvalResult evaluate(const BenchmarkTask& task, 
                        const std::string& actual_output, 
                        const Trajectory& trajectory) override;
};

class RegexEvaluator : public Evaluator {
public:
    EvalResult evaluate(const BenchmarkTask& task, 
                        const std::string& actual_output, 
                        const Trajectory& trajectory) override;
};