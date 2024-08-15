add_executable(benchmark benchmark.cpp)

#target_compile_definitions(benchmark PUBLIC DISABLE_LOGGING)
target_compile_definitions(benchmark PUBLIC RELEASE_LOGGING)

target_link_libraries(benchmark PUBLIC order_forge TracyClient)
