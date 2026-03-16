// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_builders_cfg_rst.cpp — Unit tests for ubx_cfg_rst_builder

#include "ubx_cfg_rst_builder.h"
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

// Extract the UBX frame header fields from a raw frame byte vector.
// Frame layout: [0xB5][0x62][cls][id][len_lo][len_hi][payload...][ck_a][ck_b]
static uint8_t  frame_class(const std::vector<uint8_t>& f) { return f[2]; }
static uint8_t  frame_id   (const std::vector<uint8_t>& f) { return f[3]; }
static uint16_t frame_len  (const std::vector<uint8_t>& f)
{
    return static_cast<uint16_t>(f[4]) |
           (static_cast<uint16_t>(f[5]) << 8u);
}
// Extract a LE-16 value from the payload (payload starts at offset 6).
static uint16_t payload_le16(const std::vector<uint8_t>& f, std::size_t off)
{
    return static_cast<uint16_t>(f[6u + off]) |
           (static_cast<uint16_t>(f[7u + off]) << 8u);
}
static uint8_t payload_u8(const std::vector<uint8_t>& f, std::size_t off)
{
    return f[6u + off];
}

// ─── test: hot-start frame ────────────────────────────────────────────────────

static void test_cfg_rst_hot_start()
{
    const char* name = "cfg_rst_hot_start";

    const auto frame = ubx_cfg_rst_builder::build_hot_start();

    // Expected total size: 8 (overhead) + 4 (payload) = 12 bytes.
    if (frame.size() != 12u)
    { fail(name, "unexpected frame size"); return; }
    if (frame[0] != UBX_SYNC_CHAR_1 || frame[1] != UBX_SYNC_CHAR_2)
    { fail(name, "wrong sync bytes"); return; }
    if (frame_class(frame) != UBX_CLASS_CFG_RST)
    { fail(name, "wrong class"); return; }
    if (frame_id(frame) != UBX_ID_CFG_RST)
    { fail(name, "wrong id"); return; }
    if (frame_len(frame) != 4u)
    { fail(name, "wrong payload length field"); return; }
    // navBbrMask at payload offset 0: 0x0000 (hot start)
    if (payload_le16(frame, 0u) != NAV_BBR_HOT_START)
    { fail(name, "wrong navBbrMask"); return; }
    // resetMode at payload offset 2: 0x01 (controlled software reset)
    if (payload_u8(frame, 2u) != CFG_RST_MODE_CTRL_SW)
    { fail(name, "wrong resetMode"); return; }
    // reserved1 at payload offset 3: 0x00
    if (payload_u8(frame, 3u) != 0x00u)
    { fail(name, "reserved byte not zero"); return; }
    if (!protocol::verify_frame_checksum(frame))
    { fail(name, "checksum invalid"); return; }
    pass(name);
}

// ─── test: warm-start frame ───────────────────────────────────────────────────

static void test_cfg_rst_warm_start()
{
    const char* name = "cfg_rst_warm_start";

    const auto frame = ubx_cfg_rst_builder::build_warm_start();

    if (frame.size() != 12u)
    { fail(name, "unexpected frame size"); return; }
    if (frame_class(frame) != UBX_CLASS_CFG_RST)
    { fail(name, "wrong class"); return; }
    if (frame_id(frame) != UBX_ID_CFG_RST)
    { fail(name, "wrong id"); return; }
    if (payload_le16(frame, 0u) != NAV_BBR_WARM_START)
    { fail(name, "wrong navBbrMask"); return; }
    if (payload_u8(frame, 2u) != CFG_RST_MODE_CTRL_SW)
    { fail(name, "wrong resetMode"); return; }
    if (!protocol::verify_frame_checksum(frame))
    { fail(name, "checksum invalid"); return; }
    pass(name);
}

// ─── test: cold-start frame ───────────────────────────────────────────────────

static void test_cfg_rst_cold_start()
{
    const char* name = "cfg_rst_cold_start";

    const auto frame = ubx_cfg_rst_builder::build_cold_start();

    if (frame.size() != 12u)
    { fail(name, "unexpected frame size"); return; }
    if (frame_class(frame) != UBX_CLASS_CFG_RST)
    { fail(name, "wrong class"); return; }
    if (frame_id(frame) != UBX_ID_CFG_RST)
    { fail(name, "wrong id"); return; }
    if (payload_le16(frame, 0u) != NAV_BBR_COLD_START)
    { fail(name, "wrong navBbrMask"); return; }
    if (payload_u8(frame, 2u) != CFG_RST_MODE_CTRL_SW)
    { fail(name, "wrong resetMode"); return; }
    if (!protocol::verify_frame_checksum(frame))
    { fail(name, "checksum invalid"); return; }
    pass(name);
}

// ─── test: all three variants have distinct navBbrMask ────────────────────────

static void test_cfg_rst_variants_distinct()
{
    const char* name = "cfg_rst_variants_distinct";

    const auto hot  = ubx_cfg_rst_builder::build_hot_start();
    const auto warm = ubx_cfg_rst_builder::build_warm_start();
    const auto cold = ubx_cfg_rst_builder::build_cold_start();

    const uint16_t m_hot  = payload_le16(hot, 0u);
    const uint16_t m_warm = payload_le16(warm, 0u);
    const uint16_t m_cold = payload_le16(cold, 0u);

    if (m_hot == m_warm || m_hot == m_cold || m_warm == m_cold)
    { fail(name, "navBbrMask values are not all distinct"); return; }
    pass(name);
}

// ─── test: class/ID constants match UBX_CLASS_CFG / expected value ────────────

static void test_cfg_rst_constants()
{
    const char* name = "cfg_rst_constants";

    // UBX_CLASS_CFG_RST must equal UBX_CLASS_CFG (same class byte 0x06)
    if (UBX_CLASS_CFG_RST != UBX_CLASS_CFG)
    { fail(name, "UBX_CLASS_CFG_RST != UBX_CLASS_CFG"); return; }
    if (UBX_CLASS_CFG_RST != 0x06u)
    { fail(name, "UBX_CLASS_CFG_RST != 0x06"); return; }
    if (UBX_ID_CFG_RST != 0x04u)
    { fail(name, "UBX_ID_CFG_RST != 0x04"); return; }
    if (CFG_RST_MODE_CTRL_SW != 0x01u)
    { fail(name, "CFG_RST_MODE_CTRL_SW != 0x01"); return; }
    if (NAV_BBR_HOT_START != 0x0000u)
    { fail(name, "NAV_BBR_HOT_START != 0x0000"); return; }
    if (NAV_BBR_WARM_START != 0x0001u)
    { fail(name, "NAV_BBR_WARM_START != 0x0001"); return; }
    if (NAV_BBR_COLD_START != 0xFFFFu)
    { fail(name, "NAV_BBR_COLD_START != 0xFFFF"); return; }
    pass(name);
}

// ─── main ────────────────────────────────────────────────────────────────────

int main()
{
    test_cfg_rst_constants();
    test_cfg_rst_hot_start();
    test_cfg_rst_warm_start();
    test_cfg_rst_cold_start();
    test_cfg_rst_variants_distinct();
    return 0;
}
