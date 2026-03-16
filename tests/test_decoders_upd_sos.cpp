// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_upd_sos.cpp — Unit tests for upd_sos_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/upd_sos_decoder.h"
#include "messages/ubx_upd_sos.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r)
{
    std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str());
}

// ─── helper: build a UBX-UPD-SOS output payload ──────────────────────────────

// Constructs the correct 8-byte payload for UBX-UPD-SOS receiver-output frames.
//
// Correct layout per u-blox spec (no version field):
//   [0]     cmd         U1  - 0x02 (ack) or 0x03 (restore status)
//   [1..3]  reserved0   U1x3
//   [4]     response    U1  - 0 = not successful, 1 = successful
//   [5..7]  reserved1   U1x3
static std::vector<uint8_t> build_upd_sos_payload(uint8_t cmd, uint8_t response)
{
    std::vector<uint8_t> p;
    p.push_back(cmd);
    p.push_back(0x00u);  // reserved0[0]
    p.push_back(0x00u);  // reserved0[1]
    p.push_back(0x00u);  // reserved0[2]
    p.push_back(response);
    p.push_back(0x00u);  // reserved1[0]
    p.push_back(0x00u);  // reserved1[1]
    p.push_back(0x00u);  // reserved1[2]
    return p;
}

// ─── test: basic decode — backup creation acknowledged ───────────────────────

static void test_upd_sos_basic_decode()
{
    const char* name = "upd_sos_basic_decode";

    ubx_upd_sos_output received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
        new upd_sos_decoder([&](const ubx_upd_sos_output& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
        build_upd_sos_payload(UPD_SOS_CMD_ACK, UPD_SOS_RESP_ACK)));

    if (!called)
    { fail(name, "callback not called"); return; }
    if (received.cmd != UPD_SOS_CMD_ACK)
    { fail(name, "wrong cmd"); return; }
    if (received.response != UPD_SOS_RESP_ACK)
    { fail(name, "wrong response"); return; }
    pass(name);
}

// ─── test: not acknowledged (response == 0) ───────────────────────────────────

static void test_upd_sos_not_acked()
{
    const char* name = "upd_sos_not_acked";

    ubx_upd_sos_output received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
        new upd_sos_decoder([&](const ubx_upd_sos_output& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
        build_upd_sos_payload(UPD_SOS_CMD_ACK, UPD_SOS_RESP_NOT_ACK)));

    if (!called)
    { fail(name, "callback not called"); return; }
    if (received.response != UPD_SOS_RESP_NOT_ACK)
    { fail(name, "expected response == UPD_SOS_RESP_NOT_ACK (0)"); return; }
    pass(name);
}

// ─── test: system restored from backup (cmd=0x03) ────────────────────────────

static void test_upd_sos_restore_status()
{
    const char* name = "upd_sos_restore_status";

    ubx_upd_sos_output received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
        new upd_sos_decoder([&](const ubx_upd_sos_output& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
        build_upd_sos_payload(UPD_SOS_CMD_RESTORE, UPD_SOS_RESP_ACK)));

    if (!called)
    { fail(name, "callback not called"); return; }
    if (received.cmd != UPD_SOS_CMD_RESTORE)
    { fail(name, "wrong cmd — expected UPD_SOS_CMD_RESTORE (0x03)"); return; }
    if (received.response != UPD_SOS_RESP_ACK)
    { fail(name, "wrong response"); return; }
    pass(name);
}

// ─── test: payload too small is rejected ──────────────────────────────────────

static void test_upd_sos_payload_too_small()
{
    const char* name = "upd_sos_payload_too_small";

    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
        new upd_sos_decoder([&](const ubx_upd_sos_output&) { called = true; })));

    ubx_parser p(std::move(reg));
    // Feed a 4-byte payload — less than UBX_UPD_SOS_OUTPUT_PAYLOAD_LEN (8).
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
                                            { 0x02u, 0x00u, 0x00u, 0x00u }));

    if (!called)
        pass(name);
    else
        fail(name, "callback fired for undersized payload");
}

// ─── test: input-command cmd values are rejected ─────────────────────────────
// cmd=0x00 (save) and cmd=0x01 (clear) are host→receiver commands; they must
// never trigger the output callback even if they somehow appear in the stream.

static void test_upd_sos_input_cmd_rejected()
{
    {
        const char* name = "upd_sos_save_cmd_rejected";
        bool called = false;
        ubx_decoder_registry reg;
        reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
            new upd_sos_decoder([&](const ubx_upd_sos_output&) { called = true; })));
        ubx_parser p(std::move(reg));
        p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
            build_upd_sos_payload(UPD_SOS_CMD_SAVE, UPD_SOS_RESP_ACK)));
        if (!called) pass(name);
        else         fail(name, "callback fired for input save cmd");
    }
    {
        const char* name = "upd_sos_clear_cmd_rejected";
        bool called = false;
        ubx_decoder_registry reg;
        reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
            new upd_sos_decoder([&](const ubx_upd_sos_output&) { called = true; })));
        ubx_parser p(std::move(reg));
        p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
            build_upd_sos_payload(UPD_SOS_CMD_CLEAR, UPD_SOS_RESP_ACK)));
        if (!called) pass(name);
        else         fail(name, "callback fired for input clear cmd");
    }
}

// ─── test: unknown cmd byte is rejected ──────────────────────────────────────

static void test_upd_sos_unknown_cmd()
{
    const char* name = "upd_sos_unknown_cmd";

    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
        new upd_sos_decoder([&](const ubx_upd_sos_output&) { called = true; })));

    ubx_parser p(std::move(reg));
    // cmd = 0xFF — not a valid output sub-command.
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
        build_upd_sos_payload(0xFFu, UPD_SOS_RESP_ACK)));

    if (!called)
        pass(name);
    else
        fail(name, "callback fired for unknown cmd byte");
}

// ─── test: fragmented feed across 3 calls ────────────────────────────────────

static void test_upd_sos_fragmented()
{
    const char* name = "upd_sos_fragmented";

    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
        new upd_sos_decoder([&](const ubx_upd_sos_output&) { called = true; })));

    ubx_parser p(std::move(reg));

    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
        build_upd_sos_payload(UPD_SOS_CMD_ACK, UPD_SOS_RESP_ACK));

    const std::size_t third = frame.size() / 3u;
    for (int i = 0; i < 2; ++i)
    {
        const std::size_t start = static_cast<std::size_t>(i) * third;
        const std::size_t end   = start + third;
        p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(start),
                                     frame.begin() + static_cast<long>(end)));
        if (called) { fail(name, "callback fired too early"); return; }
    }
    // Feed the remainder.
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(2u * third), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired after all feeds");
}

// ─── test: response field is faithfully forwarded (U1) ───────────────────────

static void test_upd_sos_response_forwarded()
{
    const char* name = "upd_sos_response_forwarded";

    ubx_upd_sos_output received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<upd_sos_decoder>(
        new upd_sos_decoder([&](const ubx_upd_sos_output& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    // 0xCA is an unusual U1 response value — confirms no silent truncation.
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS,
        build_upd_sos_payload(UPD_SOS_CMD_ACK, 0xCAu)));

    if (!called)
    { fail(name, "callback not called"); return; }
    if (received.response != 0xCAu)
    { fail(name, "response value corrupted"); return; }
    pass(name);
}

// ─── main ────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_upd_sos ===\n");
    test_upd_sos_basic_decode();
    test_upd_sos_not_acked();
    test_upd_sos_restore_status();
    test_upd_sos_payload_too_small();
    test_upd_sos_input_cmd_rejected();
    test_upd_sos_unknown_cmd();
    test_upd_sos_fragmented();
    test_upd_sos_response_forwarded();
    std::printf("Done.\n");
    return 0;
}
