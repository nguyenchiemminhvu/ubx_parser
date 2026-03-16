// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_inf.cpp — Unit tests for inf_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/inf_decoder.h"
#include "messages/ubx_inf.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> make_text_payload(const std::string& text)
{
    return std::vector<uint8_t>(text.begin(), text.end());
}

// ─── test: INF-ERROR ─────────────────────────────────────────────────────────

static void test_inf_error_decode()
{
    const char* name = "inf_error_decode";

    ubx_inf received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_ERROR, [&](const ubx_inf& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_INF, UBX_ID_INF_ERROR, make_text_payload("ROM core")));

    if (!called)                                 { fail(name, "callback not called"); return; }
    if (received.subtype != inf_subtype::error)  { fail(name, "subtype mismatch"); return; }
    if (received.text != "ROM core")             { fail(name, "text mismatch"); return; }
    pass(name);
}

// ─── test: INF-WARNING ───────────────────────────────────────────────────────

static void test_inf_warning_decode()
{
    const char* name = "inf_warning_decode";

    ubx_inf received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_WARNING, [&](const ubx_inf& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_INF, UBX_ID_INF_WARNING, make_text_payload("time accuracy low")));

    if (!called)                                   { fail(name, "callback not called"); return; }
    if (received.subtype != inf_subtype::warning)  { fail(name, "subtype mismatch"); return; }
    if (received.text != "time accuracy low")      { fail(name, "text mismatch"); return; }
    pass(name);
}

// ─── test: INF-NOTICE ────────────────────────────────────────────────────────

static void test_inf_notice_decode()
{
    const char* name = "inf_notice_decode";

    ubx_inf received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_NOTICE, [&](const ubx_inf& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_INF, UBX_ID_INF_NOTICE, make_text_payload("GNSS fix acquired")));

    if (!called)                                  { fail(name, "callback not called"); return; }
    if (received.subtype != inf_subtype::notice)  { fail(name, "subtype mismatch"); return; }
    if (received.text != "GNSS fix acquired")     { fail(name, "text mismatch"); return; }
    pass(name);
}

// ─── test: INF-DEBUG ─────────────────────────────────────────────────────────

static void test_inf_debug_decode()
{
    const char* name = "inf_debug_decode";

    ubx_inf received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_DEBUG, [&](const ubx_inf& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_INF, UBX_ID_INF_DEBUG, make_text_payload("satInfo")));

    if (!called)                                 { fail(name, "callback not called"); return; }
    if (received.subtype != inf_subtype::debug)  { fail(name, "subtype mismatch"); return; }
    if (received.text != "satInfo")              { fail(name, "text mismatch"); return; }
    pass(name);
}

// ─── test: empty payload (zero-length text) ───────────────────────────────────

static void test_inf_empty_payload()
{
    const char* name = "inf_empty_payload";

    ubx_inf received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_NOTICE, [&](const ubx_inf& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    // Zero-length payload is valid — empty text string.
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_INF, UBX_ID_INF_NOTICE, {}));

    if (!called)             { fail(name, "callback not called for empty payload"); return; }
    if (!received.text.empty()) { fail(name, "text should be empty"); return; }
    pass(name);
}

// ─── test: null-terminated payload handling ───────────────────────────────────

static void test_inf_null_terminated()
{
    const char* name = "inf_null_terminated";

    ubx_inf received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_NOTICE, [&](const ubx_inf& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    // Payload: "hello" + null + trailing garbage
    std::vector<uint8_t> payload = { 'h', 'e', 'l', 'l', 'o', 0x00u, 0x01u, 0x02u };
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_INF, UBX_ID_INF_NOTICE, payload));

    if (!called)               { fail(name, "callback not called"); return; }
    if (received.text != "hello") { fail(name, "text should stop at null byte"); return; }
    pass(name);
}

// ─── test: multiple INF subtypes registered simultaneously ───────────────────

static void test_inf_multiple_subtypes()
{
    const char* name = "inf_multiple_subtypes";

    int error_count   = 0;
    int warning_count = 0;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_ERROR,   [&](const ubx_inf&) { ++error_count; })));
    reg.register_decoder(std::unique_ptr<inf_decoder>(
        new inf_decoder(UBX_ID_INF_WARNING, [&](const ubx_inf&) { ++warning_count; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_INF, UBX_ID_INF_ERROR,   make_text_payload("e1")));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_INF, UBX_ID_INF_ERROR,   make_text_payload("e2")));
    p.feed(ubx_message_builder::make_frame(UBX_CLASS_INF, UBX_ID_INF_WARNING, make_text_payload("w1")));

    if (error_count != 2)   { fail(name, "expected 2 ERROR callbacks"); return; }
    if (warning_count != 1) { fail(name, "expected 1 WARNING callback"); return; }
    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_inf ===\n");
    test_inf_error_decode();
    test_inf_warning_decode();
    test_inf_notice_decode();
    test_inf_debug_decode();
    test_inf_empty_payload();
    test_inf_null_terminated();
    test_inf_multiple_subtypes();
    std::printf("Done.\n");
    return 0;
}
