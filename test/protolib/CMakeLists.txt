cmake_minimum_required(VERSION 3.1)

set(project_name test-protolib)

project(${project_name})

set(CMAKE_CXX_STANDARD 11)

execute_process(
        COMMAND bash "${CMAKE_CURRENT_LIST_DIR}/update_src.sh" "${CMAKE_CURRENT_LIST_DIR}"
)

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB LIB_SRC_FILES
        "src/*.h"
        "src/*.cc"
)

add_library(${project_name} ${LIB_SRC_FILES})
target_include_directories(${project_name} PUBLIC src)

find_package(Protobuf REQUIRED)

target_link_libraries(${project_name}
        protobuf::libprotobuf
)
