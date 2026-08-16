#include <iostream>

#include "tool_registry.h"
#include "exec_tool.h"
#include "file_tool.h"
#include "calculator_tool.h"

// File demo: minh hoạ cách ToolRegistry được dùng bởi harness_runner.h
// (thành viên C) — không phải một phần bắt buộc của module, chỉ để
// test nhanh phần tool khi build độc lập.
int main() {
    ToolRegistry registry;
    registry.registerTool(std::make_shared<ExecTool>(/*timeoutSeconds=*/5));
    registry.registerTool(std::make_shared<FileTool>("./workspace"));
    registry.registerTool(std::make_shared<CalculatorTool>());

    std::cout << "== Danh sách tool đã đăng ký ==\n";
    for (const auto& n : registry.listToolNames()) {
        std::cout << " - " << n << "\n";
    }

    std::cout << "\n== Schema gửi cho LLM ==\n";
    std::cout << registry.listToolSchemas().dump(2) << "\n";

    std::cout << "\n== Test calculator ==\n";
    auto calcResult = registry.execute("calculator", {{"expression", "(2 + 3) * 4"}});
    std::cout << calcResult.dump(2) << "\n";

    std::cout << "\n== Test file write + read ==\n";
    registry.execute("file", {{"action", "write"}, {"path", "note.txt"}, {"content", "hello agent"}});
    auto readResult = registry.execute("file", {{"action", "read"}, {"path", "note.txt"}});
    std::cout << readResult.dump(2) << "\n";

    std::cout << "\n== Test exec ==\n";
    auto execResult = registry.execute("exec", {{"command", "echo hi from ExecTool"}});
    std::cout << execResult.dump(2) << "\n";

    return 0;
}