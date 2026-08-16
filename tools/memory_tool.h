#pragma once

#include <string>

#include "tool.h"

struct sqlite3;  // forward-declare, tránh include sqlite3.h trong header

namespace agent::tools {

    // MemoryTool: lưu trữ và truy vấn "bộ nhớ dài hạn" cho Agent bằng
    // SQLite (persistent, không mất khi tắt chương trình).
    //
    // Actions hỗ trợ (tham số "action"):
    //   - "memory_save"   : lưu 1 đoạn ghi nhớ -> cần "content"
    //   - "memory_search" : tìm ghi nhớ chứa từ khoá -> cần "query"
    class MemoryTool : public Tool {
    public:
        // dbPath: đường dẫn file SQLite (VD: "agent_memory.db").
        explicit MemoryTool(std::string dbPath);
        ~MemoryTool() override;

        // Không cho copy vì đang giữ 1 con trỏ sqlite3* duy nhất.
        MemoryTool(const MemoryTool&) = delete;
        MemoryTool& operator=(const MemoryTool&) = delete;

        std::string name() const override;
        std::string description() const override;
        nlohmann::json parametersSchema() const override;
        nlohmann::json execute(const nlohmann::json& args) override;

    private:
        std::string dbPath_;
        sqlite3* db_ = nullptr;

        void ensureSchema();
        nlohmann::json saveMemory(const std::string& content);
        nlohmann::json searchMemory(const std::string& query, int limit) const;
    };

}  // namespace agent::tools