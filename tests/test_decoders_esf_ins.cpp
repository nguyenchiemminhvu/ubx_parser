// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_esf_ins.cpp — Unit tests for esf_ins_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/esf_ins_decoder.h"
#include "messages/ubx_esf_ins.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static void write_le32(std::vector<uint8_t>& v, size_t off, uint32_t val)
{
    v[off]   = static_cast<uint8_t>(val);
    v[off+1] = static_cast<uint8_t>(val >>  8u);
    v[off+2] = static_cast<uint8_t>(val >> 16u);
    v[off+3] = static_cast<uint8_t>(val >> 24u);
}

static std::vector<uint8_t> build_esf_ins_payload(
    uint32_t bitfield0, uint32_t itow,
    int32_t x_ang_rate, int32_t y_ang_rate, int32_t z_ang_rate,
    int32_t x_accel, int32_t y_accel, int32_t z_accel)
{
    std::vector<uint8_t> p(UBX_ESF_INS_PAYLOAD_LEN, 0u);

    write_le32(p, 0, bitfield0);
    // reserved at [4-7] = 0
    write_le32(p, 8, itow);
    write_le32(p, 12, static_cast<uint32_t>(x_ang_rate));
    write_le32(p, 16, static_cast<uint32_t>(y_ang_rate));
    write_le32(p, 20, static_cast<uint32_t>(z_ang_rate));
    write_le32(p, 24, static_cast<uint32_t>(x_accel));
    write_le32(p, 28, static_cast<uint32_t>(y_accel));
    write_le32(p, 32, static_cast<uint32_t>(z_accel));

    return p;
}

// ─── test: class / id ────────────────────────────────────────────────────────

static void test_esf_ins_class_id()
{
    const char* name = "esf_ins_class_and_id";
    esf_ins_decoder d([](const ubx_esf_ins&) {});
    if (d.msg_class() != UBX_CLASS_ESF) { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_ESF_INS) { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_esf_ins_basic_decode()
{
    const char* name = "esf_ins_basic_decode";

    ubx_esf_ins received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_ins_decoder>(
        new esf_ins_decoder([&](const ubx_esf_ins& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const auto payload = build_esf_ins_payload(
        0x00000007u,   // bitfield0: validity bits set for all axes
        360000u,       // iTOW ms
        100, -200, 300,   // angular rates (×1e-3 deg/s)
        -9800, 0, 50);    // acceleration (×1e-3 m/s²)

    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_INS, payload);
    p.feed(frame);

    if (!called)                              { fail(name, "callback not called"); return; }
    if (received.bitfield0     != 0x00000007u) { fail(name, "bitfield0 mismatch"); return; }
    if (received.i_tow         != 360000u)     { fail(name, "iTOW mismatch"); return; }
    if (received.x_ang_rate    != 100)         { fail(name, "x_ang_rate mismatch"); return; }
    if (received.y_ang_rate    != -200)        { fail(name, "y_ang_rate mismatch"); return; }
    if (received.z_ang_rate    != 300)         { fail(name, "z_ang_rate mismatch"); return; }
    if (received.x_accel       != -9800)       { fail(name, "x_accel mismatch"); return; }
    if (received.y_accel       != 0)           { fail(name, "y_accel mismatch"); return; }
    if (received.z_accel       != 50)          { fail(name, "z_accel mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length ──────────────────────────────────────────────

static void test_esf_ins_wrong_length()
{
    const char* name = "esf_ins_wrong_length";
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_ins_decoder>(
        new esf_ins_decoder([&](const ubx_esf_ins&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_ESF, UBX_ID_ESF_INS, std::vector<uint8_t>(20u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for wrong length");
}

// ─── test: extreme signed values (min int32) ─────────────────────────────────

static void test_esf_ins_signed_extremes()
{
    const char* name = "esf_ins_signed_extremes";

    ubx_esf_ins received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_ins_decoder>(
        new esf_ins_decoder([&](const ubx_esf_ins& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const int32_t min_val = -2147483647 - 1;   // INT32_MIN
    const auto payload = build_esf_ins_payload(0u, 0u, min_val, min_val, min_val, min_val, min_val, min_val);
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_INS, payload);
    p.feed(frame);

    if (!called)                      { fail(name, "callback not called"); return; }
    if (received.x_ang_rate != min_val) { fail(name, "x_ang_rate sign fail"); return; }
    if (received.x_accel    != min_val) { fail(name, "x_accel sign fail"); return; }
    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_esf_ins ===");
    test_esf_ins_class_id();
    test_esf_ins_basic_decode();
    test_esf_ins_wrong_length();
    test_esf_ins_signed_extremes();
    return 0;
}
