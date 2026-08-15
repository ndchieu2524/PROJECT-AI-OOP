#include "trajectory.h"
#include <fstream>
#include <iostream>
#include <utility>

Trajectory::Trajectory(std::string id) : task_id(std::move(id)) {}

void Trajectory::addStep(const AgentStep& step) {
    steps.push_back(step);
    total_latency_ms += step.latency_ms;
}

void Trajectory::setFinalStatus(bool success, double calculated_score) {
    is_success = success;
    score = calculated_score;
}

const std::string& Trajectory::getTaskId() const { return task_id; }
const std::vector<AgentStep>& Trajectory::getSteps() const { return steps; }
bool Trajectory::getIsSuccess() const { return is_success; }
bool Trajectory::isPassed() const { return is_success; }
double Trajectory::getScore() const { return score; }
double Trajectory::getFinalScore() const { return score; }
long long Trajectory::getTotalLatencyMs() const { return total_latency_ms; }

json Trajectory::toJsonObject() const {
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

std::string Trajectory::toJsonString() const {
    return toJsonObject().dump(4);
}

bool Trajectory::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[LỖI] Không thể mở file để lưu Trajectory: " << filepath << std::endl;
        return false;
    }
    file << toJsonString();
    file.close();
    std::cout << "[HARNESS] Đã lưu Trajectory vào file: " << filepath << std::endl;
    return true;
}