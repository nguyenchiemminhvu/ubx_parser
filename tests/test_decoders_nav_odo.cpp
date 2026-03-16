// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_odo.cpp — Unit tests for nav_odo_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_odo_decoder.h"
#include "messages/ubx_nav_odo.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav_odo_payload(
    uint8_t version, uint32_t itow,
    uint32_t distance, uint32_t total_distance, uint32_t distance_std)
{
    std::vector<uint8_t> p(UBX_NAV_ODO_PAYLOAD_LEN, 0u);

    p[0] = version;
    // reserved1 at [1-3] = 0

    // iTOW at offset 4
    p[4] = static_cast<uint8_t>(itow);        p[5] = static_cast<uint8_t>(itow >>  8u);
    p[6] = static_cast<uint8_t>(itow >> 16u); p[7] = static_cast<uint8_t>(itow >> 24u);

    // distance at offset 8
    p[8]  = static_cast<uint8_t>(distance);        p[9]  = static_cast<uint8_t>(distance >>  8u);
    p[10] = static_cast<uint8_t>(distance >> 16u); p[11] = static_cast<uint8_t>(distance >> 24u);

    // totalDistance at offset 12
    p[12] = static_cast<uint8_t>(total_distance);        p[13] = static_cast<uint8_t>(total_distance >>  8u);
    p[14] = static_cast<uint8_t>(total_distance >> 16u); p[15] = static_cast<uint8_t>(total_distance >> 24u);

    // distanceStd at offset 16
    p[16] = static_cast<uint8_t>(distance_std);        p[17] = static_cast<uint8_t>(distance_std >>  8u);
    p[18] = static_cast<uint8_t>(distance_std >> 16u); p[19] = static_cast<uint8_t>(distance_std >> 24u);

    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav_odo_basic_decode()
{
    const char* name = "nav_odo_basic_decode";

    ubx_nav_odo received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_odo_decoder>(
        new nav_odo_decoder([&](const ubx_nav_odo& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const auto payload = build_nav_odo_payload(0u, 500000u, 1234u, 98765u, 5u);
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_ODO, payload);
    p.feed(frame);

    if (!called)                          { fail(name, "callback not called"); return; }
    if (received.version        != 0u)    { fail(name, "version mismatch"); return; }
    if (received.i_tow          != 500000u) { fail(name, "iTOW mismatch"); return; }
    if (received.distance       != 1234u) { fail(name, "distance mismatch"); return; }
    if (received.total_distance != 98765u){ fail(name, "total_distance mismatch"); return; }
    if (received.distance_std   != 5u)    { fail(name, "distance_std mismatch"); return; }
    pass(name);
}

// ─── test: large distance values (uint32 boundary) ───────────────────────────

static void test_nav_odo_large_values()
{
    const char* name = "nav_odo_large_values";

    ubx_nav_odo received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_odo_decoder>(
        new nav_odo_decoder([&](const ubx_nav_odo& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // Max uint32 values
    const auto payload = build_nav_odo_payload(
        0u, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu);
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_ODO, payload);
    p.feed(frame);

    if (!called)                               { fail(name, "callback not called"); return; }
    if (received.distance       != 0xFFFFFFFFu) { fail(name, "distance boundary fail"); return; }
    if (received.total_distance != 0xFFFFFFFFu) { fail(name, "total_distance boundary fail"); return; }
    pass(name);
}

// ─── test: wrong payload length ──────────────────────────────────────────────

static void test_nav_odo_wrong_length()
{
    const char* name = "nav_odo_wrong_length";
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_odo_decoder>(
        new nav_odo_decoder([&](const ubx_nav_odo&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_ODO, std::vector<uint8_t>(16u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for wrong length");
}

// ─── test: class/id ──────────────────────────────────────────────────────────

static void test_nav_odo_class_id()
{
    const char* name = "nav_odo_class_and_id";
    nav_odo_decoder d([](const ubx_nav_odo&) {});
    if (d.msg_class() != UBX_CLASS_NAV) { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_NAV_ODO) { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_nav_odo ===");
    test_nav_odo_class_id();
    test_nav_odo_basic_decode();
    test_nav_odo_large_values();
    test_nav_odo_wrong_length();
    return 0;
}
