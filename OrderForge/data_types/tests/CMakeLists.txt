add_executable(test_data_types
        test_data_types_main.cpp
        test_scaled_int.cpp
        test_hashed_linked_list.cpp
        test_pool_allocator.cpp
)

target_link_libraries(test_data_types gtest_main data_types)

target_include_directories(test_data_types PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME test_data_types COMMAND test_data_types)
