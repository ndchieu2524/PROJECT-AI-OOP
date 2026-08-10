#pragma once

#include <string>
#include <nlohmann/json.hpp>

// Interface (abstract base class) cho mọi Tool trong hệ thống.
// Mỗi Tool cụ thể (ExecTool, FileTool, CalculatorTool, ...) kế thừa
// class này và implement đầy đủ các phương thức thuần ảo bên dưới.
//
// Thiết kế theo hướng "function-calling": Agent (qua LlmClient) sẽ nhận
// mô tả các Tool (name + description + parametersSchema) để LLM quyết
// định gọi tool nào, sau đó Harness gọi ToolRegistry::execute(...).
class Tool {
public:
    virtual ~Tool() = default;

    // Tên định danh duy nhất của tool, dùng làm key trong ToolRegistry
    // và trong lời gọi function-calling của LLM. VD: "exec", "file",
    // "calculator".
    virtual std::string name() const = 0;

    // Mô tả ngắn gọn, dễ hiểu để đưa vào system prompt / tool schema
    // cho LLM biết khi nào nên dùng tool này.
    virtual std::string description() const = 0;

    // JSON Schema mô tả các tham số đầu vào mà tool chấp nhận.
    // VD cho CalculatorTool:
    // {
    //   "type": "object",
    //   "properties": {
    //     "expression": { "type": "string", "description": "Biểu thức toán học" }
    //   },
    //   "required": ["expression"]
    // }
    virtual nlohmann::json parametersSchema() const = 0;

    // Thực thi tool với tham số đầu vào (đã được LLM sinh ra dưới dạng
    // JSON, khớp với parametersSchema()). Trả về kết quả dạng JSON.
    //
    // Quy ước kết quả trả về:
    // {
    //   "success": true/false,
    //   "output": <bất kỳ kiểu nào, tuỳ tool>,
    //   "error": "<mô tả lỗi nếu success == false>"
    // }
    virtual nlohmann::json execute(const nlohmann::json& args) = 0;
};