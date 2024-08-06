cmake_minimum_required(VERSION 3.28)
project(Exchange)

set(CMAKE_CXX_STANDARD 23)

add_executable(Exchange main.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.1.1 # Replace with the latest tag or version you want to use
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG v0.8.0  # Replace with the latest tag or version you want to use
)
FetchContent_MakeAvailable(magic_enum)

enable_testing()

add_subdirectory(exchange)
target_link_libraries(Exchange PUBLIC exchange fmt::fmt magic_enum)
