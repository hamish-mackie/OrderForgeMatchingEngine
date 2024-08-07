add_executable(exchange_tests
        exchange_tests.cpp
        test_book_side.cpp
        test_book_level.cpp
        test_trade_producer.cpp
        test_order_book.cpp
)

target_link_libraries(exchange_tests gtest_main exchange)

target_include_directories(exchange_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
