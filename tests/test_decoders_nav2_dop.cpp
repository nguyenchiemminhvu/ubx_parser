// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav2_dop.cpp — Unit tests for nav2_dop_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav2_dop_decoder.h"
#include "messages/ubx_nav2_dop.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav2_dop_payload(
    uint32_t itow, uint16_t gdop, uint16_t pdop,
    uint16_t tdop, uint16_t vdop, uint16_t hdop)
{
    std::vector<uint8_t> p(UBX_NAV2_DOP_PAYLOAD_LEN, 0u);

    // iTOW at offset 0
    p[0] = static_cast<uint8_t>(itow);        p[1] = static_cast<uint8_t>(itow >>  8u);
    p[2] = static_cast<uint8_t>(itow >> 16u); p[3] = static_cast<uint8_t>(itow >> 24u);

    // gDOP at offset 4
    p[4] = static_cast<uint8_t>(gdop); p[5] = static_cast<uint8_t>(gdop >> 8u);
    // pDOP at offset 6
    p[6] = static_cast<uint8_t>(pdop); p[7] = static_cast<uint8_t>(pdop >> 8u);
    // tDOP at offset 8
    p[8] = static_cast<uint8_t>(tdop); p[9] = static_cast<uint8_t>(tdop >> 8u);
    // vDOP at offset 10
    p[10] = static_cast<uint8_t>(vdop); p[11] = static_cast<uint8_t>(vdop >> 8u);
    // hDOP at offset 12
    p[12] = static_cast<uint8_t>(hdop); p[13] = static_cast<uint8_t>(hdop >> 8u);
    // nDOP/eDOP left as zero

    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav2_dop_basic_decode()
{
    const char* name = "nav2_dop_basic_decode";

    ubx_nav2_dop received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_dop_decoder>(
        new nav2_dop_decoder([&](const ubx_nav2_dop& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // DOP values × 0.01: gDOP=1.50, pDOP=1.20, tDOP=0.80, vDOP=1.10, hDOP=0.90
    const auto payload = build_nav2_dop_payload(777000u, 150u, 120u, 80u, 110u, 90u);
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_NAV2, UBX_ID_NAV2_DOP, payload);
    p.feed(frame);

    if (!called)                    { fail(name, "callback not called"); return; }
    if (received.i_tow != 777000u)  { fail(name, "iTOW mismatch"); return; }
    if (received.g_dop != 150u)     { fail(name, "g_dop mismatch"); return; }
    if (received.p_dop != 120u)     { fail(name, "p_dop mismatch"); return; }
    if (received.t_dop != 80u)      { fail(name, "t_dop mismatch"); return; }
    if (received.v_dop != 110u)     { fail(name, "v_dop mismatch"); return; }
    if (received.h_dop != 90u)      { fail(name, "h_dop mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_nav2_dop_wrong_length()
{
    const char* name = "nav2_dop_wrong_length";
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_dop_decoder>(
        new nav2_dop_decoder([&](const ubx_nav2_dop&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV2, UBX_ID_NAV2_DOP, std::vector<uint8_t>(4u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for wrong length");
}

// ─── test: class/id ──────────────────────────────────────────────────────────

static void test_nav2_dop_class_id()
{
    const char* name = "nav2_dop_class_and_id";
    nav2_dop_decoder d([](const ubx_nav2_dop&) {});
    if (d.msg_class() != UBX_CLASS_NAV2)  { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_NAV2_DOP) { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_nav2_dop ===");
    test_nav2_dop_class_id();
    test_nav2_dop_basic_decode();
    test_nav2_dop_wrong_length();
    return 0;
}
