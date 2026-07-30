cmake_minimum_required(VERSION 3.15)
project(CppAgentProject CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG        curl-8_21_0
)
set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(HTTP_ONLY ON CACHE INTERNAL "")
set(CURL_USE_LIBPSL OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(curl)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(json)

set(SOURCES
    src/main.cpp
    src/client/ollama_client.cpp
    src/utils/http_client.cpp
)

set(HEADERS
    src/client/llm_client.h
    src/client/ollama_client.h
    src/utils/http_client.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE libcurl nlohmann_json::nlohmann_json)