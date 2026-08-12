#pragma once
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include "trajectory.h"
#include "evaluator.h"
#include "i_agent.h"
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

namespace Color {
    const string RESET   = "\033[0m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string CYAN    = "\033[36m";
    const string BOLD    = "\033[1m";
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

    bool saveToJsonFile(const string& filepath) const {
        ofstream file(filepath);
        if (!file.is_open()) {
            cerr << Color::RED << "[LỖI] Không thể lưu file: " << filepath << endl;
            return false;
        }

        json j;
        j["metrics"] = {
            {"total_tasks", total_tasks},
            {"passed_tasks", passed_tasks},
            {"success_rate_percent", success_rate},
            {"average_score_percent", average_score},
            {"total_latency_ms", total_latency_ms},
            {"average_latency_ms", average_latency_ms},
            {"min_latency_ms", min_latency_ms},
            {"max_latency_ms", max_latency_ms},
            {"average_steps_per_task", average_steps_per_task}
        };

        file << j.dump(4);
        file.close();
        cout << Color::GREEN << "[HARNESS] Đã xuất file Báo cáo tổng hợp: " << filepath << Color::RESET << endl;
        return true;
    }
};

class HarnessRunner {
private:
    vector<BenchmarkTask> tasks;
    shared_ptr<IAgent> agent;
public:
    void setAgent(shared_ptr<IAgent> target) {
        agent = target;
    }

    void setTasks(const vector<BenchmarkTask>& loaded_tasks) {
        tasks = loaded_tasks;
    }

    BenchmarkReport runBenchmarkSuite(const string& report_filepath = "benchmark_report.json") {
        BenchmarkReport report;
        report.total_tasks = static_cast<int>(tasks.size());

        if (tasks.empty()) {
            cout << Color::YELLOW << "[CẢNH BÁO HARNESS] Không có task nào trong danh sách benchmark!!!" << Color::RESET << endl;
            return report;
        }
        if (!agent) {
            cerr << Color::RED << "[LỖI HARNESS] Chưa gán Agent vào HarnessRunner! Vui lòng gọi setAgent()." << Color::RESET << endl;
            return report;
        }
        cout << "\n" << Color::CYAN << Color::BOLD << "==================================================" << endl;
        cout << "   BẮT ĐẦU CHẠY HỆ THỐNG BENCHMARK (" << report.total_tasks << " TASKS)" << endl;
        cout << "==================================================" << Color::RESET << endl;

        double total_score_sum = 0.0;
        int total_steps_sum = 0;
        report.min_latency_ms = -1;
        report.max_latency_ms = 0;

        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& task = tasks[i];
            cout << Color::BOLD << "\n[ĐANG CHẠY] Task " << (i + 1) << "/" << tasks.size() 
                 << " [" << task.id << "]: " << task.description << Color::RESET << endl;

            Trajectory traj(task.id);
            
            string actual_output = agent->run(task.description, traj);

            unique_ptr<Evaluator> evaluator;
            if (task.eval_type == "functional") {
                evaluator = make_unique<FunctionalEvaluator>();
            } else if (task.eval_type == "regex") {
                evaluator = make_unique<RegexEvaluator>();
            } else {
                evaluator = make_unique<KeywordEvaluator>();
            }

            EvalResult result = evaluator->evaluate(task, actual_output, traj);
            traj.setFinalStatus(result.passed, result.score);

            if (result.passed) report.passed_tasks++;
            total_score_sum += result.score;

            long long current_latency = traj.getTotalLatencyMs();
            report.total_latency_ms += current_latency;
            total_steps_sum += static_cast<int>(traj.getSteps().size());

            if (report.min_latency_ms == -1 || current_latency < report.min_latency_ms) {
                report.min_latency_ms = current_latency;
            }
            if (current_latency > report.max_latency_ms) {
                report.max_latency_ms = current_latency;
            }

            string filename = "trajectory_" + task.id + ".json";
            traj.saveToJsonFile(filename);

            string status_str = result.passed ? (Color::GREEN + "[ĐẠT]" + Color::RESET) : (Color::RED + "[THẤT BẠI]" + Color::RESET);

            cout << "   -> Đầu ra Agent : " << actual_output << endl;
            cout << "   -> Trạng thái   : " << status_str 
                 << " | Điểm số: " << (result.score * 100) << "%"
                 << " | Thời gian: " << current_latency << "ms" << endl;
            cout << "   -> Phản hồi     : " << result.feedback << endl;
        }

        report.success_rate = (static_cast<double>(report.passed_tasks) / report.total_tasks) * 100.0;
        report.average_score = (total_score_sum / report.total_tasks) * 100.0;
        report.average_latency_ms = static_cast<double>(report.total_latency_ms) / report.total_tasks;
        report.average_steps_per_task = static_cast<double>(total_steps_sum) / report.total_tasks;

        cout << "\n" << Color::CYAN << Color::BOLD << "==================================================" << endl;
        cout << "           BÁO CÁO TỔNG KẾT BENCHMARK             " << endl;
        cout << "==================================================" << Color::RESET << endl;
        cout << " Tổng số Task        : " << report.total_tasks << endl;
        cout << " Số Task đạt         : " << Color::GREEN << report.passed_tasks << Color::RESET 
             << " / " << report.total_tasks << endl;
        cout << " Tỷ lệ thành công    : " << (report.success_rate >= 80.0 ? Color::GREEN : Color::RED) 
             << report.success_rate << "%" << Color::RESET << endl;
        cout << " Điểm trung bình     : " << report.average_score << "%" << endl;
        cout << " Thời gian trung bình: " << report.average_latency_ms << " ms/task" << endl;
        cout << " Thời gian Min / Max : " << report.min_latency_ms << " ms / " << report.max_latency_ms << " ms" << endl;
        cout << " Số bước ReAct trung bình: " << report.average_steps_per_task << " bước/task" << endl;
        cout << Color::CYAN << "==================================================\n" << Color::RESET << endl;
        
        report.saveToJsonFile("benchmark_report.json");
        return report;
    }
};