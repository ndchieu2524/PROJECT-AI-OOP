#pragma once
#include <string>
#include <vector>
#include "trajectory.h"

class TaskLoader {
public:
    static std::vector<BenchmarkTask> loadFromJsonFile(const std::string& filepath);
};