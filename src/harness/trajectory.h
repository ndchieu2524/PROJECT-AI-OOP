#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <utility>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

struct AgentStep {
    int step_number{0};
    string thought;
    string action_name;
    string action_args;
    string observation;
    long long latency_ms{0};
};

struct BenchmarkTask {
    string id;
    string description;
    string expected_output;
    vector<string> keywords;
    string eval_type{"keyword"};
    int timeout_seconds{10};
};

class Trajectory {
private:
    string task_id;
    vector<AgentStep> steps;
    bool is_success{false};
    double score{0.0};
    long long total_latency_ms{0};

public:
    explicit Trajectory(string id) : task_id(move(id)) {}

    void addStep(const AgentStep& step) {
        steps.push_back(step);
        total_latency_ms += step.latency_ms;
    }

    void setFinalStatus(bool success, double calculated_score) {
        is_success = success;
        score = calculated_score;
    }

    const string& getTaskId() const { return task_id; }
    const vector<AgentStep>& getSteps() const { return steps; }
    bool getIsSuccess() const { return is_success; }
    bool isPassed() const { return is_success; }
    double getScore() const { return score; }
    double getFinalScore() const { return score; }
    long long getTotalLatencyMs() const { return total_latency_ms; }

    json toJsonObject() const {
        json j;
        j["task_id"] = task_id;
        j["is_success"] = is_success;
        j["score"] = score;
        j["total_latency_ms"] = total_latency_ms;

        json steps_arr = json::array();
        for (const auto& s : steps) {
            steps_arr.push_back({
                {"step_id", s.step_number},
                {"thought", s.thought},
                {"action_name", s.action_name},
                {"action_args", s.action_args},
                {"observation", s.observation},
                {"latency_ms", s.latency_ms}
            });
        }
        j["steps"] = steps_arr;
        return j;
    }

    string toJsonString() const {
        return toJsonObject().dump(4);
    }

    bool saveToJsonFile(const string& filepath) const {
        ofstream file(filepath);
        if (!file.is_open()) {
            cerr << "[ERROR] Không thể mở file để lưu Trajectory: " << filepath << endl;
            return false;
        }
        file << toJsonString();
        file.close();
        cout << "[HARNESS] Đã lưu Trajectory vào file: " << filepath << endl;
        return true;
    }
};