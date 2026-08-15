cmake_minimum_required(VERSION 3.14)
project(PROJECT-AI-OOP CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

file(GLOB HARNESS_SOURCES "src/harness/*.cpp")

add_executable(agent_app src/main.cpp ${HARNESS_SOURCES})
target_include_directories(agent_app PRIVATE src)
target_link_libraries(agent_app PRIVATE nlohmann_json::nlohmann_json)

add_executable(test_harness tests/test_harness.cpp ${HARNESS_SOURCES})
target_include_directories(test_harness PRIVATE src)
target_link_libraries(test_harness PRIVATE nlohmann_json::nlohmann_json)