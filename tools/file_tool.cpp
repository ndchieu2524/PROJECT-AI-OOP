#include "file_tool.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

FileTool::FileTool(fs::path sandboxRoot)
    : sandboxRoot_(fs::absolute(std::move(sandboxRoot))) {
    fs::create_directories(sandboxRoot_);
    sandboxRoot_ = fs::weakly_canonical(sandboxRoot_);
}

std::string FileTool::name() const { return "file"; }

std::string FileTool::description() const {
    return "Đọc, ghi, hoặc liệt kê file/thư mục trong khu vực làm việc được phép. action = 'read' | 'write' | 'list'.";
}

nlohmann::json FileTool::parametersSchema() const {
    return {
        {"type", "object"},
        {"properties", {
            {"action", {{"type", "string"}, {"enum", nlohmann::json::array({"read", "write", "list"})}}},
            {"path", {{"type", "string"}, {"description", "Đường dẫn tương đối trong khu vực làm việc"}}},
            {"content", {{"type", "string"}, {"description", "Nội dung để ghi (chỉ cần khi action='write')"}}}
        }},
        {"required", nlohmann::json::array({"action", "path"})}
    };
}

fs::path FileTool::resolveSafePath(const std::string& relativePath) const {
    fs::path candidate = fs::weakly_canonical(sandboxRoot_ / relativePath);
    auto rootStr = sandboxRoot_.string();
    auto candidateStr = candidate.string();
    if (candidateStr.compare(0, rootStr.size(), rootStr) != 0) {
        throw std::runtime_error("Đường dẫn nằm ngoài khu vực làm việc cho phép: " + relativePath);
    }
    return candidate;
}

nlohmann::json FileTool::execute(const nlohmann::json& args) {
    if (!args.contains("action") || !args.contains("path")) {
        return {{"success", false}, {"error", "Thiếu tham số 'action' hoặc 'path'"}};
    }

    std::string action = args["action"].get<std::string>();
    std::string relativePath = args["path"].get<std::string>();

    try {
        fs::path target = resolveSafePath(relativePath);

        if (action == "read") {
            if (!fs::exists(target) || !fs::is_regular_file(target)) {
                return {{"success", false}, {"error", "File không tồn tại: " + relativePath}};
            }
            std::ifstream in(target);
            std::ostringstream buf;
            buf << in.rdbuf();
            return {{"success", true}, {"output", buf.str()}};
        }

        if (action == "write") {
            if (!args.contains("content")) {
                return {{"success", false}, {"error", "Thiếu tham số 'content' cho action='write'"}};
            }
            fs::create_directories(target.parent_path());
            std::ofstream out(target, std::ios::trunc);
            out << args["content"].get<std::string>();
            return {{"success", true}, {"output", "Đã ghi vào " + relativePath}};
        }

        if (action == "list") {
            if (!fs::exists(target) || !fs::is_directory(target)) {
                return {{"success", false}, {"error", "Thư mục không tồn tại: " + relativePath}};
            }
            nlohmann::json entries = nlohmann::json::array();
            for (const auto& entry : fs::directory_iterator(target)) {
                entries.push_back(entry.path().filename().string());
            }
            return {{"success", true}, {"output", entries}};
        }

        return {{"success", false}, {"error", "action không hợp lệ: " + action}};

    } catch (const std::exception& e) {
        return {{"success", false}, {"error", std::string("Lỗi: ") + e.what()}};
    }
}
