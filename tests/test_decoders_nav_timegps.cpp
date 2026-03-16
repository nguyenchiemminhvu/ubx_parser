// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_timegps.cpp — Unit tests for nav_timegps_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_timegps_decoder.h"
#include "messages/ubx_nav_timegps.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav_timegps_payload(uint32_t itow,
                                                       int32_t  f_tow,
                                                       int16_t  week,
                                                       int8_t   leap_s,
                                                       uint8_t  valid,
                                                       uint32_t t_acc)
{
    std::vector<uint8_t> p;
    auto push_u32 = [&](uint32_t v) {
        p.push_back(static_cast<uint8_t>(v));
        p.push_back(static_cast<uint8_t>(v >> 8u));
        p.push_back(static_cast<uint8_t>(v >> 16u));
        p.push_back(static_cast<uint8_t>(v >> 24u));
    };

    push_u32(itow);
    push_u32(static_cast<uint32_t>(f_tow));
    const uint16_t week_u = static_cast<uint16_t>(week);
    p.push_back(static_cast<uint8_t>(week_u));
    p.push_back(static_cast<uint8_t>(week_u >> 8u));
    p.push_back(static_cast<uint8_t>(leap_s));
    p.push_back(valid);
    push_u32(t_acc);
    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav_timegps_basic_decode()
{
    const char* name = "nav_timegps_basic_decode";

    ubx_nav_timegps received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_timegps_decoder>(
        new nav_timegps_decoder([&](const ubx_nav_timegps& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_TIMEGPS,
        build_nav_timegps_payload(432000000u, -150000, 2350, 18, 0x07u, 5u)));

    if (!called)                      { fail(name, "callback not called"); return; }
    if (received.i_tow != 432000000u) { fail(name, "iTOW mismatch"); return; }
    if (received.f_tow != -150000)    { fail(name, "fTOW mismatch"); return; }
    if (received.week != 2350)        { fail(name, "week mismatch"); return; }
    if (received.leap_s != 18)        { fail(name, "leapS mismatch"); return; }
    if (received.valid != 0x07u)      { fail(name, "valid flags mismatch"); return; }
    if (received.t_acc != 5u)         { fail(name, "tAcc mismatch"); return; }
    pass(name);
}

// ─── test: valid flags decode ─────────────────────────────────────────────────

static void test_nav_timegps_valid_flags()
{
    const char* name = "nav_timegps_valid_flags";

    ubx_nav_timegps received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_timegps_decoder>(
        new nav_timegps_decoder([&](const ubx_nav_timegps& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    // valid = 0x03: towValid + weekValid set, leapSValid clear
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_TIMEGPS,
        build_nav_timegps_payload(0u, 0, 0, 0, 0x03u, 0u)));

    if (!called)               { fail(name, "callback not called"); return; }
    if (received.valid != 0x03u) { fail(name, "valid mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_nav_timegps_wrong_length()
{
    const char* name = "nav_timegps_wrong_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_timegps_decoder>(
        new nav_timegps_decoder([&](const ubx_nav_timegps&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_TIMEGPS, std::vector<uint8_t>(10u, 0u)));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for wrong payload length");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_nav_timegps_fragmented()
{
    const char* name = "nav_timegps_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_timegps_decoder>(
        new nav_timegps_decoder([&](const ubx_nav_timegps&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_TIMEGPS,
        build_nav_timegps_payload(0u, 0, 0, 0, 0x07u, 100u));

    const std::size_t half = frame.size() / 2u;
    p.feed(std::vector<uint8_t>(frame.begin(), frame.begin() + static_cast<long>(half)));
    if (called) { fail(name, "fired too early"); return; }
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(half), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_nav_timegps ===\n");
    test_nav_timegps_basic_decode();
    test_nav_timegps_valid_flags();
    test_nav_timegps_wrong_length();
    test_nav_timegps_fragmented();
    std::printf("Done.\n");
    return 0;
}
