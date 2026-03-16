// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_eell.cpp — Unit tests for nav_eell_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_eell_decoder.h"
#include "messages/ubx_nav_eell.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_nav_eell_payload(uint32_t itow,
                                                    uint8_t  version,
                                                    uint16_t err_maj,
                                                    uint16_t err_min,
                                                    uint16_t err_orient)
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
    p.push_back(version);
    p.push_back(0x00u); p.push_back(0x00u); p.push_back(0x00u); // reserved1
    push_u16(err_maj);
    push_u16(err_min);
    push_u16(err_orient);
    push_u16(0u); // reserved2
    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav_eell_basic_decode()
{
    const char* name = "nav_eell_basic_decode";

    ubx_nav_eell received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_eell_decoder>(
        new nav_eell_decoder([&](const ubx_nav_eell& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_EELL,
        build_nav_eell_payload(500000u, 0u, 1234u, 567u, 9000u)));

    if (!called)                        { fail(name, "callback not called"); return; }
    if (received.i_tow != 500000u)      { fail(name, "iTOW mismatch"); return; }
    if (received.version != 0u)         { fail(name, "version mismatch"); return; }
    if (received.err_maj != 1234u)      { fail(name, "err_maj mismatch"); return; }
    if (received.err_min != 567u)       { fail(name, "err_min mismatch"); return; }
    if (received.err_orient != 9000u)   { fail(name, "err_orient mismatch"); return; }
    pass(name);
}

// ─── test: boundary values ────────────────────────────────────────────────────

static void test_nav_eell_boundary_values()
{
    const char* name = "nav_eell_boundary_values";

    ubx_nav_eell received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_eell_decoder>(
        new nav_eell_decoder([&](const ubx_nav_eell& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_EELL,
        build_nav_eell_payload(0xFFFFFFFFu, 0u, 0xFFFFu, 0xFFFFu, 0xFFFFu)));

    if (!called)                          { fail(name, "callback not called"); return; }
    if (received.i_tow != 0xFFFFFFFFu)    { fail(name, "iTOW mismatch"); return; }
    if (received.err_maj != 0xFFFFu)      { fail(name, "err_maj mismatch"); return; }
    if (received.err_min != 0xFFFFu)      { fail(name, "err_min mismatch"); return; }
    if (received.err_orient != 0xFFFFu)   { fail(name, "err_orient mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_nav_eell_wrong_length()
{
    const char* name = "nav_eell_wrong_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_eell_decoder>(
        new nav_eell_decoder([&](const ubx_nav_eell&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_EELL, std::vector<uint8_t>(8u, 0u)));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for wrong payload length");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_nav_eell_fragmented()
{
    const char* name = "nav_eell_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_eell_decoder>(
        new nav_eell_decoder([&](const ubx_nav_eell&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_EELL,
        build_nav_eell_payload(0u, 0u, 100u, 50u, 1800u));

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
    std::printf("=== test_decoders_nav_eell ===\n");
    test_nav_eell_basic_decode();
    test_nav_eell_boundary_values();
    test_nav_eell_wrong_length();
    test_nav_eell_fragmented();
    std::printf("Done.\n");
    return 0;
}
