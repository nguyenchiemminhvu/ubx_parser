// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_esf_meas.cpp — Unit tests for esf_meas_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/esf_meas_decoder.h"
#include "messages/ubx_esf_meas.h"
#include "messages/ubx_esf_sensor_types.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

// Encode the 8-byte header
static void encode_meas_header(std::vector<uint8_t>& p, size_t off,
                                uint32_t time_tag, uint16_t flags, uint16_t id)
{
    p[off]   = static_cast<uint8_t>(time_tag);
    p[off+1] = static_cast<uint8_t>(time_tag >>  8u);
    p[off+2] = static_cast<uint8_t>(time_tag >> 16u);
    p[off+3] = static_cast<uint8_t>(time_tag >> 24u);
    p[off+4] = static_cast<uint8_t>(flags);
    p[off+5] = static_cast<uint8_t>(flags >>  8u);
    p[off+6] = static_cast<uint8_t>(id);
    p[off+7] = static_cast<uint8_t>(id   >>  8u);
}

// Encode a single 4-byte measurement word:
//   bits [23:0]  = data_value (24-bit, takes the low 24 bits of data_value)
//   bits [29:24] = data_type (6 bits)
//   bits [31:30] = 0 (reserved)
static void encode_meas_word(std::vector<uint8_t>& p, size_t off,
                              uint8_t data_type, int32_t data_value)
{
    const uint32_t raw   = static_cast<uint32_t>(data_value) & 0x00FFFFFFu;
    const uint32_t word  = raw | (static_cast<uint32_t>(data_type & 0x3Fu) << 24u);
    p[off]   = static_cast<uint8_t>(word);
    p[off+1] = static_cast<uint8_t>(word >>  8u);
    p[off+2] = static_cast<uint8_t>(word >> 16u);
    p[off+3] = static_cast<uint8_t>(word >> 24u);
}

// ─── test: class / id ────────────────────────────────────────────────────────

static void test_esf_meas_class_id()
{
    const char* name = "esf_meas_class_and_id";
    esf_meas_decoder d([](const ubx_esf_meas&) {});
    if (d.msg_class() != UBX_CLASS_ESF)    { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_ESF_MEAS)  { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── test: header-only decode (0 measurements, no calibTtag) ─────────────────

static void test_esf_meas_header_only()
{
    const char* name = "esf_meas_header_only";

    ubx_esf_meas received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_meas_decoder>(
        new esf_meas_decoder([&](const ubx_esf_meas& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // flags bits[1:0] == 0 → no calibTtag; 0 measurement words
    std::vector<uint8_t> payload(8u, 0u);
    encode_meas_header(payload, 0u, 77777u, 0x0000u, 42u);

    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_MEAS, payload);
    p.feed(frame);

    if (!called)                       { fail(name, "callback not called"); return; }
    if (received.time_tag  != 77777u)  { fail(name, "time_tag mismatch"); return; }
    if (received.flags     != 0x0000u) { fail(name, "flags mismatch"); return; }
    if (received.id        != 42u)     { fail(name, "id mismatch"); return; }
    if (received.num_meas  != 0u)      { fail(name, "num_meas should be 0"); return; }
    if (received.has_calib_ttag)       { fail(name, "should have no calib_ttag"); return; }
    pass(name);
}

// ─── test: two positive measurements ─────────────────────────────────────────

static void test_esf_meas_two_measurements()
{
    const char* name = "esf_meas_two_measurements";

    ubx_esf_meas received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_meas_decoder>(
        new esf_meas_decoder([&](const ubx_esf_meas& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    std::vector<uint8_t> payload(16u, 0u);  // header(8) + 2×4 bytes
    encode_meas_header(payload, 0u, 100u, 0x0000u, 1u);
    encode_meas_word(payload, 8u,  13u, 1000);   // data_type=13 (x-accel), value=+1000
    encode_meas_word(payload, 12u, 14u, 2000);   // data_type=14 (y-accel), value=+2000

    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_MEAS, payload);
    p.feed(frame);

    if (!called)                             { fail(name, "callback not called"); return; }
    if (received.num_meas   != 2u)           { fail(name, "num_meas mismatch"); return; }
    if (received.data[0].data_type  != 13u)  { fail(name, "data[0] type mismatch"); return; }
    if (received.data[0].data_value != 1000) { fail(name, "data[0] value mismatch"); return; }
    if (received.data[1].data_type  != 14u)  { fail(name, "data[1] type mismatch"); return; }
    if (received.data[1].data_value != 2000) { fail(name, "data[1] value mismatch"); return; }
    pass(name);
}

// ─── test: negative data value (24-bit sign extension) ───────────────────────

static void test_esf_meas_negative_value()
{
    const char* name = "esf_meas_negative_value";

    ubx_esf_meas received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_meas_decoder>(
        new esf_meas_decoder([&](const ubx_esf_meas& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    std::vector<uint8_t> payload(12u, 0u);   // header(8) + 1×4
    encode_meas_header(payload, 0u, 0u, 0x0000u, 0u);
    // Encode -1 as 24-bit: 0xFFFFFF
    encode_meas_word(payload, 8u, 5u, -1);

    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_MEAS, payload);
    p.feed(frame);

    if (!called)                              { fail(name, "callback not called"); return; }
    if (received.data[0].data_value != -1)    { fail(name, "sign extension failed"); return; }
    pass(name);
}

// ─── test: calibTtag present (flags bits[1:0] != 0) ─────────────────────────

static void test_esf_meas_calib_ttag_present()
{
    const char* name = "esf_meas_calib_ttag_present";

    ubx_esf_meas received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_meas_decoder>(
        new esf_meas_decoder([&](const ubx_esf_meas& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // 1 measurement + calibTtag: header(8) + meas(4) + calibTtag(4) = 16
    std::vector<uint8_t> payload(16u, 0u);
    encode_meas_header(payload, 0u, 500u, 0x0001u, 7u); // flags bits[1:0]=1 → calibTtag present
    encode_meas_word(payload, 8u, 5u, 300);

    // calibTtag = 0xDEADBEEF
    const uint32_t calib = 0xDEADBEEFu;
    payload[12] = static_cast<uint8_t>(calib);
    payload[13] = static_cast<uint8_t>(calib >>  8u);
    payload[14] = static_cast<uint8_t>(calib >> 16u);
    payload[15] = static_cast<uint8_t>(calib >> 24u);

    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_MEAS, payload);
    p.feed(frame);

    if (!called)                              { fail(name, "callback not called"); return; }
    if (received.num_meas      != 1u)         { fail(name, "num_meas mismatch"); return; }
    if (!received.has_calib_ttag)             { fail(name, "has_calib_ttag should be true"); return; }
    if (received.calib_ttag    != 0xDEADBEEFu) { fail(name, "calib_ttag value mismatch"); return; }
    pass(name);
}

// ─── test: too-short payload rejected ────────────────────────────────────────

static void test_esf_meas_too_short()
{
    const char* name = "esf_meas_too_short";
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_meas_decoder>(
        new esf_meas_decoder([&](const ubx_esf_meas&) { called = true; })));

    ubx_parser p(std::move(reg));

    // Only 4 bytes — less than minimum header of 8
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_ESF, UBX_ID_ESF_MEAS, std::vector<uint8_t>(4u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for too-short payload");
}

// ─── test: data_type field uses esf_sensor_type enum values ──────────────────
//
// Encodes 4 measurements using named esf_sensor_type constants, then verifies
// that the decoded data_type fields match those constants exactly.

static void test_esf_meas_data_type_enum_values()
{
    const char* name = "esf_meas_data_type_enum_values";

    ubx_esf_meas received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_meas_decoder>(
        new esf_meas_decoder([&](const ubx_esf_meas& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // header(8) + 4 measurement words(16)
    std::vector<uint8_t> payload(24u, 0u);
    encode_meas_header(payload, 0u, 300u, 0x0000u, 0u);
    encode_meas_word(payload,  8u, static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE),  -100);
    encode_meas_word(payload, 12u, static_cast<uint8_t>(ESF_SENSOR_X_ACCEL),           2048);
    encode_meas_word(payload, 16u, static_cast<uint8_t>(ESF_SENSOR_Y_ACCEL),          -2048);
    encode_meas_word(payload, 20u, static_cast<uint8_t>(ESF_SENSOR_SPEED),              777);

    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_MEAS, payload);
    p.feed(frame);

    if (!called)             { fail(name, "callback not called"); return; }
    if (received.num_meas != 4u) { fail(name, "num_meas mismatch"); return; }

    if (received.data[0].data_type != static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE))
                             { fail(name, "data[0] type (Z_GYRO)");  return; }
    if (received.data[0].data_value != -100)
                             { fail(name, "data[0] value"); return; }

    if (received.data[1].data_type != static_cast<uint8_t>(ESF_SENSOR_X_ACCEL))
                             { fail(name, "data[1] type (X_ACCEL)"); return; }
    if (received.data[1].data_value != 2048)
                             { fail(name, "data[1] value"); return; }

    if (received.data[2].data_type != static_cast<uint8_t>(ESF_SENSOR_Y_ACCEL))
                             { fail(name, "data[2] type (Y_ACCEL)"); return; }
    if (received.data[2].data_value != -2048)
                             { fail(name, "data[2] value"); return; }

    if (received.data[3].data_type != static_cast<uint8_t>(ESF_SENSOR_SPEED))
                             { fail(name, "data[3] type (SPEED)");   return; }
    if (received.data[3].data_value != 777)
                             { fail(name, "data[3] value"); return; }

    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_esf_meas ===");
    test_esf_meas_class_id();
    test_esf_meas_header_only();
    test_esf_meas_two_measurements();
    test_esf_meas_negative_value();
    test_esf_meas_calib_ttag_present();
    test_esf_meas_too_short();
    test_esf_meas_data_type_enum_values();
    return 0;
}
