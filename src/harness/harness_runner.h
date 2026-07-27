#pragma once
#include "trajectory.h"
#include "evaluator.h"
#include <functional>
#include <vector>
#include <string>

using StepHookFn = std::function<void(const AgentStep&)>;

class HarnessRunner {
private:
    std::vector<BenchmarkTask> tasks;
    std::unique_ptr<Evaluator> keyword_evaluator;
    std::unique_ptr<Evaluator> functional_evaluator;

public:
    HarnessRunner();
    
    bool loadTasks(const std::string& json_filepath);

    StepHookFn createHookForTask(Trajectory& current_trajectory);

    void runBatchBenchmark();
};