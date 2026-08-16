#pragma once

#include <string>

#include "tool.h"

namespace agent::tools {

    // WebSearchTool: tìm kiếm thông tin trên web, dùng DuckDuckGo
    // Instant Answer API (không cần API key, trả về JSON có sẵn).
    //
    // LƯU Ý: API này chỉ trả "instant answer" (tóm tắt nhanh) + các chủ
    // đề liên quan, không phải kết quả tìm kiếm đầy đủ như Google. Phù
    // hợp cho câu hỏi tra cứu nhanh (VD: "thủ đô nước Pháp"), không phù
    // hợp cho tìm kiếm chuyên sâu.
    class WebSearchTool : public Tool {
    public:
        // maxResults: giới hạn số related-topic trả về (mặc định 5).
        explicit WebSearchTool(int maxResults = 5);

        std::string name() const override;
        std::string description() const override;
        nlohmann::json parametersSchema() const override;
        nlohmann::json execute(const nlohmann::json& args) override;

    private:
        int maxResults_;

        // Gửi HTTP GET tới DuckDuckGo API, trả về raw JSON string.
        // throw std::runtime_error nếu request thất bại.
        std::string httpGet(const std::string& url) const;
    };

}  // namespace agent::tools