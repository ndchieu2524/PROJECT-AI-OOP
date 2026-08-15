#pragma once
#include <string>
#include <vector>
#include <memory>
#include "trajectory.h"
#include "i_agent.h"

namespace Color {
    extern const std::string RESET;
    extern const std::string RED;
    extern const std::string GREEN;
    extern const std::string YELLOW;
    extern const std::string CYAN;
    extern const std::string BOLD;
}

struct BenchmarkReport {
    int total_tasks{0};
    int passed_tasks{0};
    double success_rate{0.0};
    double average_score{0.0};
    long long total_latency_ms{0};
    double average_latency_ms{0.0};
    long long min_latency_ms{0};
    long long max_latency_ms{0};
    double average_steps_per_task{0.0};

    bool saveToJsonFile(const std::string& filepath) const;
    bool saveToMarkdownFile(const std::string& filepath) const;
};

class HarnessRunner {
private:
    std::vector<BenchmarkTask> tasks;
    std::shared_ptr<IAgent> agent;

public:
    void setTasks(const std::vector<BenchmarkTask>& loaded_tasks);
    void setAgent(std::shared_ptr<IAgent> target_agent);
    BenchmarkReport runBenchmarkSuite(const std::string& report_filepath = "benchmark_report.json");
};