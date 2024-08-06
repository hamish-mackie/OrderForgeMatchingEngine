add_library(logger INTERFACE)

target_link_libraries(logger INTERFACE fmt::fmt magic_enum)