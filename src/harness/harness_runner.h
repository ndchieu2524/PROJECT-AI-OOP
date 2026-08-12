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
            cerr << "[LỖI] Không thể lưu file: " << filepath << endl;
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
        cout << "[HARNESS] Đã xuất file Báo cáo tổng hợp: " << filepath << endl;
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

    BenchmarkReport runBenchmarkSuite() {
        BenchmarkReport report;
        report.total_tasks = static_cast<int>(tasks.size());

        if (tasks.empty()) {
            cout << "[CẢNH BÁO HARNESS] Không có task nào trong danh sách benchmark!!!" << endl;
            return report;
        }
        if (!agent) {
            cerr << "[LỖI HARNESS] Chưa gán Agent vào HarnessRunner! Vui lòng gọi setAgent()." << endl;
            return report;
        }
        cout << "\n==================================================" << endl;
        cout << "   BẮT ĐẦU CHẠY HỆ THỐNG BENCHMARK (" << report.total_tasks << " TASKS)" << endl;
        cout << "==================================================" << endl;

        double total_score_sum = 0.0;
        int total_steps_sum = 0;
        report.min_latency_ms = -1;
        report.max_latency_ms = 0;

        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& task = tasks[i];
            cout << "\n[ĐANG CHẠY] Task " << (i + 1) << "/" << tasks.size() 
                 << " [" << task.id << "]: " << task.description << endl;

            Trajectory traj(task.id);
            
            string actual_output = agent->run(task.description, traj);

            unique_ptr<Evaluator> evaluator;
            if (task.eval_type == "functional") {
                evaluator = make_unique<FunctionalEvaluator>();
            } else {
                evaluator = make_unique<KeywordEvaluator>();
            }

            EvalResult result = evaluator->evaluate(task, actual_output, traj);
            traj.setFinalStatus(result.passed, result.score);

            if (result.passed) report.passed_tasks++;
            total_score_sum += result.score;
            report.total_latency_ms += traj.getTotalLatencyMs();

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

            cout << "   -> Đầu ra Agent : " << actual_output << endl;
            cout << "   -> Trạng thái: " << (result.passed ? "[ĐẠT]" : "[THẤT BẠI]") 
                 << " | Điểm số: " << (result.score * 100) << "%"
                 << " | Thời gian: " << traj.getTotalLatencyMs() << "ms" << endl;
            cout << "   -> Phản hồi  : " << result.feedback << endl;
        }

        report.success_rate = (static_cast<double>(report.passed_tasks) / report.total_tasks) * 100.0;
        report.average_score = (total_score_sum / report.total_tasks) * 100.0;
        report.average_latency_ms = static_cast<double>(report.total_latency_ms) / report.total_tasks;
        report.average_steps_per_task = static_cast<double>(total_steps_sum) / report.total_tasks;

        cout << "\n==================================================" << endl;
        cout << "           BÁO CÁO TỔNG KẾT BENCHMARK             " << endl;
        cout << "==================================================" << endl;
        cout << " Tổng số Task     : " << report.total_tasks << endl;
        cout << " Số Task đạt      : " << report.passed_tasks << " / " << report.total_tasks << endl;
        cout << " Tỷ lệ thành công : " << report.success_rate << "%" << endl;
        cout << " Điểm trung bình  : " << report.average_score << "%" << endl;
        cout << " Thời gian trung bình: " << report.average_latency_ms << " ms/task" << endl;
        cout << " Thời gian Min / Max : " << report.min_latency_ms << " ms / " << report.max_latency_ms << " ms" << endl;
        cout << " Số bước ReAct trung bình: " << report.average_steps_per_task << " bước/task" << endl;
        cout << "==================================================\n" << endl;
        
        report.saveToJsonFile("benchmark_report.json");
        return report;
    }
};