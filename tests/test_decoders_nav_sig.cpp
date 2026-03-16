// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_sig.cpp — Unit tests for nav_sig_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_sig_decoder.h"
#include "messages/ubx_nav_sig.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper: build NAV-SIG payload ────────────────────────────────────────────

static std::vector<uint8_t> build_nav_sig_payload(uint32_t itow,
                                                   uint8_t  num_sigs,
                                                   uint8_t  first_gnss_id = 0u,
                                                   uint8_t  first_sv_id   = 1u,
                                                   uint8_t  first_cno     = 35u)
{
    // header: iTOW(4) + version(1) + numSigs(1) + reserved(2) = 8 bytes
    std::vector<uint8_t> p;
    p.push_back(static_cast<uint8_t>(itow));
    p.push_back(static_cast<uint8_t>(itow >> 8u));
    p.push_back(static_cast<uint8_t>(itow >> 16u));
    p.push_back(static_cast<uint8_t>(itow >> 24u));
    p.push_back(0x00u);    // version
    p.push_back(num_sigs); // numSigs
    p.push_back(0x00u);    // reserved[0]
    p.push_back(0x00u);    // reserved[1]

    for (uint8_t i = 0u; i < num_sigs; ++i)
    {
        const uint8_t gid = (i == 0u) ? first_gnss_id : 0u;
        const uint8_t sid = (i == 0u) ? first_sv_id   : static_cast<uint8_t>(i + 1u);
        const uint8_t cno = (i == 0u) ? first_cno     : 25u;

        p.push_back(gid);    // gnssId
        p.push_back(sid);    // svId
        p.push_back(0u);     // sigId
        p.push_back(255u);   // freqId = unknown
        p.push_back(0x00u); p.push_back(0x00u); // prRes = 0
        p.push_back(cno);    // cno
        p.push_back(4u);     // qualInd
        p.push_back(0u);     // corrSource = none
        p.push_back(0u);     // ionoModel = none
        p.push_back(0x01u); p.push_back(0x00u); // sigFlags
        p.push_back(0u); p.push_back(0u); p.push_back(0u); p.push_back(0u); // reserved
    }
    return p;
}

// ─── test: zero signals ───────────────────────────────────────────────────────

static void test_nav_sig_zero_signals()
{
    const char* name = "nav_sig_zero_signals";

    ubx_nav_sig received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sig_decoder>(
        new nav_sig_decoder([&](const ubx_nav_sig& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SIG, build_nav_sig_payload(0u, 0u)));

    if (!called)                  { fail(name, "callback not called"); return; }
    if (received.num_sigs != 0u)  { fail(name, "num_sigs should be 0"); return; }
    if (!received.signals.empty()) { fail(name, "signals vector should be empty"); return; }
    pass(name);
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_nav_sig_basic_decode()
{
    const char* name = "nav_sig_basic_decode";

    ubx_nav_sig received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sig_decoder>(
        new nav_sig_decoder([&](const ubx_nav_sig& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SIG,
        build_nav_sig_payload(720000u, 2u, 0u, 5u, 38u)));

    if (!called)                              { fail(name, "callback not called"); return; }
    if (received.i_tow != 720000u)            { fail(name, "iTOW mismatch"); return; }
    if (received.num_sigs != 2u)              { fail(name, "num_sigs mismatch"); return; }
    if (received.signals.size() != 2u)        { fail(name, "signals.size() mismatch"); return; }
    if (received.signals[0].gnss_id != 0u)    { fail(name, "gnss_id mismatch"); return; }
    if (received.signals[0].sv_id != 5u)      { fail(name, "sv_id mismatch"); return; }
    if (received.signals[0].cno != 38u)       { fail(name, "cno mismatch"); return; }
    pass(name);
}

// ─── test: payload length mismatch rejected ───────────────────────────────────

static void test_nav_sig_wrong_length()
{
    const char* name = "nav_sig_wrong_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sig_decoder>(
        new nav_sig_decoder([&](const ubx_nav_sig&) { called = true; })));

    ubx_parser p(std::move(reg));
    // Build for 1 signal then patch numSigs to claim 3 — length won't match.
    auto bad = build_nav_sig_payload(0u, 1u);
    bad[5] = 3u; // claim 3 signals
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_SIG, bad));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for length mismatch");
}

// ─── test: too-short header rejected ─────────────────────────────────────────

static void test_nav_sig_too_short()
{
    const char* name = "nav_sig_too_short";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sig_decoder>(
        new nav_sig_decoder([&](const ubx_nav_sig&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SIG, { 0u, 0u, 0u }));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for too-short payload");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_nav_sig_fragmented()
{
    const char* name = "nav_sig_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sig_decoder>(
        new nav_sig_decoder([&](const ubx_nav_sig&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SIG, build_nav_sig_payload(0u, 2u));

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
    std::printf("=== test_decoders_nav_sig ===\n");
    test_nav_sig_zero_signals();
    test_nav_sig_basic_decode();
    test_nav_sig_wrong_length();
    test_nav_sig_too_short();
    test_nav_sig_fragmented();
    std::printf("Done.\n");
    return 0;
}
