// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_dop.cpp — Unit tests for nav_dop_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_dop_decoder.h"
#include "messages/ubx_nav_dop.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav_dop_payload(uint32_t itow,
                                                   uint16_t g_dop,
                                                   uint16_t p_dop,
                                                   uint16_t t_dop,
                                                   uint16_t v_dop,
                                                   uint16_t h_dop,
                                                   uint16_t n_dop,
                                                   uint16_t e_dop)
{
    std::vector<uint8_t> p;
    auto push_u32 = [&](uint32_t v) {
        p.push_back(static_cast<uint8_t>(v));
        p.push_back(static_cast<uint8_t>(v >> 8u));
        p.push_back(static_cast<uint8_t>(v >> 16u));
        p.push_back(static_cast<uint8_t>(v >> 24u));
    };
    auto push_u16 = [&](uint16_t v) {
        p.push_back(static_cast<uint8_t>(v));
        p.push_back(static_cast<uint8_t>(v >> 8u));
    };

    push_u32(itow);
    push_u16(g_dop);
    push_u16(p_dop);
    push_u16(t_dop);
    push_u16(v_dop);
    push_u16(h_dop);
    push_u16(n_dop);
    push_u16(e_dop);
    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav_dop_basic_decode()
{
    const char* name = "nav_dop_basic_decode";

    ubx_nav_dop received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_dop_decoder>(
        new nav_dop_decoder([&](const ubx_nav_dop& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_DOP,
        build_nav_dop_payload(1000u, 120u, 110u, 100u, 90u, 80u, 70u, 60u)));

    if (!called)                  { fail(name, "callback not called"); return; }
    if (received.i_tow != 1000u)  { fail(name, "iTOW mismatch"); return; }
    if (received.g_dop != 120u)   { fail(name, "gDOP mismatch"); return; }
    if (received.p_dop != 110u)   { fail(name, "pDOP mismatch"); return; }
    if (received.t_dop != 100u)   { fail(name, "tDOP mismatch"); return; }
    if (received.v_dop != 90u)    { fail(name, "vDOP mismatch"); return; }
    if (received.h_dop != 80u)    { fail(name, "hDOP mismatch"); return; }
    if (received.n_dop != 70u)    { fail(name, "nDOP mismatch"); return; }
    if (received.e_dop != 60u)    { fail(name, "eDOP mismatch"); return; }
    pass(name);
}

// ─── test: correct payload length accepted ────────────────────────────────────

static void test_nav_dop_correct_length()
{
    const char* name = "nav_dop_correct_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_dop_decoder>(
        new nav_dop_decoder([&](const ubx_nav_dop&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_DOP,
        build_nav_dop_payload(0u, 100u, 100u, 100u, 100u, 100u, 100u, 100u)));

    if (called)
        pass(name);
    else
        fail(name, "callback not called for valid 18-byte payload");
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_nav_dop_wrong_length()
{
    const char* name = "nav_dop_wrong_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_dop_decoder>(
        new nav_dop_decoder([&](const ubx_nav_dop&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_DOP, std::vector<uint8_t>(10u, 0u)));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for wrong payload length");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_nav_dop_fragmented()
{
    const char* name = "nav_dop_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_dop_decoder>(
        new nav_dop_decoder([&](const ubx_nav_dop&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_DOP,
        build_nav_dop_payload(0u, 99u, 99u, 99u, 99u, 99u, 99u, 99u));

    const std::size_t third = frame.size() / 3u;
    for (int i = 0; i < 2; ++i)
    {
        const std::size_t start = static_cast<std::size_t>(i) * third;
        p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(start),
                                     frame.begin() + static_cast<long>(start + third)));
        if (called) { fail(name, "fired too early"); return; }
    }
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(2u * third), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired after all chunks");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_nav_dop ===\n");
    test_nav_dop_basic_decode();
    test_nav_dop_correct_length();
    test_nav_dop_wrong_length();
    test_nav_dop_fragmented();
    std::printf("Done.\n");
    return 0;
}
