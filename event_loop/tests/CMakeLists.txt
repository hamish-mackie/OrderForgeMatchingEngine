add_executable(test_event_loop
        test_event_loop_main.cpp
        test_event_loop.cpp
)

target_link_libraries(test_event_loop gtest_main event_loop)

target_include_directories(test_event_loop PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME test_event_loop COMMAND test_event_loop)
