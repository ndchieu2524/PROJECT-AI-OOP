cmake_minimum_required(VERSION 3.16)
project(PROJECT_AI_OOP CXX)

# Khắc phục cảnh báo CMP0135 của CMake khi dùng FetchContent
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# 1. Cấu hình C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 2. Tìm các thư viện hệ thống bắt buộc (CURL & SQLite3 & Threads)
find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

# 3. Tự động tải & tích hợp thư viện nlohmann_json
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# 4. Quét toàn bộ file .cpp trong thư mục src/
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

# Lọc bỏ các file main phụ/mock test cũ (nếu còn sót)
list(FILTER SOURCES EXCLUDE REGEX ".*/tools/main\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*/harness/mockagent\\.cpp$")

# 5. Tạo file thực thi (Executable)
add_executable(${PROJECT_NAME} ${SOURCES})

# 6. Khai báo các đường dẫn Header
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CURL_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
)

# 7. Liên kết thư viện vào target thực thi
target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    CURL::libcurl
    SQLite::SQLite3
    Threads::Threads
)