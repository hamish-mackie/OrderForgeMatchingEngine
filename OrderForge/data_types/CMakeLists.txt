add_library(data_types INTERFACE)

target_include_directories(data_types INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(data_types INTERFACE logger)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)