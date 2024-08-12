add_library(order_forge)

set_property(TARGET order_forge PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_include_directories(order_forge PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(order_forge PUBLIC
        order_book.cpp
        book_side.cpp
        book_level.cpp
        trade_producer.cpp
)

add_subdirectory(logger)
add_subdirectory(data_types)

target_link_libraries(order_forge PUBLIC
        fmt::fmt
        magic_enum
        TracyClient
        logger
        data_types
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)