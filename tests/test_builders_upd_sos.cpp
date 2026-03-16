// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_builders_upd_sos.cpp — Unit tests for ubx_upd_sos_builder

#include "ubx_upd_sos_builder.h"
#include "ubx_protocol.h"
#include "ubx_types.h"

#include <cstdio>
#include <string>
#include <vector>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r)
{
    std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str());
}

// ─── helpers ─────────────────────────────────────────────────────────────────

static uint8_t  frame_class(const std::vector<uint8_t>& f) { return f[2]; }
static uint8_t  frame_id   (const std::vector<uint8_t>& f) { return f[3]; }
static uint16_t frame_len  (const std::vector<uint8_t>& f)
{
    return static_cast<uint16_t>(f[4]) |
           (static_cast<uint16_t>(f[5]) << 8u);
}
static uint8_t payload_u8(const std::vector<uint8_t>& f, std::size_t off)
{
    return f[6u + off];
}

// ─── test: poll frame (0-byte payload, 8 bytes total) ────────────────────────

static void test_upd_sos_poll()
{
    const char* name = "upd_sos_poll_frame";

    const auto frame = ubx_upd_sos_builder::build_poll();

    if (frame.size() != 8u)
    { fail(name, "expected 8 bytes for poll frame"); return; }
    if (frame[0] != UBX_SYNC_CHAR_1 || frame[1] != UBX_SYNC_CHAR_2)
    { fail(name, "wrong sync bytes"); return; }
    if (frame_class(frame) != UBX_CLASS_UPD)
    { fail(name, "wrong class"); return; }
    if (frame_id(frame) != UBX_ID_UPD_SOS)
    { fail(name, "wrong id"); return; }
    if (frame_len(frame) != 0u)
    { fail(name, "payload length field not 0"); return; }
    if (!protocol::verify_frame_checksum(frame))
    { fail(name, "checksum invalid"); return; }
    pass(name);
}

// ─── test: save frame (4-byte payload, 12 bytes total) ───────────────────────

static void test_upd_sos_save()
{
    const char* name = "upd_sos_save_frame";

    const auto frame = ubx_upd_sos_builder::build_save();

    if (frame.size() != 12u)
    { fail(name, "expected 12 bytes"); return; }
    if (frame_class(frame) != UBX_CLASS_UPD)
    { fail(name, "wrong class"); return; }
    if (frame_id(frame) != UBX_ID_UPD_SOS)
    { fail(name, "wrong id"); return; }
    if (frame_len(frame) != 4u)
    { fail(name, "wrong payload length field"); return; }
    // Payload byte 0 = cmd (save).
    if (payload_u8(frame, 0u) != UPD_SOS_CMD_SAVE)
    { fail(name, "byte 0: wrong cmd"); return; }
    // Payload bytes 1..3 = reserved0[3] -- must all be 0x00.
    if (payload_u8(frame, 1u) != 0x00u)
    { fail(name, "byte 1: reserved0[0] not zero"); return; }
    if (payload_u8(frame, 2u) != 0x00u)
    { fail(name, "byte 2: reserved0[1] not zero"); return; }
    if (payload_u8(frame, 3u) != 0x00u)
    { fail(name, "byte 3: reserved0[2] not zero"); return; }
    if (!protocol::verify_frame_checksum(frame))
    { fail(name, "checksum invalid"); return; }
    pass(name);
}

// ─── test: clear frame (4-byte payload, 12 bytes total) ──────────────────────

static void test_upd_sos_clear()
{
    const char* name = "upd_sos_clear_frame";

    const auto frame = ubx_upd_sos_builder::build_clear();

    if (frame.size() != 12u)
    { fail(name, "expected 12 bytes"); return; }
    if (frame_class(frame) != UBX_CLASS_UPD)
    { fail(name, "wrong class"); return; }
    if (frame_id(frame) != UBX_ID_UPD_SOS)
    { fail(name, "wrong id"); return; }
    if (frame_len(frame) != 4u)
    { fail(name, "wrong payload length field"); return; }
    // Payload byte 0 = cmd (clear).
    if (payload_u8(frame, 0u) != UPD_SOS_CMD_CLEAR)
    { fail(name, "byte 0: wrong cmd"); return; }
    // Payload bytes 1..3 = reserved0[3] -- must all be 0x00.
    if (payload_u8(frame, 1u) != 0x00u)
    { fail(name, "byte 1: reserved0[0] not zero"); return; }
    if (payload_u8(frame, 2u) != 0x00u)
    { fail(name, "byte 2: reserved0[1] not zero"); return; }
    if (payload_u8(frame, 3u) != 0x00u)
    { fail(name, "byte 3: reserved0[2] not zero"); return; }
    if (!protocol::verify_frame_checksum(frame))
    { fail(name, "checksum invalid"); return; }
    pass(name);
}

// ─── test: save and clear differ at byte 0 (cmd field) ─────────────────────────

static void test_upd_sos_save_vs_clear_cmd()
{
    const char* name = "upd_sos_save_vs_clear_cmd";

    const auto save  = ubx_upd_sos_builder::build_save();
    const auto clear = ubx_upd_sos_builder::build_clear();

    if (save.size() != clear.size())
    { fail(name, "frame sizes differ"); return; }
    // cmd lives at payload byte 0 (frame byte 6); only this byte must differ.
    if (payload_u8(save, 0u) == payload_u8(clear, 0u))
    { fail(name, "cmd bytes (byte 0) are equal — should differ"); return; }
    if (payload_u8(save, 0u) != UPD_SOS_CMD_SAVE)
    { fail(name, "save cmd byte wrong"); return; }
    if (payload_u8(clear, 0u) != UPD_SOS_CMD_CLEAR)
    { fail(name, "clear cmd byte wrong"); return; }
    // All reserved bytes (1..3) must be identical (both 0x00).
    for (std::size_t i = 1u; i <= 3u; ++i)
    {
        if (payload_u8(save, i) != payload_u8(clear, i))
        { fail(name, "reserved bytes should be identical in save and clear"); return; }
    }
    pass(name);
}

// ─── test: clear byte 0 == 0x01 (regression for old buggy layout) ─────────────
// Old incorrect layout had a version byte at position 0 and cmd at position 1,
// making clear: [0x00, 0x01, 0x00, 0x00].  Correct layout: [0x01, 0x00, 0x00, 0x00].

static void test_upd_sos_clear_cmd_at_byte_zero()
{
    const char* name = "upd_sos_clear_cmd_at_byte_zero";

    const auto frame = ubx_upd_sos_builder::build_clear();

    if (payload_u8(frame, 0u) != 0x01u)
    { fail(name, "byte 0 should be 0x01 (cmd=clear), not 0x00"); return; }
    if (payload_u8(frame, 1u) != 0x00u)
    { fail(name, "byte 1 should be 0x00 (reserved), not 0x01"); return; }
    pass(name);
}

// ─── test: UPD-SOS protocol constants ────────────────────────────────────────

static void test_upd_sos_constants()
{
    const char* name = "upd_sos_constants";

    if (UBX_CLASS_UPD != 0x09u)
    { fail(name, "UBX_CLASS_UPD != 0x09"); return; }
    if (UBX_ID_UPD_SOS != 0x14u)
    { fail(name, "UBX_ID_UPD_SOS != 0x14"); return; }
    if (UPD_SOS_CMD_SAVE != 0x00u)
    { fail(name, "UPD_SOS_CMD_SAVE != 0x00"); return; }
    if (UPD_SOS_CMD_CLEAR != 0x01u)
    { fail(name, "UPD_SOS_CMD_CLEAR != 0x01"); return; }
    if (UPD_SOS_CMD_ACK != 0x02u)
    { fail(name, "UPD_SOS_CMD_ACK != 0x02"); return; }
    if (UPD_SOS_CMD_RESTORE != 0x03u)
    { fail(name, "UPD_SOS_CMD_RESTORE != 0x03"); return; }
    if (UPD_SOS_RESP_NOT_ACK != 0x00u)
    { fail(name, "UPD_SOS_RESP_NOT_ACK != 0x00"); return; }
    if (UPD_SOS_RESP_ACK != 0x01u)
    { fail(name, "UPD_SOS_RESP_ACK != 0x01"); return; }
    pass(name);
}

// ─── main ────────────────────────────────────────────────────────────────────

int main()
{
    test_upd_sos_constants();
    test_upd_sos_poll();
    test_upd_sos_save();
    test_upd_sos_clear();
    test_upd_sos_save_vs_clear_cmd();
    test_upd_sos_clear_cmd_at_byte_zero();
    return 0;
}
