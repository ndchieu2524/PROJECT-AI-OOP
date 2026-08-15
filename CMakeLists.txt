cmake_minimum_required(VERSION 3.14)
project(PROJECT_AI_OOP CXX)

# Khắc phục cảnh báo CMP0135 của CMake
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# 1. Cấu hình C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 2. Tìm và cấu hình thư viện CURL (cho utils/http_client.cpp)
find_package(CURL REQUIRED)

# 3. Tự động tải & tích hợp thư viện nlohmann_json
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# 4. Quét tất cả file .cpp trong dự án
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "agent/*.cpp"
    "client/*.cpp"
    "tools/*.cpp"
    "harness/*.cpp"
    "utils/*.cpp"
)

# Tự động lọc bỏ file main.cpp phụ trong thư mục tools (nếu còn)
list(FILTER SOURCES EXCLUDE REGEX ".*/tools/main\\.cpp$")

# 5. Tạo file thực thi (Executable)
add_executable(${PROJECT_NAME} ${SOURCES})

# 6. Khai báo các đường dẫn Header
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/agent
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
    ${CMAKE_CURRENT_SOURCE_DIR}/harness
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CURL_INCLUDE_DIRS}
)

# 7. Liên kết thư viện JSON và CURL vào project
target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    CURL::libcurl
)