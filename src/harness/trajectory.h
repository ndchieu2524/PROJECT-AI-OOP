#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct AgentStep {
    int step_number{0};
    std::string thought;
    std::string action_name;
    std::string action_args;
    std::string observation;
    long long latency_ms{0};
};

struct BenchmarkTask {
    std::string id;
    std::string description;
    std::string expected_output;
    std::vector<std::string> keywords;
    std::string eval_type{"keyword"};
    int timeout_seconds{10};
};

class Trajectory {
private:
    std::string task_id;
    std::vector<AgentStep> steps;
    bool is_success{false};
    double score{0.0};
    long long total_latency_ms{0};

public:
    explicit Trajectory(std::string id);

    void addStep(const AgentStep& step);
    void setFinalStatus(bool success, double calculated_score);

    const std::string& getTaskId() const;
    const std::vector<AgentStep>& getSteps() const;
    bool getIsSuccess() const;
    bool isPassed() const;
    double getScore() const;
    double getFinalScore() const;
    long long getTotalLatencyMs() const;

    json toJsonObject() const;
    std::string toJsonString() const;
    bool saveToJsonFile(const std::string& filepath) const;
};