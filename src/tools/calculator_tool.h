#pragma once

#include "tool.h"

namespace agent::tools {

    // CalculatorTool: tính toán biểu thức số học (+, -, *, /, dấu ngoặc,
    // số thập phân) một cách an toàn bằng recursive-descent parser tự
    // viết — không dùng eval() hay gọi shell, để tránh rủi ro thực thi
    // mã tuỳ ý.
    class CalculatorTool : public Tool {
    public:
        std::string name() const override;
        std::string description() const override;
        nlohmann::json parametersSchema() const override;
        nlohmann::json execute(const nlohmann::json& args) override;
    };

}  // namespace agent::tools