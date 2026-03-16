# examples/CMakeLists.txt
#
# Works in two modes:
#   1. Subdirectory — included by the root CMakeLists.txt via add_subdirectory().
#      The ubx_parser target is already defined; only executables are added here.
#
#   2. Standalone — build directly from this directory without the root project:
#        cmake -S . -B build
#        cmake --build build

cmake_minimum_required(VERSION 3.16)

# ── Standalone mode ────────────────────────────────────────────────────────────
if(NOT TARGET ubx_parser)
    project(ubx_parser_examples LANGUAGES CXX)

    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    find_package(Threads REQUIRED)

    set(UBX_ROOT        "${CMAKE_CURRENT_LIST_DIR}/..")
    set(UBX_INCLUDE_DIR "${UBX_ROOT}/include")
    set(UBX_SRC_DIR     "${UBX_ROOT}/src")

    file(GLOB_RECURSE UBX_LIB_SOURCES CONFIGURE_DEPENDS
        "${UBX_SRC_DIR}/decoders/*.cpp"
        "${UBX_SRC_DIR}/database/*.cpp"
        "${UBX_SRC_DIR}/database/handlers/*.cpp"
        "${UBX_SRC_DIR}/config/*.cpp"
        "${UBX_SRC_DIR}/*.cpp"
    )

    add_library(ubx_parser STATIC ${UBX_LIB_SOURCES})

    target_include_directories(ubx_parser
        PUBLIC
            "${UBX_INCLUDE_DIR}"
        PRIVATE
            "${UBX_SRC_DIR}"
    )

    target_compile_options(ubx_parser PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
            -Wall
            -Wextra
            -Wpedantic
        >
    )

    target_link_libraries(ubx_parser
        PUBLIC
            Threads::Threads
    )
endif()

# ── Example executables ────────────────────────────────────────────────────────
add_executable(basic_parse_demo "${CMAKE_CURRENT_LIST_DIR}/basic_parse_demo.cpp")
target_link_libraries(basic_parse_demo PRIVATE ubx_parser)

add_executable(config_demo "${CMAKE_CURRENT_LIST_DIR}/config_demo.cpp")
target_link_libraries(config_demo PRIVATE ubx_parser)
