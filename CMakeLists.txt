cmake_minimum_required(VERSION 3.5.1)

project(CyhairCombiner)

include_directories(deps)

set (CORE_SOURCE
    ${CMAKE_SOURCE_DIR}/cyhair-combiner.cc
    ${CMAKE_SOURCE_DIR}/cyhair-loader.cc
    ${CMAKE_SOURCE_DIR}/cyhair-writer.cc
    )

add_executable( combiner ${CORE_SOURCE} )

# Increase warning level for clang.
IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(combiner PRIVATE -Weverything -Wno-system-headers -Werror -Wno-padded -Wno-c++98-compat-pedantic -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro)
ENDIF ()
