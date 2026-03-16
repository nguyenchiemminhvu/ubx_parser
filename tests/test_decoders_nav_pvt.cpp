// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_nav_pvt.cpp — Unit tests for nav_pvt_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/nav_pvt_decoder.h"
#include "messages/ubx_nav_pvt.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helpers ──────────────────────────────────────────────────────────────────

/// Build a synthetic NAV-PVT payload with specific fields.
/// Fields not explicitly set are zero.
static std::vector<uint8_t> build_nav_pvt_payload(uint32_t itow, int32_t lat, int32_t lon,
                                                    nav_pvt_fix_type fix_type, uint8_t num_sv)
{
    std::vector<uint8_t> p(UBX_NAV_PVT_PAYLOAD_LEN, 0u);

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

static void test_nav_pvt_basic_decode()
{
    const char* name = "nav_pvt_basic_decode";

    ubx_nav_pvt received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_pvt_decoder>(
        new nav_pvt_decoder([&](const ubx_nav_pvt& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const auto payload = build_nav_pvt_payload(360000u, 473996880, 85344650,
                                                nav_pvt_fix_type::fix_3d, 12u);
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_PVT, payload);
    p.feed(frame);

    if (!called)         { fail(name, "callback not called"); return; }
    if (received.i_tow != 360000u) { fail(name, "iTOW mismatch"); return; }
    if (received.lat   != 473996880) { fail(name, "lat mismatch"); return; }
    if (received.lon   != 85344650)  { fail(name, "lon mismatch"); return; }
    if (received.fix_type != nav_pvt_fix_type::fix_3d) { fail(name, "fix_type mismatch"); return; }
    if (received.num_sv != 12u)      { fail(name, "num_sv mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length is rejected ───────────────────────────────────

static void test_nav_pvt_wrong_length()
{
    const char* name = "nav_pvt_wrong_length";

    bool called = false;
    int  errors = 0;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_pvt_decoder>(
        new nav_pvt_decoder([&](const ubx_nav_pvt&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.set_error_callback([&](const parse_error_info&) { ++errors; });

    // Build frame with payload of wrong size (10 bytes instead of 92)
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_NAV, UBX_ID_NAV_PVT, std::vector<uint8_t>(10u, 0u));
    p.feed(frame);

    // Decoder returns false → dispatch returns false → no error callback from parser,
    // but the frame is still "decoded" (checksum ok).  The callback should NOT fire.
    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for wrong payload length");
}

// ─── test: fragmented NAV-PVT across 5 chunks ────────────────────────────────

static void test_nav_pvt_fragmented()
{
    const char* name = "nav_pvt_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<nav_pvt_decoder>(
        new nav_pvt_decoder([&](const ubx_nav_pvt&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto payload = build_nav_pvt_payload(0u, 0, 0, nav_pvt_fix_type::no_fix, 0u);
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_PVT, payload);

    // Split into 5 chunks
    const std::size_t chunk_size = frame.size() / 5u;
    std::size_t pos = 0u;
    for (int i = 0; i < 4 && pos < frame.size(); ++i)
    {
        const std::size_t end = std::min(pos + chunk_size, frame.size());
        p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(pos),
                                     frame.begin() + static_cast<long>(end)));
        if (called) { fail(name, "callback fired too early"); return; }
        pos = end;
    }
    // Feed remainder
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(pos), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired after all chunks");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_nav_pvt ===\n");
    test_nav_pvt_basic_decode();
    test_nav_pvt_wrong_length();
    test_nav_pvt_fragmented();
    std::printf("Done.\n");
    return 0;
}
