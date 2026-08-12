cmake_minimum_required(VERSION 3.14)
project(AIAgentFramework CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. TỰ ĐỘNG TẢI VÀ TÍCH HỢP THƯ VIỆN NLOHMANN/JSON (Header-only)
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# 2. Tự động tìm tất cả file .cpp và .h trong thư mục src
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Thêm đường dẫn để C++ tự tìm thấy các file header (.h)
include_directories(src)

# 3. Tìm thư viện libcurl (cho kết nối API)
find_package(CURL REQUIRED)

# 4. Tạo file thực thi ứng dụng
add_executable(agent_app ${SOURCES} ${HEADERS})

# 5. Tích hợp CURL và nlohmann_json vào ứng dụng
target_link_libraries(agent_app PRIVATE 
    CURL::libcurl 
    nlohmann_json::nlohmann_json
)