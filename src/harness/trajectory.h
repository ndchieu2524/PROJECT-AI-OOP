#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

// Cấu trúc lưu vết từng bước chạy ReAct của Agent
struct AgentStep {
    int step_number;
    std::string thought;
    std::string action_name;
    std::string action_args;
    std::string observation;
    long long latency_ms;
};

// Cấu trúc mô tả một bài test
struct BenchmarkTask {
    std::string id;
    std::string description;
    std::string expected_output;
    std::vector<std::string> keywords;
    std::string eval_type;
};

// Lớp quản lý hộp đen Trajectory
class Trajectory {
private:
    std::string task_id;
    std::vector<AgentStep> steps;
    bool is_success{false};
    double score{0.0};
    long long total_latency_ms{0};

public:
    explicit Trajectory(std::string id) : task_id(std::move(id)) {}

    void addStep(const AgentStep& step) {
        steps.push_back(step);
        total_latency_ms += step.latency_ms;
    }

    void setFinalStatus(bool success, double calculated_score) {
        is_success = success;
        score = calculated_score;
    }

    // Getters
    const std::string& getTaskId() const { return task_id; }
    const std::vector<AgentStep>& getSteps() const { return steps; }
    bool getIsSuccess() const { return is_success; }
    double getScore() const { return score; }
    long long getTotalLatencyMs() const { return total_latency_ms; }

    // Chuyển đổi toàn bộ vết chạy thành định dạng JSON string
    std::string toJsonString() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"task_id\": \"" << task_id << "\",\n";
        ss << "  \"is_success\": " << (is_success ? "true" : "false") << ",\n";
        ss << "  \"score\": " << score << ",\n";
        ss << "  \"total_latency_ms\": " << total_latency_ms << ",\n";
        ss << "  \"steps\": [\n";
        for (size_t i = 0; i < steps.size(); ++i) {
            const auto& s = steps[i];
            ss << "    {\n";
            ss << "      \"step_number\": " << s.step_number << ",\n";
            ss << "      \"thought\": \"" << s.thought << "\",\n";
            ss << "      \"action_name\": \"" << s.action_name << "\",\n";
            ss << "      \"action_args\": \"" << s.action_args << "\",\n";
            ss << "      \"observation\": \"" << s.observation << "\",\n";
            ss << "      \"latency_ms\": " << s.latency_ms << "\n";
            ss << "    }" << (i + 1 < steps.size() ? "," : "") << "\n";
        }
        ss << "  ]\n";
        ss << "}";
        return ss.str();
    }

    bool saveToJsonFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Khong the mo file de ghi: " << filepath << std::endl;
            return false;
        }
        file << toJsonString();
        file.close();
        std::cout << "[HARNESS] Da luu trajectory vao file: " << filepath << std::endl;
        return true;
    }
};