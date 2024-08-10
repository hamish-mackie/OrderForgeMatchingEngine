add_executable(benchmark benchmark.cpp)

add_definitions(-DDISABLE_LOGGING)
#add_definitions(-DRELEASE_LOGGING)

target_link_libraries(benchmark PUBLIC exchange TracyClient)
