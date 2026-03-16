// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav2_pvt.cpp — Unit tests for nav2_pvt_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav2_pvt_decoder.h"
#include "messages/ubx_nav2_pvt.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helpers ──────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav2_pvt_payload(
    uint32_t itow, int32_t lat, int32_t lon,
    nav2_pvt_fix_type fix_type, uint8_t num_sv)
{
    std::vector<uint8_t> p(UBX_NAV2_PVT_PAYLOAD_LEN, 0u);

    // iTOW at offset 0
    p[0] = static_cast<uint8_t>(itow);
    p[1] = static_cast<uint8_t>(itow >> 8u);
    p[2] = static_cast<uint8_t>(itow >> 16u);
    p[3] = static_cast<uint8_t>(itow >> 24u);

    // lat at offset 28
    const uint32_t lat_u = static_cast<uint32_t>(lat);
    p[28] = static_cast<uint8_t>(lat_u);
    p[29] = static_cast<uint8_t>(lat_u >> 8u);
    p[30] = static_cast<uint8_t>(lat_u >> 16u);
    p[31] = static_cast<uint8_t>(lat_u >> 24u);

    // lon at offset 24
    const uint32_t lon_u = static_cast<uint32_t>(lon);
    p[24] = static_cast<uint8_t>(lon_u);
    p[25] = static_cast<uint8_t>(lon_u >> 8u);
    p[26] = static_cast<uint8_t>(lon_u >> 16u);
    p[27] = static_cast<uint8_t>(lon_u >> 24u);

    // fixType at offset 20
    p[20] = static_cast<uint8_t>(fix_type);

    // numSV at offset 23
    p[23] = num_sv;

    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav2_pvt_basic_decode()
{
    const char* name = "nav2_pvt_basic_decode";

    ubx_nav2_pvt received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_pvt_decoder>(
        new nav2_pvt_decoder([&](const ubx_nav2_pvt& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const auto payload = build_nav2_pvt_payload(360000u, 473996880, 85344650,
                                                 nav2_pvt_fix_type::fix_3d, 12u);
    // NAV2 class is 0x29
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_NAV2, UBX_ID_NAV2_PVT, payload);
    p.feed(frame);

    if (!called)                      { fail(name, "callback not called"); return; }
    if (received.i_tow != 360000u)    { fail(name, "iTOW mismatch"); return; }
    if (received.lat   != 473996880)  { fail(name, "lat mismatch"); return; }
    if (received.lon   != 85344650)   { fail(name, "lon mismatch"); return; }
    if (received.fix_type != nav2_pvt_fix_type::fix_3d) { fail(name, "fix_type mismatch"); return; }
    if (received.num_sv != 12u)       { fail(name, "num_sv mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_nav2_pvt_wrong_length()
{
    const char* name = "nav2_pvt_wrong_length";

    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_pvt_decoder>(
        new nav2_pvt_decoder([&](const ubx_nav2_pvt&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV2, UBX_ID_NAV2_PVT, std::vector<uint8_t>(10u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for wrong payload length");
}

// ─── test: signed fields correct (negative lat) ──────────────────────────────

static void test_nav2_pvt_signed_fields()
{
    const char* name = "nav2_pvt_signed_fields";

    ubx_nav2_pvt received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_pvt_decoder>(
        new nav2_pvt_decoder([&](const ubx_nav2_pvt& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // lat = -330000000 (= -33.000000 deg), lon = 1510000000 (= 151.000000 deg)
    const auto payload = build_nav2_pvt_payload(0u, -330000000, 1510000000,
                                                 nav2_pvt_fix_type::fix_3d, 0u);
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_NAV2, UBX_ID_NAV2_PVT, payload);
    p.feed(frame);

    if (!called)                         { fail(name, "callback not called"); return; }
    if (received.lat != -330000000)      { fail(name, "negative lat mismatch"); return; }
    if (received.lon != 1510000000)      { fail(name, "positive lon mismatch"); return; }
    pass(name);
}

// ─── test: msg_class / msg_id ────────────────────────────────────────────────

static void test_nav2_pvt_class_id()
{
    const char* name = "nav2_pvt_class_and_id";
    nav2_pvt_decoder d([](const ubx_nav2_pvt&) {});
    if (d.msg_class() != UBX_CLASS_NAV2) { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_NAV2_PVT) { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── test: class 0x01 (NAV) does NOT trigger NAV2 decoder ────────────────────

static void test_nav2_pvt_different_from_nav()
{
    const char* name = "nav2_pvt_different_from_nav";

    bool called = false;

    ubx_decoder_registry reg;
    // Register NAV2 decoder only
    reg.register_decoder(std::unique_ptr<nav2_pvt_decoder>(
        new nav2_pvt_decoder([&](const ubx_nav2_pvt&) { called = true; })));

    ubx_parser p(std::move(reg));

    // Send a NAV (class 0x01) PVT — should NOT trigger our NAV2 decoder
    const std::vector<uint8_t> payload(UBX_NAV2_PVT_PAYLOAD_LEN, 0u);
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_PVT, payload);
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "NAV2 callback should not fire for NAV class");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_nav2_pvt ===");
    test_nav2_pvt_class_id();
    test_nav2_pvt_basic_decode();
    test_nav2_pvt_wrong_length();
    test_nav2_pvt_signed_fields();
    test_nav2_pvt_different_from_nav();
    return 0;
}
