add_executable(module-tests
        oatpp-protobuf/tests.cpp
)

set_target_properties(module-tests PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(module-tests
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_EXTERNAL)
    add_dependencies(module-tests ${LIB_OATPP_EXTERNAL})
endif()

add_dependencies(module-tests ${OATPP_THIS_MODULE_NAME})

target_link_oatpp(module-tests)

## proto-gen

set(PROTOLIB_DIR ${CMAKE_CURRENT_LIST_DIR}/protolib)

execute_process(
        COMMAND bash "${PROTOLIB_DIR}/update_protolib.sh" "${PROTOLIB_DIR}"
)

target_include_directories(module-tests
        PUBLIC ${PROTOLIB_DIR}/src
)

## link libs

find_library(PROTOLIB_LIBRARY
        NAMES test-protolib
        HINTS ${PROTOLIB_DIR}/build
)

target_link_libraries(module-tests
        PRIVATE ${OATPP_THIS_MODULE_NAME}
        PRIVATE ${PROTOLIB_LIBRARY}
)

## TODO link dependencies here (if some)

add_test(module-tests module-tests)