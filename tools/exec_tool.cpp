#include "exec_tool.h"

#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/wait.h>  // WEXITSTATUS (POSIX only)

ExecTool::ExecTool(int timeoutSeconds) : timeoutSeconds_(timeoutSeconds) {}

std::string ExecTool::name() const { return "exec"; }

std::string ExecTool::description() const {
    return "Thực thi một lệnh shell trên hệ thống và trả về stdout, "
           "stderr, và exit code. Dùng khi cần chạy script, kiểm tra "
           "môi trường, hoặc thao tác mà các tool khác không hỗ trợ.";
}

nlohmann::json ExecTool::parametersSchema() const {
    return {
        {"type", "object"},
        {"properties", {
            {"command", {
                {"type", "string"},
                {"description", "Câu lệnh shell cần thực thi"}
            }}
        }},
        {"required", nlohmann::json::array({"command"})}
    };
}

nlohmann::json ExecTool::execute(const nlohmann::json& args) {
    if (!args.contains("command") || !args["command"].is_string()) {
        return {
            {"success", false},
            {"error", "Thiếu tham số 'command' (kiểu string)"}
        };
    }

    std::string command = args["command"].get<std::string>();

    // Nếu có timeout, bọc lệnh bằng tiện ích `timeout` của Linux.
    if (timeoutSeconds_ > 0) {
        command = "timeout " + std::to_string(timeoutSeconds_) + " " + command;
    }

    // Gộp stderr vào stdout để capture cả hai qua popen.
    std::string fullCommand = command + " 2>&1";

    std::array<char, 4096> buffer{};
    std::string output;

    FILE* pipe = popen(fullCommand.c_str(), "r");
    if (!pipe) {
        return {
            {"success", false},
            {"error", "Không thể khởi tạo tiến trình con (popen thất bại)"}
        };
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output += buffer.data();
    }

    int status = pclose(pipe);
    int exitCode = WEXITSTATUS(status);

    return {
        {"success", exitCode == 0},
        {"output", output},
        {"exit_code", exitCode}
    };
}