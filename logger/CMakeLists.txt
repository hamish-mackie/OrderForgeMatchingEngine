add_library(logger INTERFACE)

target_include_directories(logger INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logger INTERFACE precompiled_lib config)

add_subdirectory(tests)