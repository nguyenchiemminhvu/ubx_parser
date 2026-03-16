// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_status.cpp — Unit tests for nav_status_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_status_decoder.h"
#include "messages/ubx_nav_status.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav_status_payload(uint32_t           itow,
                                                      nav_status_gps_fix gps_fix,
                                                      uint8_t            flags,
                                                      uint8_t            fix_stat,
                                                      uint8_t            flags2,
                                                      uint32_t           ttff,
                                                      uint32_t           msss)
{
    std::vector<uint8_t> p;
    auto push_u32 = [&](uint32_t v) {
        p.push_back(static_cast<uint8_t>(v));
        p.push_back(static_cast<uint8_t>(v >> 8u));
        p.push_back(static_cast<uint8_t>(v >> 16u));
        p.push_back(static_cast<uint8_t>(v >> 24u));
    };

    push_u32(itow);
    p.push_back(static_cast<uint8_t>(gps_fix));
    p.push_back(flags);
    p.push_back(fix_stat);
    p.push_back(flags2);
    push_u32(ttff);
    push_u32(msss);
    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav_status_basic_decode()
{
    const char* name = "nav_status_basic_decode";

    ubx_nav_status received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_status_decoder>(
        new nav_status_decoder([&](const ubx_nav_status& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_STATUS,
        build_nav_status_payload(200000u, nav_status_gps_fix::fix_3d, 0x01u, 0u, 0u, 1500u, 30000u)));

    if (!called)                                                { fail(name, "callback not called"); return; }
    if (received.i_tow != 200000u)                              { fail(name, "iTOW mismatch"); return; }
    if (received.gps_fix != nav_status_gps_fix::fix_3d)         { fail(name, "gps_fix mismatch"); return; }
    if (received.flags != 0x01u)                                { fail(name, "flags mismatch"); return; }
    if (received.ttff != 1500u)                                 { fail(name, "ttff mismatch"); return; }
    if (received.msss != 30000u)                                { fail(name, "msss mismatch"); return; }
    pass(name);
}

// ─── test: no-fix state ───────────────────────────────────────────────────────

static void test_nav_status_no_fix()
{
    const char* name = "nav_status_no_fix";

    ubx_nav_status received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_status_decoder>(
        new nav_status_decoder([&](const ubx_nav_status& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_STATUS,
        build_nav_status_payload(0u, nav_status_gps_fix::no_fix, 0u, 0u, 0u, 0u, 0u)));

    if (!called)                                                { fail(name, "callback not called"); return; }
    if (received.gps_fix != nav_status_gps_fix::no_fix)         { fail(name, "gps_fix should be no_fix"); return; }
    pass(name);
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_nav_status_wrong_length()
{
    const char* name = "nav_status_wrong_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_status_decoder>(
        new nav_status_decoder([&](const ubx_nav_status&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_STATUS, std::vector<uint8_t>(8u, 0u)));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for wrong payload length");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_nav_status_fragmented()
{
    const char* name = "nav_status_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_status_decoder>(
        new nav_status_decoder([&](const ubx_nav_status&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_STATUS,
        build_nav_status_payload(0u, nav_status_gps_fix::fix_2d, 0u, 0u, 0u, 0u, 100u));

    const std::size_t half = frame.size() / 2u;
    p.feed(std::vector<uint8_t>(frame.begin(), frame.begin() + static_cast<long>(half)));
    if (called) { fail(name, "fired too early"); return; }
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(half), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired after second chunk");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_nav_status ===\n");
    test_nav_status_basic_decode();
    test_nav_status_no_fix();
    test_nav_status_wrong_length();
    test_nav_status_fragmented();
    std::printf("Done.\n");
    return 0;
}
