add_executable(exchange_tests
        exchange_tests.cpp
)

target_link_libraries(exchange_tests gtest_main exchange)

target_include_directories(exchange_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ExchangeTests COMMAND exchange_tests)

add_custom_target(run_exchange_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS exchange_tests
)