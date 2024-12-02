add_executable(test_message_protocols_json
        test_message_protocol_json_main.cpp
        test_message_protocol_json.cpp
)

target_link_libraries(test_message_protocols_json gtest_main order_forge message_protocols_json)


add_test(NAME test_message_protocols_json COMMAND test_message_protocols_json)
