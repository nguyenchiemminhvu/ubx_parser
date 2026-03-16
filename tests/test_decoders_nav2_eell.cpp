// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav2_eell.cpp — Unit tests for nav2_eell_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav2_eell_decoder.h"
#include "messages/ubx_nav2_eell.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav2_eell_payload(
    uint32_t itow, uint8_t version,
    uint16_t err_maj, uint16_t err_min, uint16_t err_orient)
{
    std::vector<uint8_t> p(UBX_NAV2_EELL_PAYLOAD_LEN, 0u);

    // iTOW at offset 0
    p[0] = static_cast<uint8_t>(itow);
    p[1] = static_cast<uint8_t>(itow >> 8u);
    p[2] = static_cast<uint8_t>(itow >> 16u);
    p[3] = static_cast<uint8_t>(itow >> 24u);

    // version at offset 4
    p[4] = version;

    // reserved1[3] at offset 5-7 (zeroed)

    // errMaj at offset 8
    p[8]  = static_cast<uint8_t>(err_maj);
    p[9]  = static_cast<uint8_t>(err_maj >> 8u);

    // errMin at offset 10
    p[10] = static_cast<uint8_t>(err_min);
    p[11] = static_cast<uint8_t>(err_min >> 8u);

    // errOrient at offset 12
    p[12] = static_cast<uint8_t>(err_orient);
    p[13] = static_cast<uint8_t>(err_orient >> 8u);

    // reserved2[2] at offset 14-15 (zeroed)

    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav2_eell_basic_decode()
{
    const char* name = "nav2_eell_basic_decode";

    ubx_nav2_eell received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_eell_decoder>(
        new nav2_eell_decoder([&](const ubx_nav2_eell& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const auto payload = build_nav2_eell_payload(123456u, 0u, 500u, 300u, 4500u);
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_NAV2, UBX_ID_NAV2_EELL, payload);
    p.feed(frame);

    if (!called)                      { fail(name, "callback not called"); return; }
    if (received.i_tow    != 123456u) { fail(name, "iTOW mismatch"); return; }
    if (received.version  != 0u)      { fail(name, "version mismatch"); return; }
    if (received.err_maj  != 500u)    { fail(name, "err_maj mismatch"); return; }
    if (received.err_min  != 300u)    { fail(name, "err_min mismatch"); return; }
    if (received.err_orient != 4500u) { fail(name, "err_orient mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_nav2_eell_wrong_length()
{
    const char* name = "nav2_eell_wrong_length";
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav2_eell_decoder>(
        new nav2_eell_decoder([&](const ubx_nav2_eell&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV2, UBX_ID_NAV2_EELL, std::vector<uint8_t>(8u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for wrong length");
}

// ─── test: class/id ──────────────────────────────────────────────────────────

static void test_nav2_eell_class_id()
{
    const char* name = "nav2_eell_class_and_id";
    nav2_eell_decoder d([](const ubx_nav2_eell&) {});
    if (d.msg_class() != UBX_CLASS_NAV2)  { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_NAV2_EELL) { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_nav2_eell ===");
    test_nav2_eell_class_id();
    test_nav2_eell_basic_decode();
    test_nav2_eell_wrong_length();
    return 0;
}
