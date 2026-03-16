// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav2_timegps.cpp — Unit tests for nav2_timegps_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav2_timegps_decoder.h"
#include "messages/ubx_nav2_timegps.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav2_timegps_payload(
    uint32_t itow, int32_t ftow, int16_t week,
    int8_t leap_s, uint8_t valid, uint32_t tacc)
{
    std::vector<uint8_t> p(UBX_NAV2_TIMEGPS_PAYLOAD_LEN, 0u);

    // iTOW at offset 0
    p[0] = static_cast<uint8_t>(itow);        p[1] = static_cast<uint8_t>(itow >>  8u);
    p[2] = static_cast<uint8_t>(itow >> 16u); p[3] = static_cast<uint8_t>(itow >> 24u);

    // fTOW at offset 4 (signed)
    const uint32_t ftow_u = static_cast<uint32_t>(ftow);
    p[4] = static_cast<uint8_t>(ftow_u);        p[5] = static_cast<uint8_t>(ftow_u >>  8u);
    p[6] = static_cast<uint8_t>(ftow_u >> 16u); p[7] = static_cast<uint8_t>(ftow_u >> 24u);

    // week at offset 8 (signed 16-bit)
    const uint16_t week_u = static_cast<uint16_t>(week);
    p[8] = static_cast<uint8_t>(week_u); p[9] = static_cast<uint8_t>(week_u >> 8u);

    // leapS at offset 10
    p[10] = static_cast<uint8_t>(leap_s);

    // valid at offset 11
    p[11] = valid;

    // tAcc at offset 12
    p[12] = static_cast<uint8_t>(tacc);        p[13] = static_cast<uint8_t>(tacc >>  8u);
    p[14] = static_cast<uint8_t>(tacc >> 16u); p[15] = static_cast<uint8_t>(tacc >> 24u);

    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav2_timegps_basic_decode()
{
    const char* name = "nav2_timegps_basic_decode";

    ubx_nav2_timegps received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_timegps_decoder>(
        new nav2_timegps_decoder([&](const ubx_nav2_timegps& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // GPS week 2348, leap seconds = 18
    const auto payload = build_nav2_timegps_payload(
        400000u, 12345, static_cast<int16_t>(2348), static_cast<int8_t>(18), 0x07u, 25u);
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_NAV2, UBX_ID_NAV2_TIMEGPS, payload);
    p.feed(frame);

    if (!called)                      { fail(name, "callback not called"); return; }
    if (received.i_tow != 400000u)    { fail(name, "iTOW mismatch"); return; }
    if (received.f_tow != 12345)      { fail(name, "fTOW mismatch"); return; }
    if (received.week  != 2348)       { fail(name, "week mismatch"); return; }
    if (received.leap_s != 18)        { fail(name, "leapS mismatch"); return; }
    if (received.valid != 0x07u)      { fail(name, "valid mismatch"); return; }
    if (received.t_acc != 25u)        { fail(name, "tAcc mismatch"); return; }
    pass(name);
}

// ─── test: negative leap seconds ─────────────────────────────────────────────

static void test_nav2_timegps_neg_leap()
{
    const char* name = "nav2_timegps_neg_leap_seconds";

    ubx_nav2_timegps received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_timegps_decoder>(
        new nav2_timegps_decoder([&](const ubx_nav2_timegps& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const auto payload = build_nav2_timegps_payload(
        0u, 0, 0, static_cast<int8_t>(-1), 0u, 0u);
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_NAV2, UBX_ID_NAV2_TIMEGPS, payload);
    p.feed(frame);

    if (!called)            { fail(name, "callback not called"); return; }
    if (received.leap_s != -1) { fail(name, "negative leapS mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length ──────────────────────────────────────────────

static void test_nav2_timegps_wrong_length()
{
    const char* name = "nav2_timegps_wrong_length";
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_timegps_decoder>(
        new nav2_timegps_decoder([&](const ubx_nav2_timegps&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV2, UBX_ID_NAV2_TIMEGPS, std::vector<uint8_t>(8u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for wrong length");
}

// ─── test: class/id ──────────────────────────────────────────────────────────

static void test_nav2_timegps_class_id()
{
    const char* name = "nav2_timegps_class_and_id";
    nav2_timegps_decoder d([](const ubx_nav2_timegps&) {});
    if (d.msg_class() != UBX_CLASS_NAV2)       { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_NAV2_TIMEGPS)  { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_nav2_timegps ===");
    test_nav2_timegps_class_id();
    test_nav2_timegps_basic_decode();
    test_nav2_timegps_neg_leap();
    test_nav2_timegps_wrong_length();
    return 0;
}
