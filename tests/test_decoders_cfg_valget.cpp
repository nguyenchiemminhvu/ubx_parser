// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_cfg_valget.cpp — Unit tests for cfg_valget_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/cfg_valget_decoder.h"
#include "messages/ubx_cfg_valget.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper: build response payload with two entries ─────────────────────────
//
// rate_meas (size 3 → uint16_t) + signal_gps_ena (size 1 → uint8_t)

static std::vector<uint8_t> build_valget_payload()
{
    std::vector<uint8_t> p;
    p.push_back(0x01u); // version = response
    p.push_back(0x00u); // layer = RAM
    p.push_back(0x00u); p.push_back(0x00u); // position

    // rate_meas key 0x30210001, value 500 (LE uint16_t)
    p.push_back(0x01u); p.push_back(0x00u); p.push_back(0x21u); p.push_back(0x30u);
    p.push_back(0xF4u); p.push_back(0x01u); // 500

    // signal_gps_ena key 0x1031001F, value 1 (uint8_t)
    p.push_back(0x1Fu); p.push_back(0x00u); p.push_back(0x31u); p.push_back(0x10u);
    p.push_back(0x01u);

    return p;
}

// ─── test: basic decode with two key-value pairs ──────────────────────────────

static void test_cfg_valget_two_entries()
{
    const char* name = "cfg_valget_two_entries";

    ubx_cfg_valget received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<cfg_valget_decoder>(
        new cfg_valget_decoder([&](const ubx_cfg_valget& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_CFG, UBX_ID_CFG_VALGET,
                                            build_valget_payload()));

    if (!called)               { fail(name, "callback not called"); return; }
    if (received.version != 1u) { fail(name, "wrong version"); return; }
    if (received.layer != 0u)   { fail(name, "wrong layer"); return; }
    if (received.entries.size() != 2u) { fail(name, "wrong entry count"); return; }

    // Check rate_meas entry
    const auto& e0 = received.entries[0];
    if (e0.key_id != 0x30210001u) { fail(name, "wrong key[0]"); return; }
    if (e0.value  != 500u)        { fail(name, "wrong value[0]"); return; }

    // Check signal_gps_ena entry
    const auto& e1 = received.entries[1];
    if (e1.key_id != 0x1031001Fu) { fail(name, "wrong key[1]"); return; }
    if (e1.value  != 1u)          { fail(name, "wrong value[1]"); return; }

    pass(name);
}

// ─── test: empty entries (header only) ───────────────────────────────────────

static void test_cfg_valget_header_only()
{
    const char* name = "cfg_valget_header_only";

    ubx_cfg_valget received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<cfg_valget_decoder>(
        new cfg_valget_decoder([&](const ubx_cfg_valget& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));

    // Payload: header only (4 bytes)
    const std::vector<uint8_t> hdr = { 0x01u, 0x00u, 0x00u, 0x00u };
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_CFG, UBX_ID_CFG_VALGET, hdr));

    if (!called)                    { fail(name, "callback not called"); return; }
    if (!received.entries.empty())  { fail(name, "expected zero entries"); return; }
    pass(name);
}

// ─── test: payload too small rejects gracefully ───────────────────────────────

static void test_cfg_valget_too_small()
{
    const char* name = "cfg_valget_too_small";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<cfg_valget_decoder>(
        new cfg_valget_decoder([&](const ubx_cfg_valget&) { called = true; })));

    ubx_parser p(std::move(reg));
    // Payload: 2 bytes only (less than 4-byte header)
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_CFG, UBX_ID_CFG_VALGET, { 0x01u, 0x00u }));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for undersized payload");
}

// ─── test: fragmented CFG-VALGET across 2 feeds ──────────────────────────────

static void test_cfg_valget_fragmented()
{
    const char* name = "cfg_valget_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<cfg_valget_decoder>(
        new cfg_valget_decoder([&](const ubx_cfg_valget&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_CFG, UBX_ID_CFG_VALGET, build_valget_payload());

    const std::size_t split = frame.size() / 2u;
    p.feed(std::vector<uint8_t>(frame.begin(), frame.begin() + static_cast<long>(split)));
    if (called) { fail(name, "fired before complete"); return; }
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(split), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired after second feed");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_cfg_valget ===\n");
    test_cfg_valget_two_entries();
    test_cfg_valget_header_only();
    test_cfg_valget_too_small();
    test_cfg_valget_fragmented();
    std::printf("Done.\n");
    return 0;
}
