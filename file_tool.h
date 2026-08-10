#pragma once

#include <filesystem>

#include "tool.h"

// FileTool: đọc, ghi, và liệt kê file trong một thư mục gốc (sandbox)
// được chỉ định khi khởi tạo. Mọi đường dẫn tương đối trong tham số
// "path" sẽ được resolve bên trong thư mục này, để tránh Agent truy
// cập file ngoài phạm vi cho phép (VD: "../../etc/passwd").
//
// Actions hỗ trợ (tham số "action"):
//   - "read"  : đọc nội dung file    -> cần thêm "path"
//   - "write" : ghi nội dung vào file -> cần thêm "path", "content"
//   - "list"  : liệt kê file/thư mục  -> cần thêm "path" (thư mục)
class FileTool : public Tool {
public:
    explicit FileTool(std::filesystem::path sandboxRoot);

    std::string name() const override;
    std::string description() const override;
    nlohmann::json parametersSchema() const override;
    nlohmann::json execute(const nlohmann::json& args) override;

private:
    std::filesystem::path sandboxRoot_;

    // Resolve đường dẫn tương đối vào trong sandboxRoot_, throw nếu
    // đường dẫn cố gắng thoát ra ngoài sandbox.
    std::filesystem::path resolveSafePath(const std::string& relativePath) const;
};