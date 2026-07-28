#include "calculator_tool.h"

#include <cctype>
#include <stdexcept>
#include <string>

namespace {

// Recursive-descent parser cho biểu thức số học:
//   expr   := term (('+' | '-') term)*
//   term   := factor (('*' | '/') factor)*
//   factor := number | '(' expr ')' | ('-' | '+') factor
class ExprParser {
public:
    explicit ExprParser(const std::string& s) : s_(s), pos_(0) {}

    double parse() {
        double result = parseExpr();
        skipSpaces();
        if (pos_ != s_.size()) {
            throw std::runtime_error("Ký tự không hợp lệ tại vị trí " + std::to_string(pos_));
        }
        return result;
    }

private:
    const std::string& s_;
    size_t pos_;

    void skipSpaces() {
        while (pos_ < s_.size() && std::isspace(static_cast<unsigned char>(s_[pos_]))) ++pos_;
    }

    char peek() {
        skipSpaces();
        return pos_ < s_.size() ? s_[pos_] : '\0';
    }

    double parseExpr() {
        double value = parseTerm();
        while (true) {
            char c = peek();
            if (c == '+') { ++pos_; value += parseTerm(); }
            else if (c == '-') { ++pos_; value -= parseTerm(); }
            else break;
        }
        return value;
    }

    double parseTerm() {
        double value = parseFactor();
        while (true) {
            char c = peek();
            if (c == '*') { ++pos_; value *= parseFactor(); }
            else if (c == '/') {
                ++pos_;
                double divisor = parseFactor();
                if (divisor == 0.0) throw std::runtime_error("Chia cho 0");
                value /= divisor;
            } else break;
        }
        return value;
    }

    double parseFactor() {
        char c = peek();
        if (c == '+') { ++pos_; return parseFactor(); }
        if (c == '-') { ++pos_; return -parseFactor(); }
        if (c == '(') {
            ++pos_;
            double value = parseExpr();
            if (peek() != ')') throw std::runtime_error("Thiếu dấu ')'");
            ++pos_;
            return value;
        }
        return parseNumber();
    }

    double parseNumber() {
        skipSpaces();
        size_t start = pos_;
        while (pos_ < s_.size() &&
               (std::isdigit(static_cast<unsigned char>(s_[pos_])) || s_[pos_] == '.')) {
            ++pos_;
        }
        if (start == pos_) {
            throw std::runtime_error("Mong đợi một số tại vị trí " + std::to_string(pos_));
        }
        return std::stod(s_.substr(start, pos_ - start));
    }
};

}  // namespace

std::string CalculatorTool::name() const { return "calculator"; }

std::string CalculatorTool::description() const {
    return "Tính toán một biểu thức số học (hỗ trợ +, -, *, /, dấu "
           "ngoặc). Dùng khi cần tính toán chính xác thay vì để LLM "
           "tự nhẩm.";
}

nlohmann::json CalculatorTool::parametersSchema() const {
    return {
        {"type", "object"},
        {"properties", {
            {"expression", {
                {"type", "string"},
                {"description", "Biểu thức toán học, VD: '(2 + 3) * 4'"}
            }}
        }},
        {"required", nlohmann::json::array({"expression"})}
    };
}

nlohmann::json CalculatorTool::execute(const nlohmann::json& args) {
    if (!args.contains("expression") || !args["expression"].is_string()) {
        return {
            {"success", false},
            {"error", "Thiếu tham số 'expression' (kiểu string)"}
        };
    }

    std::string expr = args["expression"].get<std::string>();

    try {
        ExprParser parser(expr);
        double result = parser.parse();
        return {{"success", true}, {"output", result}};
    } catch (const std::exception& e) {
        return {{"success", false}, {"error", std::string("Lỗi biểu thức: ") + e.what()}};
    }
}
