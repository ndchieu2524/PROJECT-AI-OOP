cmake_minimum_required(VERSION 3.14)
project(AIAgentFramework CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Tải thư viện nlohmann/json
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

include_directories(src)

find_package(CURL REQUIRED)

# 2. Target 1: Ứng dụng chính (Agent App)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
if(NOT SOURCES)
    set(SOURCES src/main.cpp)
endif()

add_executable(agent_app ${SOURCES} ${HEADERS})
target_link_libraries(agent_app PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)

# 3. Target 2: Unit Test
add_executable(test_harness tests/test_harness.cpp)
target_link_libraries(test_harness PRIVATE nlohmann_json::nlohmann_json)