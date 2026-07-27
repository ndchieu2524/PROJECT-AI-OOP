cmake_minimum_required(VERSION 3.14)
project(AIAgentFramework CXX)

# Bắt buộc chuẩn C++17 theo yêu cầu đồ án
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tự động tìm tất cả file .cpp và .h trong thư mục src
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Thêm đường dẫn để C++ tự tìm thấy các file header (.h) mà không cần path dài
include_directories(src)

# Tìm thư viện libcurl (cho kết nối API)
find_package(CURL REQUIRED)

# Tạo file thực thi ứng dụng từ đống mã nguồn
add_executable(agent_app ${SOURCES} ${HEADERS})

# Tích hợp thư viện CURL vào ứng dụng
target_link_libraries(agent_app PRIVATE CURL::libcurl)