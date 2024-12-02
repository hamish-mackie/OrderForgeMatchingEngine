add_library(event_loop tcp_reactor.h)

target_include_directories(event_loop PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(event_loop PUBLIC logger precompiled_lib)

target_sources(event_loop PUBLIC tcp_reactor.cpp tcp_connection_handler.cpp reactor.cpp tcp_client_handler.cpp)

add_subdirectory(tests)