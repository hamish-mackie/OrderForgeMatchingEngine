add_library(exchange)

target_include_directories(exchange PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(exchange PUBLIC
        order_book.cpp
        book_side.cpp
        book_level.cpp
        trade_producer.cpp
)

add_subdirectory(logger)
add_subdirectory(data_types)

target_link_libraries(exchange PUBLIC
        fmt::fmt
        magic_enum
        logger
        data_types
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)