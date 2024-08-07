add_executable(data_type_tests
        data_type_tests.cpp
        test_scaled_int.cpp
)

target_link_libraries(data_type_tests gtest_main data_types)

target_include_directories(data_type_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
