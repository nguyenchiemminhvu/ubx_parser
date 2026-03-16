// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_checksum.cpp — Unit tests for UBX Fletcher-8 checksum

#include "ubx_protocol.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <string>

using namespace ubx::parser;
using namespace ubx::parser::protocol;

// ─── helpers ──────────────────────────────────────────────────────────────────

static void pass(const std::string& name)
{
    std::printf("[PASS] %s\n", name.c_str());
}

static void fail(const std::string& name, const std::string& reason)
{
    std::printf("[FAIL] %s: %s\n", name.c_str(), reason.c_str());
}

// ─── tests ────────────────────────────────────────────────────────────────────

/// Verify the checksum of a known-good UBX-NAV-POSLLH frame snippet.
/// The bytes covered by the checksum are: class=0x01, id=0x02, len=0x1C00,
/// followed by the 28-byte payload.
/// Expected: CK_A=0x?? CK_B=0x?? — we verify the function is consistent
/// (round-trip) rather than testing against a hardware-captured value.
static void test_checksum_roundtrip()
{
    const char* name = "checksum_roundtrip";

    // Build a small payload
    const std::vector<uint8_t> payload(28u, 0xAAu);
    const auto frame = build_ubx_frame(0x01u, 0x02u, payload);

    // The last two bytes of frame are CK_A, CK_B
    const std::size_t sz = frame.size();
    const uint8_t expected_ck_a = frame[sz - 2u];
    const uint8_t expected_ck_b = frame[sz - 1u];

    // Recompute independently
    uint8_t ck_a = 0u, ck_b = 0u;
    compute_checksum(frame.data() + 2u, sz - 4u, ck_a, ck_b);

    if (ck_a == expected_ck_a && ck_b == expected_ck_b)
        pass(name);
    else
        fail(name, "computed checksum differs from frame bytes");
}

/// Verify that verify_frame_checksum returns true for a correct frame.
static void test_verify_good_frame()
{
    const char* name = "verify_good_frame";

    const std::vector<uint8_t> frame = build_ubx_frame(0x0Au, 0x04u, { 0x01u, 0x02u });
    if (verify_frame_checksum(frame))
        pass(name);
    else
        fail(name, "valid frame rejected");
}

/// Verify that verify_frame_checksum returns false when CK_B is corrupted.
static void test_verify_bad_frame()
{
    const char* name = "verify_bad_frame";

    std::vector<uint8_t> frame = build_ubx_frame(0x0Au, 0x04u, { 0x01u, 0x02u });
    frame.back() ^= 0xFFu; // corrupt CK_B

    if (!verify_frame_checksum(frame))
        pass(name);
    else
        fail(name, "corrupted frame accepted as valid");
}

/// Empty-payload frame: checksum still valid.
static void test_checksum_empty_payload()
{
    const char* name = "checksum_empty_payload";

    const std::vector<uint8_t> frame = build_ubx_frame(0x06u, 0x8Bu, {});
    if (verify_frame_checksum(frame))
        pass(name);
    else
        fail(name, "empty-payload frame rejected");
}

/// All-zero payload frame.
static void test_checksum_all_zero_payload()
{
    const char* name = "checksum_all_zero_payload";

    const std::vector<uint8_t> frame = build_ubx_frame(0x01u, 0x07u, std::vector<uint8_t>(92u, 0u));
    if (verify_frame_checksum(frame))
        pass(name);
    else
        fail(name, "all-zero payload frame rejected");
}

/// Frame too small returns false.
static void test_verify_too_small()
{
    const char* name = "verify_too_small";

    const std::vector<uint8_t> too_small = { 0xB5u, 0x62u, 0x01u };
    if (!verify_frame_checksum(too_small))
        pass(name);
    else
        fail(name, "undersized frame accepted");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_checksum ===\n");
    test_checksum_roundtrip();
    test_verify_good_frame();
    test_verify_bad_frame();
    test_checksum_empty_payload();
    test_checksum_all_zero_payload();
    test_verify_too_small();
    std::printf("Done.\n");
    return 0;
}
