cmake_minimum_required(VERSION 3.28)
project(OrderForge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.0.2
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG v0.9.6
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.11.0
)
FetchContent_MakeAvailable(tracy)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

enable_testing()

add_library(precompiled_lib INTERFACE)

target_link_libraries(precompiled_lib INTERFACE
        ${Protobuf_LIBRARIES}
        nlohmann_json
        fmt::fmt
        magic_enum
)

target_precompile_headers(precompiled_lib INTERFACE
        <string>
        <vector>
        <unordered_map>
        <map>
        <memory>
        <fmt/format.h>
        <magic_enum.hpp>
        <nlohmann/json.hpp>
)

add_subdirectory(config)
add_subdirectory(logger)
add_subdirectory(event_loop)
add_subdirectory(order_forge)
add_subdirectory(message_protocols)
add_subdirectory(applications)
