#pragma once

#include "tool.h"

namespace agent::tools {

    // ExecTool: cho phép Agent chạy một câu lệnh shell và lấy lại
    // stdout/stderr/exit code.
    //
    // LƯU Ý AN TOÀN: đây là tool mạnh nhất trong hệ thống vì nó thực thi
    // lệnh tuỳ ý trên máy host. Trong đồ án, nên:
    //   - Giới hạn thời gian chạy (timeout) để tránh treo tiến trình.
    //   - Cân nhắc chạy trong sandbox/container khi tích hợp thật.
    //   - Log lại mọi lệnh được thực thi để phục vụ Evaluator/trajectory.
    class ExecTool : public Tool {
    public:
        // timeoutSeconds: giới hạn thời gian chạy lệnh, 0 = không giới hạn.
        explicit ExecTool(int timeoutSeconds = 10);

        std::string name() const override;
        std::string description() const override;
        nlohmann::json parametersSchema() const override;
        nlohmann::json execute(const nlohmann::json& args) override;

    private:
        int timeoutSeconds_;
    };

}  // namespace agent::tools