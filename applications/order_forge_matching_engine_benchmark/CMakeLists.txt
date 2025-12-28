add_executable(app_benchmark benchmark.cpp)

target_compile_definitions(app_benchmark PUBLIC LOG_MODE=0) # disable logging
#target_compile_definitions(app_benchmark PUBLIC LOG_MODE=1) # release logging

target_link_libraries(app_benchmark PUBLIC order_forge TracyClient)
