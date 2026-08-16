#include "memory_tool.h"

#include <sqlite3.h>

#include <stdexcept>

using namespace agent::tools;

MemoryTool::MemoryTool(std::string dbPath) : dbPath_(std::move(dbPath)) {
    if (sqlite3_open(dbPath_.c_str(), &db_) != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "unknown error";
        if (db_) sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("Không thể mở SQLite database: " + err);
    }
    ensureSchema();
}

MemoryTool::~MemoryTool() {
    if (db_) sqlite3_close(db_);
}

void MemoryTool::ensureSchema() {
    const char* createTableSql =
        "CREATE TABLE IF NOT EXISTS memories ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  content TEXT NOT NULL,"
        "  created_at TEXT DEFAULT CURRENT_TIMESTAMP"
        ");";

    char* errMsg = nullptr;
    if (sqlite3_exec(db_, createTableSql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("Không thể tạo bảng memories: " + err);
    }
}

std::string MemoryTool::name() const { return "memory"; }

std::string MemoryTool::description() const {
    return "Lưu trữ (memory_save) hoặc tìm kiếm (memory_search) thông "
           "tin trong bộ nhớ dài hạn của Agent, dùng SQLite persistent "
           "để không mất dữ liệu giữa các lần chạy.";
}

nlohmann::json MemoryTool::parametersSchema() const {
    return {
        {"type", "object"},
        {"properties", {
            {"action", {
                {"type", "string"},
                {"enum", nlohmann::json::array({"memory_save", "memory_search"})}
            }},
            {"content", {
                {"type", "string"},
                {"description", "Nội dung cần lưu (chỉ cần khi action='memory_save')"}
            }},
            {"query", {
                {"type", "string"},
                {"description", "Từ khoá tìm kiếm (chỉ cần khi action='memory_search')"}
            }},
            {"limit", {
                {"type", "integer"},
                {"description", "Số kết quả tối đa khi tìm kiếm (mặc định 10)"}
            }}
        }},
        {"required", nlohmann::json::array({"action"})}
    };
}

nlohmann::json MemoryTool::saveMemory(const std::string& content) {
    const char* insertSql = "INSERT INTO memories (content) VALUES (?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_, insertSql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {{"success", false}, {"error", std::string("Lỗi prepare statement: ") + sqlite3_errmsg(db_)}};
    }

    sqlite3_bind_text(stmt, 1, content.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_int64 insertedId = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);

    if (!ok) {
        return {{"success", false}, {"error", std::string("Lỗi khi lưu memory: ") + sqlite3_errmsg(db_)}};
    }

    return {{"success", true}, {"output", {{"id", insertedId}, {"content", content}}}};
}

nlohmann::json MemoryTool::searchMemory(const std::string& query, int limit) const {
    const char* selectSql =
        "SELECT id, content, created_at FROM memories "
        "WHERE content LIKE ? ORDER BY id DESC LIMIT ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {{"success", false}, {"error", std::string("Lỗi prepare statement: ") + sqlite3_errmsg(db_)}};
    }

    std::string likePattern = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, likePattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, limit);

    nlohmann::json results = nlohmann::json::array();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back({
            {"id", sqlite3_column_int64(stmt, 0)},
            {"content", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))},
            {"created_at", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))}
        });
    }
    sqlite3_finalize(stmt);

    return {{"success", true}, {"output", results}};
}

nlohmann::json MemoryTool::execute(const nlohmann::json& args) {
    if (!args.contains("action") || !args["action"].is_string()) {
        return {{"success", false}, {"error", "Thiếu tham số 'action' (kiểu string)"}};
    }

    std::string action = args["action"].get<std::string>();

    if (action == "memory_save") {
        if (!args.contains("content") || !args["content"].is_string()) {
            return {{"success", false}, {"error", "Thiếu tham số 'content' cho action='memory_save'"}};
        }
        return saveMemory(args["content"].get<std::string>());
    }

    if (action == "memory_search") {
        if (!args.contains("query") || !args["query"].is_string()) {
            return {{"success", false}, {"error", "Thiếu tham số 'query' cho action='memory_search'"}};
        }
        int limit = args.value("limit", 10);
        return searchMemory(args["query"].get<std::string>(), limit);
    }

    return {{"success", false}, {"error", "action không hợp lệ: " + action}};
}