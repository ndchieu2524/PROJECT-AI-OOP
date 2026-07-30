#pragma once
#include <vector>
#include <iostream>
#include "trajectory.h"
#include "evaluator.h"

class HarnessRunner {
private:
    std::vector<BenchmarkTask> tasks;

public:
    void addTask(const BenchmarkTask& task) {
        tasks.push_back(task);
    }

    const std::vector<BenchmarkTask>& getTasks() const {
        return tasks;
    }

    void runMockBenchmark() {
        KeywordEvaluator evaluator;

        for (const auto& task : tasks) {
            Trajectory traj(task.id);

            AgentStep mock_step{
                1,
                "Need to evaluate calculation",
                "calculator",
                "10+20",
                "30",
                150 // 150ms
            };
            traj.addStep(mock_step);

            std::string actual_output = "Kết quả = 30";
            
            // Chấm điểm
            EvalResult result = evaluator.evaluate(task, actual_output, traj);
            traj.setFinalStatus(result.passed, result.score);

            std::cout << "\n=== TASK RESULT: " << task.id << " ===" << std::endl;
            std::cout << traj.toJsonString() << std::endl;
        }
    }
};