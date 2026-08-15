#include "taskloader.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

vector<BenchmarkTask> TaskLoader::loadFromJsonFile(const string& filepath) {
    vector<BenchmarkTask> tasks;
    ifstream file(filepath);

    if (!file.is_open()) {
        cerr << "[LỖI] Không thể mở file task benchmark: " << filepath << endl;
        return tasks;
    }

    try {
        json j;
        file >> j;

        for (const auto& item : j) {
            BenchmarkTask task;
            task.id = item.value("id", "TASK_UNKNOWN");
            task.description = item.value("description", "");
            task.expected_output = item.value("expected_output", "");
            task.eval_type = item.value("eval_type", "keyword");
            task.timeout_seconds = item.value("timeout_seconds", 10);

            if (item.contains("keywords") && item["keywords"].is_array()) {
                for (const auto& kw : item["keywords"]) {
                    task.keywords.push_back(kw.get<string>());
                }
            }

            tasks.push_back(task);
        }
        cout << "[HARNESS] Nạp thành công " << tasks.size() << " task từ " << filepath << endl;
    } catch (const exception& e) {
        cerr << "[LỖI] Xử lý file JSON thất bại: " << e.what() << endl;
    }

    file.close();
    return tasks;
}