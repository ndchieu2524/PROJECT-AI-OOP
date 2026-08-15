#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "tool.h"

namespace agent::tools {

    // ToolRegistry quản lý tập hợp các Tool khả dụng trong hệ thống.
    // - Cho phép đăng ký tool mới (registerTool).
    // - Cung cấp danh sách schema của tất cả tool để gửi cho LLM
    //   (listToolSchemas) — dùng để đổ vào agent::llm::Options::tools.
    // - Dispatch lời gọi tool theo tên (execute) — dùng khi xử lý
    //   agent::llm::ToolCall trả về từ agent::llm::Response::toolCalls.
    class ToolRegistry {
    public:
        // Đăng ký một tool mới. Nếu đã tồn tại tool cùng tên, tool cũ sẽ
        // bị ghi đè.
        void registerTool(std::shared_ptr<Tool> tool);

        // Kiểm tra tool có tồn tại trong registry hay không.
        bool hasTool(const std::string& toolName) const;

        // Lấy con trỏ tới tool theo tên. Trả về nullptr nếu không tìm thấy.
        std::shared_ptr<Tool> getTool(const std::string& toolName) const;

        // Danh sách tên tất cả tool đã đăng ký.
        std::vector<std::string> listToolNames() const;

        // Trả về mảng JSON mô tả tất cả tool (name + description +
        // parameters), dùng để gán vào agent::llm::Options::tools.
        nlohmann::json listToolSchemas() const;

        // Thực thi tool theo tên với tham số đầu vào dạng JSON.
        // Nếu tool không tồn tại, trả về JSON lỗi thay vì throw, để
        // Harness/Evaluator có thể xử lý gracefully.
        nlohmann::json execute(const std::string& toolName,
                                const nlohmann::json& args) const;

    private:
        std::unordered_map<std::string, std::shared_ptr<Tool>> tools_;
    };

}  // namespace agent::tools