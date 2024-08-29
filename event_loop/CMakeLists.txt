add_library(event_loop event_loop.cpp)

target_include_directories(event_loop PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(event_loop PUBLIC logger precompiled_lib)

target_sources(event_loop PUBLIC event_handler_tcp.cpp)

add_subdirectory(tests)