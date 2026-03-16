// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_mon_ver.cpp — Unit tests for mon_ver_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/mon_ver_decoder.h"
#include "messages/ubx_mon_ver.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>
#include <cstring>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

/// Build a MON-VER payload with the given strings.
/// sw must fit in 30 bytes, hw in 10 bytes, each ext in 30 bytes.
static std::vector<uint8_t> build_mon_ver_payload(const std::string& sw,
                                                   const std::string& hw,
                                                   const std::vector<std::string>& exts = {})
{
    std::vector<uint8_t> p;

    auto push_fixed = [&](const std::string& s, std::size_t field_len)
    {
        for (std::size_t i = 0u; i < field_len; ++i)
        {
            p.push_back((i < s.size()) ? static_cast<uint8_t>(s[i]) : 0u);
        }
    };

    push_fixed(sw, UBX_MON_VER_SW_LEN);
    push_fixed(hw, UBX_MON_VER_HW_LEN);
    for (const auto& ext : exts)
        push_fixed(ext, UBX_MON_VER_EXT_LEN);

    return p;
}

// ─── test: basic decode, no extensions ───────────────────────────────────────

static void test_mon_ver_basic_decode()
{
    const char* name = "mon_ver_basic_decode";

    ubx_mon_ver received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<mon_ver_decoder>(
        new mon_ver_decoder([&](const ubx_mon_ver& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_MON, UBX_ID_MON_VER,
        build_mon_ver_payload("SPG 5.10 (7b202e)", "00190000")));

    if (!called)                                          { fail(name, "callback not called"); return; }
    if (received.sw_version != "SPG 5.10 (7b202e)")      { fail(name, "sw_version mismatch"); return; }
    if (received.hw_version != "00190000")                { fail(name, "hw_version mismatch"); return; }
    if (!received.extensions.empty())                     { fail(name, "extensions should be empty"); return; }
    pass(name);
}

// ─── test: decode with extensions ────────────────────────────────────────────

static void test_mon_ver_with_extensions()
{
    const char* name = "mon_ver_with_extensions";

    ubx_mon_ver received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<mon_ver_decoder>(
        new mon_ver_decoder([&](const ubx_mon_ver& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    const std::vector<std::string> exts = { "GPS;SBAS;GAL;BDS;QZSS", "FWVER=HPG 1.30" };
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_MON, UBX_ID_MON_VER,
        build_mon_ver_payload("HPG 1.30 (108efb)", "00190000", exts)));

    if (!called)                                          { fail(name, "callback not called"); return; }
    if (received.sw_version != "HPG 1.30 (108efb)")      { fail(name, "sw_version mismatch"); return; }
    if (received.extensions.size() != 2u)                 { fail(name, "wrong number of extensions"); return; }
    if (received.extensions[0] != "GPS;SBAS;GAL;BDS;QZSS") { fail(name, "ext[0] mismatch"); return; }
    if (received.extensions[1] != "FWVER=HPG 1.30")      { fail(name, "ext[1] mismatch"); return; }
    pass(name);
}

// ─── test: too-short payload rejected ────────────────────────────────────────

static void test_mon_ver_too_short()
{
    const char* name = "mon_ver_too_short";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<mon_ver_decoder>(
        new mon_ver_decoder([&](const ubx_mon_ver&) { called = true; })));

    ubx_parser p(std::move(reg));
    // 35 bytes: less than minimum 40
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_MON, UBX_ID_MON_VER, std::vector<uint8_t>(35u, 0u)));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for too-short payload");
}

// ─── test: extra bytes not a multiple of 30 rejected ─────────────────────────

static void test_mon_ver_bad_extension_length()
{
    const char* name = "mon_ver_bad_extension_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<mon_ver_decoder>(
        new mon_ver_decoder([&](const ubx_mon_ver&) { called = true; })));

    ubx_parser p(std::move(reg));
    // 40 + 15 = 55 bytes: not a multiple-of-30 extension
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_MON, UBX_ID_MON_VER, std::vector<uint8_t>(55u, 0u)));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for bad extension length");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_mon_ver_fragmented()
{
    const char* name = "mon_ver_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<mon_ver_decoder>(
        new mon_ver_decoder([&](const ubx_mon_ver&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_MON, UBX_ID_MON_VER,
        build_mon_ver_payload("SPG 5.10", "00190000"));

    const std::size_t third = frame.size() / 3u;
    for (int i = 0; i < 2; ++i)
    {
        const std::size_t start = static_cast<std::size_t>(i) * third;
        p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(start),
                                     frame.begin() + static_cast<long>(start + third)));
        if (called) { fail(name, "fired too early"); return; }
    }
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(2u * third), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_mon_ver ===\n");
    test_mon_ver_basic_decode();
    test_mon_ver_with_extensions();
    test_mon_ver_too_short();
    test_mon_ver_bad_extension_length();
    test_mon_ver_fragmented();
    std::printf("Done.\n");
    return 0;
}
