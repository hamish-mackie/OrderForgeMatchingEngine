add_executable(data_type_tests
        data_type_tests.cpp
)

target_link_libraries(data_type_tests gtest_main data_types)

target_include_directories(data_type_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME DataTypeTests COMMAND data_type_tests)

add_custom_target(run_data_type_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS data_type_tests
)