#add_subdirectory(config)
#
#add_library(protos INTERFACE)
#
#target_link_libraries(protos INTERFACE protos_config)

#find_package(Protobuf REQUIRED)
#include_directories(${Protobuf_INCLUDE_DIRS})
#
#file(GLOB_RECURSE ALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
#message(STATUS "Proto files found:")
#
#set(FILTERED_FILES)
#
#foreach (FILE ${ALL_FILES})
#    if (NOT FILE MATCHES ".*cmake-build.*")
#        list(APPEND FILTERED_FILES ${FILE})
#    endif ()
#endforeach ()
#
#message(STATUS "Filtered Files:")
#foreach (FILTERED_FILE ${FILTERED_FILES})
#    message(STATUS "  ${FILTERED_FILE}")
#endforeach ()
#
## Define the directory for generated files
#set(GENERATED_PROTO_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
#file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})
#message(STATUS "Generated Proto files will be placed in: ${GENERATED_PROTO_DIR}")
#
## Generate C++ files from .proto files, preserving folder structure
#protobuf_generate_cpp(
#        PROTO_SRCS PROTO_HDRS
#        ${FILTERED_FILES}
#        IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
#        PROTOC_OUT_DIR ${GENERATED_PROTO_DIR}
#)
#
## Print the generated source files, one per line
#message(STATUS "Generated Proto Sources:")
#foreach (SRC ${PROTO_SRCS})
#    message(STATUS "  ${SRC}")
#endforeach ()
#
## Print the generated header files, one per line
#message(STATUS "Generated Proto Headers:")
#foreach (HDR ${PROTO_HDRS})
#    message(STATUS "  ${HDR}")
#endforeach ()

#add_library(someprotos ${PROTO_SRCS} ${PROTO_HDRS})
#target_include_directories(someprotos PUBLIC ${GENERATED_PROTO_DIR})