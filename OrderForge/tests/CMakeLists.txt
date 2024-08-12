add_executable(test_order_forge
        test_order_forge_main.cpp
        test_book_side.cpp
        test_book_level.cpp
        test_trade_producer.cpp
        test_order_book.cpp
)

target_link_libraries(test_order_forge gtest_main order_forge)

target_include_directories(test_order_forge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME test_order_forge COMMAND test_order_forge)
