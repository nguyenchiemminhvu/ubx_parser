// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_esf_status.cpp — Unit tests for esf_status_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/esf_status_decoder.h"
#include "messages/ubx_esf_status.h"
#include "messages/ubx_esf_sensor_types.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

// Build variable-length ESF-STATUS payload
// Layout: [0-3]=iTOW, [4]=version, [5-11]=reserved(7),
//         [12]=fusionMode, [13-14]=reserved(2), [15]=numSens,
//         then numSens × 4-byte sensor blocks
static std::vector<uint8_t> build_esf_status_payload(
    uint32_t itow, uint8_t version, uint8_t fusion_mode,
    const std::vector<uint32_t>& sensor_words)
{
    const uint8_t n = static_cast<uint8_t>(sensor_words.size());
    std::vector<uint8_t> p(16u + 4u * n, 0u);

    // iTOW
    p[0] = static_cast<uint8_t>(itow);        p[1] = static_cast<uint8_t>(itow >>  8u);
    p[2] = static_cast<uint8_t>(itow >> 16u); p[3] = static_cast<uint8_t>(itow >> 24u);
    p[4] = version;
    // reserved [5-11]
    p[12] = fusion_mode;
    // reserved [13-14]
    p[15] = n;

    for (size_t i = 0; i < sensor_words.size(); ++i)
    {
        size_t off = 16u + i * 4u;
        p[off]   = static_cast<uint8_t>(sensor_words[i]);
        p[off+1] = static_cast<uint8_t>(sensor_words[i] >>  8u);
        p[off+2] = static_cast<uint8_t>(sensor_words[i] >> 16u);
        p[off+3] = static_cast<uint8_t>(sensor_words[i] >> 24u);
    }

    return p;
}

// ─── test: class / id ────────────────────────────────────────────────────────

static void test_esf_status_class_id()
{
    const char* name = "esf_status_class_and_id";
    esf_status_decoder d([](const ubx_esf_status&) {});
    if (d.msg_class() != UBX_CLASS_ESF)      { fail(name, "wrong class"); return; }
    if (d.msg_id()    != UBX_ID_ESF_STATUS)  { fail(name, "wrong id"); return; }
    pass(name);
}

// ─── test: decode with 2 sensors ─────────────────────────────────────────────

static void test_esf_status_two_sensors()
{
    const char* name = "esf_status_two_sensors";

    ubx_esf_status received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_status_decoder>(
        new esf_status_decoder([&](const ubx_esf_status& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // Each sensor block: [sensStatus1, sensStatus2, freq, faults]
    // Encode as 4-byte words (LE): word = sensStatus1 | (sensStatus2<<8) | (freq<<16) | (faults<<24)
    std::vector<uint32_t> sensors = {
        0xDD | (0xCCu << 8u) | (0xBBu << 16u) | (0xAAu << 24u),  // ss1=0xDD
        0x44 | (0x33u << 8u) | (0x22u << 16u) | (0x11u << 24u)   // ss1=0x44
    };
    const auto payload = build_esf_status_payload(123456u, 1u, 2u, sensors);
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_STATUS, payload);
    p.feed(frame);

    if (!called)                                   { fail(name, "callback not called"); return; }
    if (received.i_tow        != 123456u)          { fail(name, "iTOW mismatch"); return; }
    if (received.version      != 1u)               { fail(name, "version mismatch"); return; }
    if (received.fusion_mode  != 2u)               { fail(name, "fusion_mode mismatch"); return; }
    if (received.num_sens     != 2u)               { fail(name, "num_sens mismatch"); return; }
    if (received.sensors[0].sens_status1 != 0xDDu) { fail(name, "sensor[0].sens_status1"); return; }
    if (received.sensors[0].sens_status2 != 0xCCu) { fail(name, "sensor[0].sens_status2"); return; }
    if (received.sensors[0].freq         != 0xBBu) { fail(name, "sensor[0].freq"); return; }
    if (received.sensors[1].sens_status1 != 0x44u) { fail(name, "sensor[1].sens_status1"); return; }
    pass(name);
}

// ─── test: zero sensors ──────────────────────────────────────────────────────

static void test_esf_status_zero_sensors()
{
    const char* name = "esf_status_zero_sensors";

    ubx_esf_status received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_status_decoder>(
        new esf_status_decoder([&](const ubx_esf_status& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    const auto payload = build_esf_status_payload(0u, 0u, 0u, {});
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_STATUS, payload);
    p.feed(frame);

    if (!called)               { fail(name, "callback not called"); return; }
    if (received.num_sens != 0u) { fail(name, "num_sens should be 0"); return; }
    pass(name);
}

// ─── test: too-short payload rejected ────────────────────────────────────────

static void test_esf_status_too_short()
{
    const char* name = "esf_status_too_short";
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_status_decoder>(
        new esf_status_decoder([&](const ubx_esf_status&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_ESF, UBX_ID_ESF_STATUS, std::vector<uint8_t>(8u, 0u));
    p.feed(frame);

    if (!called) pass(name);
    else         fail(name, "callback should not fire for too-short payload");
}

// ─── test: numSens capped at MAX ─────────────────────────────────────────────

static void test_esf_status_sens_capped()
{
    const char* name = "esf_status_sens_capped";

    ubx_esf_status received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_status_decoder>(
        new esf_status_decoder([&](const ubx_esf_status& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // Build 40 sensor words but the decoder should cap at UBX_ESF_STATUS_MAX_SENS (32)
    // Payload layout (LE): byte0=sensStatus1, byte1=sensStatus2, byte2=freq, byte3=faults
    // word encoded as: sensStatus1 | (sensStatus2<<8) | (freq<<16) | (faults<<24)
    // so word = 0xABu | (0xCDu << 8u) | (0xEFu << 16u) | (0x12u << 24u)
    //   → sensStatus1=0xAB, sensStatus2=0xCD, freq=0xEF, faults=0x12
    const uint32_t sentinel = 0xABu | (0xCDu << 8u) | (0xEFu << 16u) | (0x12u << 24u);
    std::vector<uint32_t> sensors(40u, sentinel);
    const auto payload = build_esf_status_payload(0u, 0u, 0u, sensors);
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_STATUS, payload);
    p.feed(frame);

    if (!called)                                     { fail(name, "callback not called"); return; }
    if (received.num_sens > UBX_ESF_STATUS_MAX_SENS) { fail(name, "numSens not capped"); return; }
    if (received.sensors[UBX_ESF_STATUS_MAX_SENS - 1u].sens_status1 != 0xABu)
                                                     { fail(name, "last capped sensor wrong"); return; }
    pass(name);
}

// ─── test: sensor type extracted via esf_sensor_type enum ──────────────────────
//
// Build a 3-sensor message where each sensor uses a different named sensor type
// from esf_sensor_type.  After decoding, verify that
//   (sens_status1 & TYPE_MASK) == expected esf_sensor_type value.

static void test_esf_status_sensor_type_detection()
{
    const char* name = "esf_status_sensor_type_detection";

    ubx_esf_status received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<esf_status_decoder>(
        new esf_status_decoder([&](const ubx_esf_status& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // Build 3 sensor blocks:
    //  [0] Z-gyro (5), not used, not ready
    //  [1] X-accel (14), used (bit5), ready (bit6)  → 0x5E
    //  [2] speed (10)
    //
    // Each sensor block LE-word = ss1 | (ss2<<8) | (freq<<16) | (faults<<24)
    const uint8_t ss1_0 = static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE);   // 5
    const uint8_t ss1_1 = static_cast<uint8_t>(ESF_SENSOR_X_ACCEL)
                          | ubx_esf_status_sensor::USED
                          | ubx_esf_status_sensor::READY;                       // 14|0x20|0x40=0x5E
    const uint8_t ss1_2 = static_cast<uint8_t>(ESF_SENSOR_SPEED);              // 10

    std::vector<uint32_t> sensors = {
        static_cast<uint32_t>(ss1_0) | (0x00u << 8u) | (25u << 16u) | (0u << 24u),
        static_cast<uint32_t>(ss1_1) | (0x02u << 8u) | (50u << 16u) | (0u << 24u),
        static_cast<uint32_t>(ss1_2) | (0x00u << 8u) | (10u << 16u) | (0u << 24u)
    };
    const auto payload = build_esf_status_payload(999u, 2u, 1u, sensors);
    const auto frame   = ubx_message_builder::make_frame(UBX_CLASS_ESF, UBX_ID_ESF_STATUS, payload);
    p.feed(frame);

    if (!called)              { fail(name, "callback not called"); return; }
    if (received.num_sens != 3u) { fail(name, "num_sens mismatch"); return; }

    // sensor[0]: type only (neither used nor ready bits set)
    const uint8_t type0 = received.sensors[0].sens_status1 & ubx_esf_status_sensor::TYPE_MASK;
    if (type0 != static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE))
                              { fail(name, "sensor[0] type wrong"); return; }
    if (received.sensors[0].freq != 25u)
                              { fail(name, "sensor[0] freq wrong"); return; }

    // sensor[1]: type + used + ready
    const uint8_t type1 = received.sensors[1].sens_status1 & ubx_esf_status_sensor::TYPE_MASK;
    if (type1 != static_cast<uint8_t>(ESF_SENSOR_X_ACCEL))
                              { fail(name, "sensor[1] type wrong"); return; }
    if (!(received.sensors[1].sens_status1 & ubx_esf_status_sensor::USED))
                              { fail(name, "sensor[1] used bit missing"); return; }
    if (!(received.sensors[1].sens_status1 & ubx_esf_status_sensor::READY))
                              { fail(name, "sensor[1] ready bit missing"); return; }

    // sensor[2]: speed type
    const uint8_t type2 = received.sensors[2].sens_status1 & ubx_esf_status_sensor::TYPE_MASK;
    if (type2 != static_cast<uint8_t>(ESF_SENSOR_SPEED))
                              { fail(name, "sensor[2] type wrong"); return; }

    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_decoders_esf_status ===");
    test_esf_status_class_id();
    test_esf_status_two_sensors();
    test_esf_status_zero_sensors();
    test_esf_status_too_short();
    test_esf_status_sens_capped();
    test_esf_status_sensor_type_detection();
    return 0;
}
