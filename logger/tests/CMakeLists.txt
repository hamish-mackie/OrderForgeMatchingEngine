add_executable(test_logger
        test_logger_main.cpp
        test_logger.cpp
        test_ring_buffer.cpp
)

target_link_libraries(test_logger gtest_main logger)

add_test(NAME test_logger COMMAND test_logger)
