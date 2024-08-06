add_library(data_types INTERFACE)

target_include_directories(data_types INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)