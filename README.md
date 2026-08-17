### C++ AI Agent Framework with Ollama API
Một hệ thống AI Agent hoàn chỉnh được viết bằng C++23, kết nối với backend suy luận cục bộ Ollama thông qua kiến trúc phân lớp hướng đối tượng: Client – Tool – Skill – Agent Loop – Harness – Evaluator.

# Mục lục
1. Giới thiệu tổng quan

2. Kiến trúc & Design Patterns

3. Yêu cầu môi trường

4. Cấu hình Ollama Backend

5. Hướng dẫn biên dịch

6. Hướng dẫn chạy chương trình & Benchmark

7. Cấu trúc thư mục

8. Danh sách thành viên & Đóng góp


# 1. Giới thiệu tổng quan

Dự án mô phỏng các framework Agent hiện đại (như OpenClaw, LangChain) nhằm tự động hóa việc suy luận ReAct (Reasoning + Acting)
- Tự động gọi công cụ (Tool Use): Đọc/ghi tệp, tính toán số học, thực thi dòng lệnh, tìm kiếm web, và ghi nhớ trạng thái qua SQLite.
- Quản lý kỹ năng (Skill System): Đọc các chỉ dẫn .md và nạp động vào System Prompt dựa trên phân tích từ khóa.
- Phát hiện vòng lặp (Loop Detection): Tự động phát hiện và ngắt các chu kỳ lặp lại hành động hoặc ping-pong loop.
- Evaluation Harness: Chạy tự động tập dữ liệu kiểm thử (Benchmark Suite), đo lường độ trễ (latency), xuất Trajectory JSON và Báo cáo Markdown.

# 2. Kiến trúc & Design Patterns

Hệ thống áp dụng 4 mẫu thiết kế hướng đối tượng cốt lõi:
| Design Pattern | Triển khai trong dự án | Vai trò |
| :--- | :--- | :--- |
| **Strategy** | Lớp cơ sở trừu tượng `Evaluator` với các lớp con `KeywordEvaluator`, `FunctionalEvaluator`, `RegexEvaluator` | Cho phép hoán đổi thuật toán chấm điểm linh hoạt theo từng task mà không sửa đổi `HarnessRunner`. |
| **Template Method** | Cấu trúc khung `AgentLoop::run()` định nghĩa skeleton vòng lặp ReAct, các bước `observe()`, `think()`, `act()` có thể override | Cố định luồng ReAct chuẩn nhưng cho phép tùy biến từng pha hành vi. |
| **Registry / Factory** | `agent::tools::ToolRegistry` | Đăng ký động và quản lý vòng đời các đối tượng kế thừa từ `Tool` theo tên. |
| **Observer / Hook** | Cơ chế callback `set_step_hook(StepHook)` | `HarnessRunner` inject hook vào `AgentLoop` để ghi nhận `Trajectory` theo thời gian thực mà không vi phạm nguyên tắc đóng gói. |

# 3. Yêu cầu môi trường

- Hệ điều hành: Linux (Ubuntu 22.04 LTS / Debian) hoặc GitHub Codespaces / WSL2
- Trình biên dịch: GCC 13+ hoặc Clang 16+ (Hỗ trợ C++20 / C++23)
- Công cụ dựng: CMake $\ge 3.16$
- Thư viện phụ thuộc:
+ libcurl4-openssl-dev (Giao tiếp HTTP REST API)
+ libsqlite3-dev (Quản lý bộ nhớ lâu dài SQLite)
+ nlohmann/json (Tự động tải qua CMake FetchContent)

- Cài đặt các gói phụ thuộc trên Ubuntu/Debian:
sudo apt-get update
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev libsqlite3-dev

# 4. Cấu hình Ollama Backend

Chạy Ollama cục bộ (Local Machine):
## Cài đặt Ollama:
curl -fsSL https://ollama.com/install.sh | sh
## Khởi chạy server và tải mô hình:
ollama serve
ollama run qwen2.5:1.5b

# 5. Hướng dẫn biên dịch

Dự án sử dụng CMake để quản lý quy trình build chuẩn hóa:
## 1. Tạo và chuyển vào thư mục build
mkdir -p build && cd build

## 2. Sinh Makefile cấu hình C++23
cmake ..

## 3. Biên dịch chương trình
cmake --build . -j$(nproc)

File thực thi được tạo tại: build/PROJECT_AI_OOP

# 6. Hướng dẫn chạy chương trình & Benchmark

## 1. Chuẩn bị môi trường làm việc
Trước khi chạy, đảm bảo thư mục workspace và skills đã sẵn sàng tại thư mục gốc:Bashmkdir -p workspace
## 2. Khởi chạy
Từ thư mục gốc dự án, thực thi:Bash./build/PROJECT_AI_OOP
## 3. Kết quả đầu ra
- Sau khi bộ Benchmark hoàn tất, các tệp vết hoạt động chi tiết được sinh tự động: 
trajectory_task_001.json, trajectory_task_002.json, ...
- Tệp báo cáo tổng hợp JSON: benchmark_report.json
- Tệp báo cáo hiệu năng Markdown: benchmark_report.md7. 

# 7. Cấu trúc thư mục:

Agent_25127330_25127287_25127518
├── CMakeLists.txt              # Cấu hình biên dịch dự án
├── README.md                   # Hướng dẫn cài đặt và sử dụng
├── benchmark/
│   └── tasks.json              # Tập dữ liệu kiểm thử (10 tasks phân hóa)
├── skills/                     # Các tệp hướng dẫn kỹ năng cho Agent
│   ├── task_planner.md
│   ├── error_recovery.md
│   └── math_and_file_ops.md
├── workspace/                  # Môi trường cách ly để FileTool thao tác
└── src/
    ├── main.cpp                # Điểm khởi chạy chính kết nối các tầng
    ├── client/                 # Tầng giao tiếp mạng & mô hình LLM
    │   ├── llm_client.h        # Interface trừu tượng LLMClient
    │   └── ollama_client.h/.cpp# Triển khai HTTP Client gọi Ollama API
    ├── tools/                  # Tầng công cụ thực thi
    │   ├── tool.h              # Interface trừu tượng Tool
    │   ├── tool_registry.h/.cpp# Quản lý và dispatch Tool
    │   ├── exec_tool.h/.cpp    # Tool thực thi lệnh shell
    │   ├── file_tool.h/.cpp    # Tool đọc/ghi file
    │   ├── calculator_tool.h/.cpp # Tool tính toán biểu thức toán học
    │   ├── web_search_tool.h/.cpp # Tool tìm kiếm DuckDuckGo API
    │   └── memory_tool.h/.cpp  # Tool lưu trữ bộ nhớ SQLite
    ├── agent/                  # Tầng lõi Agent & Kỹ năng
    │   ├── agent_loop.h/.cpp   # Triển khai ReAct cycle
    │   ├── loop_detector.h/.cpp# Phát hiện và ngắt chu kỳ lặp
    │   └── skill_loader.h/.cpp # Nạp và chọn lọc skill tự động
    └── harness/                # Tầng đo lường & Đánh giá
        ├── i_agent.h           # Giao diện trừu tượng kết nối Harness
        ├── trajectory.h/.cpp   # Cấu trúc lưu vết từng bước thực thi
        ├── evaluator.h/.cpp    # Chiến lược đánh giá kết quả
        ├── taskloader.h/.cpp   # Nạp dữ liệu benchmark từ JSON
        └── harness_runner.h/.cpp # Điều phối chạy và xuất báo cáo

# 8. Danh sách thành viên và đóng góp

| STT | Tên thành viên | MSSV | Vai trò |
| :---: | :--- | :---: | :--- |
| 1 | Nguyễn Đoàn Công Hiếu | 25127330 | Tầng Tool Registry (5 Tools), Skill Loader |
| 2 | Phạm Tiến Cường | 25127287 | Tầng LLM Client, Agent Loop, Loop Detector |
| 3 | Nguyễn Anh Quang Tiến | 25127287 | Tầng Harness Runner, Trajectory, Evaluators & Benchmark |