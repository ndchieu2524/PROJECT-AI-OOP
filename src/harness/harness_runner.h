#pragma once
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include "trajectory.h"
#include "evaluator.h"
using namespace std;

struct BenchmarkReport {
    int total_tasks{0};
    int passed_tasks{0};
    double success_rate{0.0};
    double average_score{0.0};
    long long total_latency_ms{0};
};

class HarnessRunner {
private:
    vector<BenchmarkTask> tasks;

public:
    void addTask(const BenchmarkTask& task) {
        tasks.push_back(task);
    }

    const vector<BenchmarkTask>& getTasks() const {
        return tasks;
    }

    BenchmarkReport runBenchmarkSuite() {
        BenchmarkReport report;
        report.total_tasks = tasks.size();

        if (tasks.empty()) {
            cout << "[CẢNH BÁO HARNESS] Không có task nào trong danh sách benchmark!" << endl;
            return report;
        }

        cout << "\n==================================================" << endl;
        cout << "   BẮT ĐẦU CHẠY HỆ THỐNG BENCHMARK (" << report.total_tasks << " TASKS)" << endl;
        cout << "==================================================" << endl;

        double total_score_sum = 0.0;

        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& task = tasks[i];
            cout << "\n[ĐANG CHẠY] Task " << (i + 1) << "/" << tasks.size() 
                 << " [" << task.id << "]: " << task.description << endl;

            Trajectory traj(task.id);

            // Giả lập từng bước chạy của Agent (Sau này sẽ gọi Agent thực tế)
            AgentStep step1{
                1,
                "Đang phân tích và thực hiện yêu cầu...",
                "calculator",
                "10+20",
                "30",
                120
            };
            traj.addStep(step1);

            // Tự động chọn Evaluator phù hợp
            unique_ptr<Evaluator> evaluator;
            if (task.eval_type == "functional") {
                evaluator = make_unique<FunctionalEvaluator>();
            } else {
                evaluator = make_unique<KeywordEvaluator>();
            }

            // Chấm điểm kết quả
            string actual_output = "Kết quả là 30"; // Output giả lập
            EvalResult result = evaluator->evaluate(task, actual_output, traj);
            traj.setFinalStatus(result.passed, result.score);

            // Cập nhật chỉ số báo cáo
            if (result.passed) report.passed_tasks++;
            total_score_sum += result.score;
            report.total_latency_ms += traj.getTotalLatencyMs();

            // Lưu file log Trajectory JSON cho từng task
            string filename = "trajectory_" + task.id + ".json";
            traj.saveToJsonFile(filename);

            cout << "   -> Trạng thái: " << (result.passed ? "[ĐẠT]" : "[THẤT BẠI]") 
                 << " | Điểm số: " << (result.score * 100) << "%"
                 << " | Thời gian: " << traj.getTotalLatencyMs() << "ms" << endl;
            cout << "   -> Phản hồi  : " << result.feedback << endl;
        }

        report.success_rate = (static_cast<double>(report.passed_tasks) / report.total_tasks) * 100.0;
        report.average_score = (total_score_sum / report.total_tasks) * 100.0;

        // In báo cáo tổng kết ra Console
        cout << "\n==================================================" << endl;
        cout << "           BÁO CÁO TỔNG KẾT BENCHMARK             " << endl;
        cout << "==================================================" << endl;
        cout << " Tổng số Task     : " << report.total_tasks << endl;
        cout << " Số Task đạt      : " << report.passed_tasks << " / " << report.total_tasks << endl;
        cout << " Tỷ lệ thành công : " << report.success_rate << "%" << endl;
        cout << " Điểm trung bình  : " << report.average_score << "%" << endl;
        cout << " Tổng thời gian   : " << report.total_latency_ms << " ms" << endl;
        cout << "==================================================\n" << endl;

        return report;
    }
};