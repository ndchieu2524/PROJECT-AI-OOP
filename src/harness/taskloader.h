#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "trajectory.h"

using namespace std;
using json = nlohmann::json;

class TaskLoader {
public:
    static vector<BenchmarkTask> loadFromJsonFile(const string& filepath) {
        vector<BenchmarkTask> tasks;
        ifstream file(filepath);

        if (!file.is_open()) {
            cerr << "[LỖI HARNESS] Không thể mở file bài test: " << filepath << endl;
            return tasks;
        }

        try {
            json j;
            file >> j;
            file.close();

            for (const auto& item : j) {
                BenchmarkTask task;
                task.id = item.value("id", "");
                task.description = item.value("description", "");
                task.expected_output = item.value("expected_output", "");
                task.eval_type = item.value("eval_type", "keyword");

                if (item.contains("keywords") && item["keywords"].is_array()) {
                    task.keywords = item["keywords"].get<vector<string>>();
                } else if (!task.expected_output.empty()) {
                    task.keywords.push_back(task.expected_output);
                }

                if (!task.id.empty()) {
                    tasks.push_back(task);
                }
            }
        } catch (const json::exception& e) {
            cerr << "[LỖI HARNESS] Lỗi cú pháp JSON trong file " << filepath << ": " << e.what() << endl;
            return tasks;
        }

        cout << "[HARNESS] Đã tải thành công " << tasks.size() << " bài test từ file: " << filepath << endl;
        return tasks;
    }
};