cmake_minimum_required(VERSION 3.15)
project(tool_system CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nlohmann/json: dùng FetchContent để tự tải nếu chưa cài sẵn trên máy.
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

add_library(tool_system
    tool_registry.cpp
    exec_tool.cpp
    file_tool.cpp
    calculator_tool.cpp
)
target_include_directories(tool_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tool_system PUBLIC nlohmann_json::nlohmann_json)

# Demo executable để test nhanh (không bắt buộc, có thể xoá khi merge
# vào harness_runner của thành viên C).
add_executable(tool_system_demo main.cpp)
target_link_libraries(tool_system_demo PRIVATE tool_system)