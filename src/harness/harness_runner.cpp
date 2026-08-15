#include "harness_runner.h"
#include "evaluator.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <future>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

namespace Color {
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string CYAN = "\033[36m";
    const string BOLD = "\033[1m";
}

bool BenchmarkReport::saveToJsonFile(const string& filepath) const {
    ofstream file(filepath);
    if (!file.is_open()) return false;

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

bool BenchmarkReport::saveToMarkdownFile(const string& filepath) const {
    ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "BÁO CÁO KẾT QUẢ BENCHMARK AI AGENT\n\n";
    file << "TỔNG QUAN HIỆU NĂNG\n\n";
    file << "| Chỉ số | Giá trị |\n";
    file << "| :--- | :--- |\n";
    file << "| **Tổng số Task** | `" << total_tasks << "` |\n";
    file << "| **Số Task Đạt** | `" << passed_tasks << " / " << total_tasks << "` |\n";
    file << "| **Tỷ lệ thành công** | **" << success_rate << "%** |\n";
    file << "| **Điểm trung bình** | `" << average_score << "%` |\n";
    file << "| **Thời gian trung bình** | `" << average_latency_ms << " ms` |\n";
    file << "| **Thời gian Min / Max** | `" << min_latency_ms << " ms` / `" << max_latency_ms << " ms` |\n";
    file << "| **Số bước ReAct trung bình** | `" << average_steps_per_task << " bước` |\n\n";
    file << "---\n";
    file.close();
    cout << Color::GREEN << "[HARNESS] Đã xuất file Báo cáo Markdown: " << filepath << Color::RESET << endl;
    return true;
}

void HarnessRunner::setTasks(const vector<BenchmarkTask>& loaded_tasks) { tasks = loaded_tasks; }
void HarnessRunner::setAgent(shared_ptr<IAgent> target_agent) { agent = target_agent; }

BenchmarkReport HarnessRunner::runBenchmarkSuite(const string& report_filepath) {
    BenchmarkReport report;
    report.total_tasks = static_cast<int>(tasks.size());

    if (tasks.empty() || !agent) {
        cerr << Color::RED << "[LỖI HARNESS] Dữ liệu task rỗng hoặc chưa gán Agent!" << Color::RESET << endl;
        return report;
    }

    cout << "\n" << Color::CYAN << Color::BOLD << "==================================================" << endl;
    cout << "   BẮT ĐẦU CHẠY HỆ THỐNG BENCHMARK (" << report.total_tasks << " TASKS)" << endl;
    cout << "==================================================" << Color::RESET << endl;

    double total_score_sum = 0.0;
    int total_steps_sum = 0;
    report.min_latency_ms = -1;

    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& task = tasks[i];
        cout << "\n" << Color::BOLD << "[ĐANG CHẠY] Task " << (i + 1) << "/" << tasks.size() 
                  << " [" << task.id << "]: " << task.description << Color::RESET << endl;

        Trajectory traj(task.id);

        auto future = async(launch::async, [&]() {
            return agent->run(task.description, traj);
        });

        string actual_output;
        if (future.wait_for(chrono::seconds(task.timeout_seconds)) == future_status::timeout) {
            actual_output = "Agent vượt quá thời gian cho phép (" + to_string(task.timeout_seconds) + "s)";
        } else {
            actual_output = future.get();
        }

        unique_ptr<Evaluator> evaluator;
        if (task.eval_type == "functional") evaluator = make_unique<FunctionalEvaluator>();
        else if (task.eval_type == "regex") evaluator = make_unique<RegexEvaluator>();
        else evaluator = make_unique<KeywordEvaluator>();

        EvalResult result = evaluator->evaluate(task, actual_output, traj);
        traj.setFinalStatus(result.passed, result.score);

        if (result.passed) report.passed_tasks++;
        total_score_sum += result.score;
        
        long long current_latency = traj.getTotalLatencyMs();
        report.total_latency_ms += current_latency;
        total_steps_sum += static_cast<int>(traj.getSteps().size());

        if (report.min_latency_ms == -1 || current_latency < report.min_latency_ms) report.min_latency_ms = current_latency;
        if (current_latency > report.max_latency_ms) report.max_latency_ms = current_latency;

        traj.saveToJsonFile("trajectory_" + task.id + ".json");

        string status_str = result.passed ? (Color::GREEN + "[ĐẠT]" + Color::RESET) : (Color::RED + "[THẤT BẠI]" + Color::RESET);
        cout << "   -> Đầu ra Agent : " << actual_output << endl;
        cout << "   -> Trạng thái   : " << status_str << " | Điểm: " << (result.score * 100) << "% | Thời gian: " << current_latency << "ms" << endl;
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
    cout << " Số Task đạt         : " << Color::GREEN << report.passed_tasks << Color::RESET << " / " << report.total_tasks << endl;
    cout << " Tỷ lệ thành công    : " << (report.success_rate >= 80.0 ? Color::GREEN : Color::RED) << report.success_rate << "%" << Color::RESET << endl;
    cout << " Điểm trung bình     : " << report.average_score << "%" << endl;
    cout << " Thời gian trung bình: " << report.average_latency_ms << " ms/task" << endl;
    cout << " Thời gian Min / Max : " << report.min_latency_ms << " ms / " << report.max_latency_ms << " ms" << endl;
    cout << " Số bước ReAct trung bình: " << report.average_steps_per_task << " bước/task" << endl;
    cout << Color::CYAN << "==================================================\n" << Color::RESET << endl;
    report.saveToJsonFile(report_filepath);
    report.saveToMarkdownFile("benchmark_report.md");
    return report;
}