#pragma once
#include <string>
#include <vector>
#include <chrono>

// Cấu trúc lưu vết từng bước chạy ReAct của Agent
struct AgentStep {
    int step_number;
    std::string thought;        // LLM suy nghĩ gì
    std::string action_name;    // LLM gọi tool gì
    std::string action_args;    // Đối số truyền vào tool
    std::string observation;    // Kết quả tool trả về
    long long latency_ms;       // Thời gian thực thi bước này (ms)
};

// Cấu trúc mô tả một bài test
struct BenchmarkTask {
    std::string id;
    std::string description;
    std::string expected_output;
    std::vector<std::string> keywords; // Dùng cho KeywordEvaluator
    std::string eval_type;             // "keyword" hoặc "functional"
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

    void addStep(const AgentStep& step);
    void setFinalStatus(bool success, double calculated_score);
    
    // Xuất dữ liệu ra chuỗi JSON
    std::string toJsonString() const;
};