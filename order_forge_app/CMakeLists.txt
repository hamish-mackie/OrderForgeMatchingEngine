add_executable(order_forge_app main.cpp)

target_include_directories(order_forge_app PUBLIC ${CMAKE_SOURCE_DIR})

target_sources(order_forge_app PRIVATE order_forge_app.cpp)

target_link_libraries(order_forge_app PUBLIC order_forge event_loop message_protocols_json)