#include "web_search_tool.h"

#include <curl/curl.h>

#include <nlohmann/json.hpp>
#include <stdexcept>

using namespace agent::tools;

namespace {
    // Callback libcurl dùng để gom dữ liệu response vào 1 std::string.
    size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
        auto* out = static_cast<std::string*>(userdata);
        out->append(ptr, size * nmemb);
        return size * nmemb;
    }

    // Encode 1 chuỗi UTF-8 thành dạng an toàn cho URL (dùng chính API
    // của libcurl để không cần tự viết percent-encoding).
    std::string urlEncode(const std::string& value) {
        CURL* curl = curl_easy_init();
        if (!curl) return value;
        char* encoded = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.size()));
        std::string result = encoded ? encoded : value;
        if (encoded) curl_free(encoded);
        curl_easy_cleanup(curl);
        return result;
    }
}  // namespace

WebSearchTool::WebSearchTool(int maxResults) : maxResults_(maxResults) {}

std::string WebSearchTool::name() const { return "web_search"; }

std::string WebSearchTool::description() const {
    return "Tìm kiếm thông tin nhanh trên web qua DuckDuckGo Instant "
           "Answer API. Dùng khi cần tra cứu sự kiện, định nghĩa, hoặc "
           "thông tin cập nhật mà LLM có thể không biết.";
}

nlohmann::json WebSearchTool::parametersSchema() const {
    return {
        {"type", "object"},
        {"properties", {
            {"query", {
                {"type", "string"},
                {"description", "Câu truy vấn tìm kiếm, VD: 'thu do nuoc Phap'"}
            }}
        }},
        {"required", nlohmann::json::array({"query"})}
    };
}

std::string WebSearchTool::httpGet(const std::string& url) const {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Không thể khởi tạo CURL handle");
    }

    std::string responseBody;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "agent-oop-project/1.0");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("CURL request thất bại: ") + curl_easy_strerror(res));
    }

    return responseBody;
}

nlohmann::json WebSearchTool::execute(const nlohmann::json& args) {
    if (!args.contains("query") || !args["query"].is_string()) {
        return {
            {"success", false},
            {"error", "Thiếu tham số 'query' (kiểu string)"}
        };
    }

    std::string query = args["query"].get<std::string>();

    try {
        std::string url = "https://api.duckduckgo.com/?q=" + urlEncode(query) +
                           "&format=json&no_html=1&skip_disambig=1";
        std::string rawResponse = httpGet(url);
        nlohmann::json parsed = nlohmann::json::parse(rawResponse);

        nlohmann::json results = nlohmann::json::array();

        std::string abstractText = parsed.value("AbstractText", "");
        if (!abstractText.empty()) {
            results.push_back({
                {"title", parsed.value("Heading", query)},
                {"snippet", abstractText},
                {"url", parsed.value("AbstractURL", "")}
            });
        }

        if (parsed.contains("RelatedTopics") && parsed["RelatedTopics"].is_array()) {
            for (const auto& topic : parsed["RelatedTopics"]) {
                if (static_cast<int>(results.size()) >= maxResults_) break;
                if (topic.contains("Text") && topic.contains("FirstURL")) {
                    results.push_back({
                        {"title", topic.value("Text", "")},
                        {"snippet", topic.value("Text", "")},
                        {"url", topic.value("FirstURL", "")}
                    });
                }
            }
        }

        if (results.empty()) {
            return {
                {"success", true},
                {"output", nlohmann::json::array()},
                {"note", "Không tìm thấy kết quả instant-answer cho truy vấn này"}
            };
        }

        return {{"success", true}, {"output", results}};

    } catch (const std::exception& e) {
        return {{"success", false}, {"error", std::string("Lỗi tìm kiếm: ") + e.what()}};
    }
}