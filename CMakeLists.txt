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
enable_testing()


add_subdirectory(exchange)
target_link_libraries(Exchange PUBLIC exchange)
