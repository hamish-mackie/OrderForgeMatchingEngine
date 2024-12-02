add_executable(app_order_forge main.cpp)

target_include_directories(app_order_forge PUBLIC ${CMAKE_SOURCE_DIR})

target_sources(app_order_forge PRIVATE order_forge_app.cpp)

target_link_libraries(app_order_forge PUBLIC order_forge event_loop message_protocols_json)