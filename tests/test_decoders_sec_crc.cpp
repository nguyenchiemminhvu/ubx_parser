// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_sec_crc.cpp — Unit tests for sec_crc_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/sec_crc_decoder.h"
#include "messages/ubx_sec_crc.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper: build valid SEC-CRC payload ─────────────────────────────────────

static std::vector<uint8_t> build_sec_crc_payload(uint8_t  version,
                                                    uint8_t  crc_type,
                                                    uint32_t crc32)
{
    std::vector<uint8_t> p;
    p.push_back(version);
    p.push_back(crc_type);
    p.push_back(0x00u); p.push_back(0x00u); // reserved
    p.push_back(static_cast<uint8_t>(crc32));
    p.push_back(static_cast<uint8_t>(crc32 >> 8u));
    p.push_back(static_cast<uint8_t>(crc32 >> 16u));
    p.push_back(static_cast<uint8_t>(crc32 >> 24u));
    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_sec_crc_basic()
{
    const char* name = "sec_crc_basic";

    ubx_sec_crc received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<sec_crc_decoder>(
        new sec_crc_decoder([&](const ubx_sec_crc& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_SEC, UBX_ID_SEC_CRC,
                                            build_sec_crc_payload(0u, 1u, 0xDEADBEEFu)));

    if (!called)                         { fail(name, "callback not called"); return; }
    if (received.version != 0u)          { fail(name, "wrong version"); return; }
    if (received.crc_type != 1u)         { fail(name, "wrong crc_type"); return; }
    if (received.crc32 != 0xDEADBEEFu)  { fail(name, "wrong crc32"); return; }
    pass(name);
}

// ─── test: payload too small is rejected ─────────────────────────────────────

static void test_sec_crc_too_small()
{
    const char* name = "sec_crc_too_small";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<sec_crc_decoder>(
        new sec_crc_decoder([&](const ubx_sec_crc&) { called = true; })));

    ubx_parser p(std::move(reg));
    // Payload: 4 bytes only (less than required 8)
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_SEC, UBX_ID_SEC_CRC,
                                            { 0x00u, 0x01u, 0x00u, 0x00u }));
    if (!called)
        pass(name);
    else
        fail(name, "callback fired for undersized payload");
}

// ─── test: crc32 == 0 ────────────────────────────────────────────────────────

static void test_sec_crc_zero_crc()
{
    const char* name = "sec_crc_zero_crc";

    ubx_sec_crc received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<sec_crc_decoder>(
        new sec_crc_decoder([&](const ubx_sec_crc& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_SEC, UBX_ID_SEC_CRC,
                                            build_sec_crc_payload(0u, 0u, 0u)));
    if (!called)               { fail(name, "callback not called"); return; }
    if (received.crc32 != 0u) { fail(name, "wrong crc32"); return; }
    pass(name);
}

// ─── test: fragmented SEC-CRC across 3 feeds ─────────────────────────────────

static void test_sec_crc_fragmented()
{
    const char* name = "sec_crc_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<sec_crc_decoder>(
        new sec_crc_decoder([&](const ubx_sec_crc&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(UBX_CLASS_SEC, UBX_ID_SEC_CRC,
                                                        build_sec_crc_payload(0u, 1u, 0x12345678u));

    // Feed in 3 chunks
    const std::size_t third = frame.size() / 3u;
    for (int i = 0; i < 2; ++i)
    {
        const std::size_t start = static_cast<std::size_t>(i) * third;
        const std::size_t end   = start + third;
        p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(start),
                                     frame.begin() + static_cast<long>(end)));
        if (called) { fail(name, "fired too early"); return; }
    }
    // Feed remainder
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(2u * third), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired after all feeds");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_sec_crc ===\n");
    test_sec_crc_basic();
    test_sec_crc_too_small();
    test_sec_crc_zero_crc();
    test_sec_crc_fragmented();
    std::printf("Done.\n");
    return 0;
}
