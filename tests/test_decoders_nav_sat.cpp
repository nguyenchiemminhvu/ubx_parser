// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_sat.cpp — Unit tests for nav_sat_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_sat_decoder.h"
#include "messages/ubx_nav_sat.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helpers ──────────────────────────────────────────────────────────────────

/// Build a valid NAV-SAT payload with a configurable number of SV blocks.
static std::vector<uint8_t> build_nav_sat_payload(uint32_t itow,
                                                   uint8_t  num_svs,
                                                   uint8_t  gnss_id_0 = 0u,
                                                   uint8_t  sv_id_0   = 1u,
                                                   uint8_t  cno_0     = 30u)
{
    // header: iTOW(4) + version(1) + numSvs(1) + reserved(2) = 8 bytes
    std::vector<uint8_t> p;
    p.push_back(static_cast<uint8_t>(itow));
    p.push_back(static_cast<uint8_t>(itow >> 8u));
    p.push_back(static_cast<uint8_t>(itow >> 16u));
    p.push_back(static_cast<uint8_t>(itow >> 24u));
    p.push_back(0x01u);      // version
    p.push_back(num_svs);    // numSvs
    p.push_back(0x00u);      // reserved[0]
    p.push_back(0x00u);      // reserved[1]

    for (uint8_t i = 0u; i < num_svs; ++i)
    {
        const uint8_t gid = (i == 0u) ? gnss_id_0 : 0u;
        const uint8_t sid = (i == 0u) ? sv_id_0   : (static_cast<uint8_t>(i + 1u));
        const uint8_t cn  = (i == 0u) ? cno_0     : 20u;

        p.push_back(gid);    // gnssId
        p.push_back(sid);    // svId
        p.push_back(cn);     // cno
        p.push_back(45u);    // elev (signed, cast to uint8)
        p.push_back(0x2Cu); p.push_back(0x01u); // azim = 300 (LE signed int16)
        p.push_back(0x00u); p.push_back(0x00u); // prRes = 0
        p.push_back(0x07u); p.push_back(0x00u); p.push_back(0x00u); p.push_back(0x00u); // flags
    }
    return p;
}

// ─── test: zero SVs (empty payload body) ─────────────────────────────────────

static void test_nav_sat_zero_svs()
{
    const char* name = "nav_sat_zero_svs";

    ubx_nav_sat received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sat_decoder>(
        new nav_sat_decoder([&](const ubx_nav_sat& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SAT, build_nav_sat_payload(0u, 0u)));

    if (!called)               { fail(name, "callback not called"); return; }
    if (received.num_svs != 0u) { fail(name, "num_svs should be 0"); return; }
    if (!received.svs.empty())  { fail(name, "svs vector should be empty"); return; }
    pass(name);
}

// ─── test: basic decode with one SV ──────────────────────────────────────────

static void test_nav_sat_basic_decode()
{
    const char* name = "nav_sat_basic_decode";

    ubx_nav_sat received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sat_decoder>(
        new nav_sat_decoder([&](const ubx_nav_sat& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SAT,
        build_nav_sat_payload(360000u, 1u, 0u, 7u, 42u)));

    if (!called)                        { fail(name, "callback not called"); return; }
    if (received.i_tow != 360000u)      { fail(name, "iTOW mismatch"); return; }
    if (received.num_svs != 1u)         { fail(name, "num_svs mismatch"); return; }
    if (received.svs.size() != 1u)      { fail(name, "svs.size() mismatch"); return; }
    if (received.svs[0].gnss_id != 0u)  { fail(name, "gnss_id mismatch"); return; }
    if (received.svs[0].sv_id != 7u)    { fail(name, "sv_id mismatch"); return; }
    if (received.svs[0].cno != 42u)     { fail(name, "cno mismatch"); return; }
    pass(name);
}

// ─── test: multiple SVs ───────────────────────────────────────────────────────

static void test_nav_sat_multi_svs()
{
    const char* name = "nav_sat_multi_svs";

    ubx_nav_sat received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sat_decoder>(
        new nav_sat_decoder([&](const ubx_nav_sat& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    const uint8_t num_svs = 5u;
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SAT, build_nav_sat_payload(12345u, num_svs)));

    if (!called)                              { fail(name, "callback not called"); return; }
    if (received.num_svs != num_svs)          { fail(name, "num_svs mismatch"); return; }
    if (received.svs.size() != num_svs)       { fail(name, "svs.size() mismatch"); return; }
    pass(name);
}

// ─── test: payload length mismatch is rejected ───────────────────────────────

static void test_nav_sat_wrong_length()
{
    const char* name = "nav_sat_wrong_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sat_decoder>(
        new nav_sat_decoder([&](const ubx_nav_sat&) { called = true; })));

    ubx_parser p(std::move(reg));
    // Claim numSvs=2 but only provide 1 block of data (8 + 12 = 20 bytes).
    auto bad_payload = build_nav_sat_payload(0u, 1u);
    // Patch numSvs byte (offset 5) to claim 2 SVs to create a mismatch.
    bad_payload[5] = 2u;
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_SAT, bad_payload));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for length mismatch");
}

// ─── test: too-short payload is rejected ─────────────────────────────────────

static void test_nav_sat_too_short()
{
    const char* name = "nav_sat_too_short";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sat_decoder>(
        new nav_sat_decoder([&](const ubx_nav_sat&) { called = true; })));

    ubx_parser p(std::move(reg));
    // Payload shorter than the 8-byte header.
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SAT, { 0x01u, 0x02u, 0x03u }));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for too-short payload");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_nav_sat_fragmented()
{
    const char* name = "nav_sat_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_sat_decoder>(
        new nav_sat_decoder([&](const ubx_nav_sat&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_SAT, build_nav_sat_payload(0u, 3u));

    const std::size_t chunk = frame.size() / 4u;
    std::size_t pos = 0u;
    for (int i = 0; i < 3 && pos < frame.size(); ++i)
    {
        const std::size_t end = std::min(pos + chunk, frame.size());
        p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(pos),
                                     frame.begin() + static_cast<long>(end)));
        if (called) { fail(name, "fired too early"); return; }
        pos = end;
    }
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(pos), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired after all chunks");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_nav_sat ===\n");
    test_nav_sat_zero_svs();
    test_nav_sat_basic_decode();
    test_nav_sat_multi_svs();
    test_nav_sat_wrong_length();
    test_nav_sat_too_short();
    test_nav_sat_fragmented();
    std::printf("Done.\n");
    return 0;
}
