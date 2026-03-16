# tests/CMakeLists.txt
#
# Works in two modes:
#   1. Subdirectory — included by the root CMakeLists.txt via add_subdirectory().
#      The ubx_parser target is already defined; only test executables are added here.
#
#   2. Standalone — build directly from this directory without the root project:
#        cmake -S . -B build
#        cmake --build build
#        ctest --test-dir build --output-on-failure

cmake_minimum_required(VERSION 3.16)

# ── Standalone mode ────────────────────────────────────────────────────────────
if(NOT TARGET ubx_parser)
    project(ubx_parser_tests LANGUAGES CXX)

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

    enable_testing()
endif()

# ── Helper macro ──────────────────────────────────────────────────────────────
# ubx_test(<name> <source>) — builds an executable and registers it with CTest.
macro(ubx_test TEST_NAME SOURCE_FILE)
    add_executable(${TEST_NAME} "${CMAKE_CURRENT_LIST_DIR}/${SOURCE_FILE}")
    target_link_libraries(${TEST_NAME} PRIVATE ubx_parser)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# ── Checksum ──────────────────────────────────────────────────────────────────
ubx_test(test_checksum               test_checksum.cpp)

# ── Message builder ───────────────────────────────────────────────────────────
ubx_test(test_message_builder        test_message_builder.cpp)

# ── Parser stream ─────────────────────────────────────────────────────────────
ubx_test(test_parser_stream          test_parser_stream.cpp)

# ── Builders ──────────────────────────────────────────────────────────────────
ubx_test(test_builders_cfg_rst       test_builders_cfg_rst.cpp)
ubx_test(test_builders_upd_sos       test_builders_upd_sos.cpp)

# ── Decoders — NAV ────────────────────────────────────────────────────────────
ubx_test(test_decoders_nav_pvt       test_decoders_nav_pvt.cpp)
ubx_test(test_decoders_nav_sig       test_decoders_nav_sig.cpp)
ubx_test(test_decoders_nav_sat       test_decoders_nav_sat.cpp)
ubx_test(test_decoders_nav_status    test_decoders_nav_status.cpp)
ubx_test(test_decoders_nav_dop       test_decoders_nav_dop.cpp)
ubx_test(test_decoders_nav_eell      test_decoders_nav_eell.cpp)
ubx_test(test_decoders_nav_odo       test_decoders_nav_odo.cpp)
ubx_test(test_decoders_nav_timegps   test_decoders_nav_timegps.cpp)

# ── Decoders — NAV2 ───────────────────────────────────────────────────────────
ubx_test(test_decoders_nav2_pvt      test_decoders_nav2_pvt.cpp)
ubx_test(test_decoders_nav2_dop      test_decoders_nav2_dop.cpp)
ubx_test(test_decoders_nav2_eell     test_decoders_nav2_eell.cpp)
ubx_test(test_decoders_nav2_timegps  test_decoders_nav2_timegps.cpp)

# ── Decoders — ESF ────────────────────────────────────────────────────────────
ubx_test(test_decoders_esf_ins       test_decoders_esf_ins.cpp)
ubx_test(test_decoders_esf_meas      test_decoders_esf_meas.cpp)
ubx_test(test_decoders_esf_status    test_decoders_esf_status.cpp)

# ── Decoders — SEC / TIM / MON / INF / CFG / UPD ─────────────────────────────
ubx_test(test_decoders_sec_crc       test_decoders_sec_crc.cpp)
ubx_test(test_decoders_tim_tp        test_decoders_tim_tp.cpp)
ubx_test(test_decoders_mon_ver       test_decoders_mon_ver.cpp)
ubx_test(test_decoders_inf           test_decoders_inf.cpp)
ubx_test(test_decoders_cfg_valget    test_decoders_cfg_valget.cpp)
ubx_test(test_decoders_upd_sos       test_decoders_upd_sos.cpp)

# ── Database ──────────────────────────────────────────────────────────────────
ubx_test(test_database_nav_pvt       test_database_nav_pvt.cpp)
ubx_test(test_database_nav_sig       test_database_nav_sig.cpp)
ubx_test(test_database_epoch_commit  test_database_epoch_commit.cpp)
ubx_test(test_database_new_messages  test_database_new_messages.cpp)
