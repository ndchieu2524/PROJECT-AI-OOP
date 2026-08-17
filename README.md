### C++ AI Agent Framework with Ollama API
Một hệ thống AI Agent hoàn chỉnh được viết bằng C++23, kết nối với backend suy luận cục bộ Ollama thông qua kiến trúc phân lớp hướng đối tượng: Client – Tool – Skill – Agent Loop – Harness – Evaluator.

# Mục lục
- [1. Giới thiệu tổng quan](#1-giới-thiệu-tổng-quan)
- [2. Kiến trúc & Design Patterns](#2-kiến-trúc--design-patterns)
- [3. Yêu cầu môi trường](#3-yêu-cầu-môi-trường)
- [4. Cấu hình Ollama Backend](#4-cấu-hình-ollama-backend)
  - [Cách 1: Chạy Ollama cục bộ](#cách-1-chạy-ollama-cục-bộ)
  - [Cách 2: Sử dụng Google Colab / GPU Cloud](#cách-2-sử-dụng-google-colab--gpu-cloud)
- [5. Hướng dẫn biên dịch](#5-hướng-dẫn-biên-dịch)
- [6. Hướng dẫn chạy chương trình & Benchmark](#6-hướng-dẫn-chạy-chương-trình--benchmark)
- [7. Cấu trúc thư mục](#7-cấu-trúc-thư-mục)
- [8. Danh sách thành viên & Đóng góp](#8-danh-sách-thành-viên--đóng-góp)


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

```bash
# Cài đặt các thư viện phụ thuộc
sudo apt-get update
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev libsqlite3-dev
```
# 4. Cấu hình Ollama Backend

Chạy Ollama cục bộ (Local Machine):
```bash
# Cài đặt Ollama:
curl -fsSL https://ollama.com/install.sh | sh
# Khởi chạy server và tải mô hình:
ollama serve
ollama run qwen2.5:1.5b
```

# 5. Hướng dẫn biên dịch

Dự án sử dụng CMake để quản lý quy trình build chuẩn hóa:
```bash
## 1. Tạo và chuyển vào thư mục build
mkdir -p build && cd build

## 2. Sinh Makefile cấu hình C++23
cmake ..

## 3. Biên dịch chương trình
cmake --build . -j$(nproc)
```
File thực thi được tạo tại: build/PROJECT_AI_OOP

# 6. Hướng dẫn chạy chương trình & Benchmark

## 1. Chuẩn bị môi trường làm việc
Trước khi chạy, đảm bảo thư mục workspace và skills đã sẵn sàng tại thư mục gốc:
```bash
Bashmkdir -p workspace
```
## 2. Khởi chạy
Từ thư mục gốc dự án, thực thi:
```bash
./build/PROJECT_AI_OOP
```
## 3. Kết quả đầu ra
- Sau khi bộ Benchmark hoàn tất, các tệp vết hoạt động chi tiết được sinh tự động: 
**trajectory_task_001.json, trajectory_task_002.json, ...**
- Tệp báo cáo tổng hợp JSON: **benchmark_report.json**
- Tệp báo cáo hiệu năng Markdown: **benchmark_report.md**

# 7. Cấu trúc thư mục:

```text
Agent_25127330_25127287_25127518/
├── CMakeLists.txt              # Cấu hình biên dịch dự án
├── README.md                   # Hướng dẫn build, chạy và cấu hình Ollama
├── src/
│   ├── main.cpp                # Điểm khởi chạy chính
│   ├── agent/
│   │   ├── agent_loop.h/.cpp   # Triển khai vòng lặp ReAct
│   │   ├── loop_detector.h/.cpp# Phát hiện chu kỳ lặp
│   │   └── skill_loader.h/.cpp # Nạp và chọn lọc skill
│   ├── client/
│   │   ├── llm_client.h        # Interface trừu tượng LLMClient
│   │   └── ollama_client.h/.cpp# Triển khai HTTP Client gọi Ollama API
│   ├── tools/
│   │   ├── tool.h              # Interface trừu tượng Tool
│   │   ├── tool_registry.h/.cpp# Quản lý và dispatch Tool
│   │   ├── exec_tool.h/.cpp    # Tool chạy lệnh shell
│   │   ├── file_tool.h/.cpp    # Tool đọc/ghi file
│   │   ├── calculator_tool.h/.cpp # Tool tính toán biểu thức
│   │   ├── web_search_tool.h/.cpp # Tool tìm kiếm DuckDuckGo API
│   │   └── memory_tool.h/.cpp  # Tool lưu/truy vấn SQLite
│   └── harness/
│       ├── i_agent.h           # Interface IAgent
│       ├── harness_runner.h/.cpp # Điều phối chạy và xuất báo cáo
│       ├── trajectory.h/.cpp   # Lưu vết từng bước thực thi
│       └── evaluator.h/.cpp    # Chiến lược chấm điểm tự động
├── skills/                     # Tập các file hướng dẫn kỹ năng (.md)
│   ├── task_planner.md
│   ├── error_recovery.md
│   └── math_and_file_ops.md
├── benchmark/
│   ├── tasks.json              # Tập dữ liệu kiểm thử (>= 10 tasks)
│   └── run_eval.cpp
├── tests/                      # Mã nguồn Unit Test
└── docs/                       # Biểu đồ thiết kế (Mermaid/Images)
    ├── class_diagram.png
    ├── sequence_diagram_agent.png
    └── component_diagram.png
```
# 8. Danh sách thành viên & Đóng góp

| STT | Tên thành viên | MSSV | Vai trò |
| :---: | :--- | :---: | :--- |
| 1 | Nguyễn Đoàn Công Hiếu | 25127330 | Tầng Tool Registry (5 Tools), Skill Loader |
| 2 | Phạm Tiến Cường | 25127287 | Tầng LLM Client, Agent Loop, Loop Detector |
| 3 | Nguyễn Anh Quang Tiến | 25127518 | Tầng Harness Runner, Trajectory, Evaluators & Benchmark |