// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_decoders_tim_tp.cpp — Unit tests for tim_tp_decoder

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "decoders/tim_tp_decoder.h"
#include "messages/ubx_tim_tp.h"
#include "ubx_types.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name) { std::printf("[PASS] %s\n", name.c_str()); }
static void fail(const std::string& name, const std::string& r) { std::printf("[FAIL] %s: %s\n", name.c_str(), r.c_str()); }

// ─── helper ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> build_tim_tp_payload(uint32_t tow_ms,
                                                  uint32_t tow_sub_ms,
                                                  int32_t  q_err,
                                                  uint16_t week,
                                                  uint8_t  flags,
                                                  uint8_t  ref_info)
{
    std::vector<uint8_t> p;
    auto push_u32 = [&](uint32_t v) {
        p.push_back(static_cast<uint8_t>(v));
        p.push_back(static_cast<uint8_t>(v >> 8u));
        p.push_back(static_cast<uint8_t>(v >> 16u));
        p.push_back(static_cast<uint8_t>(v >> 24u));
    };

    push_u32(tow_ms);
    push_u32(tow_sub_ms);
    push_u32(static_cast<uint32_t>(q_err));
    p.push_back(static_cast<uint8_t>(week));
    p.push_back(static_cast<uint8_t>(week >> 8u));
    p.push_back(flags);
    p.push_back(ref_info);
    return p;
}

// ─── test: basic decode ───────────────────────────────────────────────────────

static void test_tim_tp_basic_decode()
{
    const char* name = "tim_tp_basic_decode";

    ubx_tim_tp received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<tim_tp_decoder>(
        new tim_tp_decoder([&](const ubx_tim_tp& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_TIM, UBX_ID_TIM_TP,
        build_tim_tp_payload(604800000u, 0xABCDEF01u, -250, 2350u, 0x01u, 0x00u)));

    if (!called)                             { fail(name, "callback not called"); return; }
    if (received.tow_ms != 604800000u)       { fail(name, "towMS mismatch"); return; }
    if (received.tow_sub_ms != 0xABCDEF01u)  { fail(name, "towSubMS mismatch"); return; }
    if (received.q_err != -250)              { fail(name, "qErr mismatch"); return; }
    if (received.week != 2350u)              { fail(name, "week mismatch"); return; }
    if (received.flags != 0x01u)             { fail(name, "flags mismatch"); return; }
    if (received.ref_info != 0x00u)          { fail(name, "ref_info mismatch"); return; }
    pass(name);
}

// ─── test: positive quantisation error ───────────────────────────────────────

static void test_tim_tp_positive_q_err()
{
    const char* name = "tim_tp_positive_q_err";

    ubx_tim_tp received{};
    bool called = false;

    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<tim_tp_decoder>(
        new tim_tp_decoder([&](const ubx_tim_tp& m) {
            received = m;
            called = true;
        })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_TIM, UBX_ID_TIM_TP,
        build_tim_tp_payload(0u, 0u, 500, 0u, 0x00u, 0x00u)));

    if (!called)            { fail(name, "callback not called"); return; }
    if (received.q_err != 500) { fail(name, "qErr mismatch"); return; }
    pass(name);
}

// ─── test: wrong payload length rejected ─────────────────────────────────────

static void test_tim_tp_wrong_length()
{
    const char* name = "tim_tp_wrong_length";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<tim_tp_decoder>(
        new tim_tp_decoder([&](const ubx_tim_tp&) { called = true; })));

    ubx_parser p(std::move(reg));
    p.feed(ubx_message_builder::make_frame(
        UBX_CLASS_TIM, UBX_ID_TIM_TP, std::vector<uint8_t>(8u, 0u)));

    if (!called)
        pass(name);
    else
        fail(name, "callback should not fire for wrong payload length");
}

// ─── test: fragmented feed ────────────────────────────────────────────────────

static void test_tim_tp_fragmented()
{
    const char* name = "tim_tp_fragmented";

    bool called = false;
    ubx_decoder_registry reg;
    reg.register_decoder(std::unique_ptr<tim_tp_decoder>(
        new tim_tp_decoder([&](const ubx_tim_tp&) { called = true; })));

    ubx_parser p(std::move(reg));
    const auto frame = ubx_message_builder::make_frame(
        UBX_CLASS_TIM, UBX_ID_TIM_TP,
        build_tim_tp_payload(0u, 0u, 0, 0u, 0u, 0u));

    const std::size_t half = frame.size() / 2u;
    p.feed(std::vector<uint8_t>(frame.begin(), frame.begin() + static_cast<long>(half)));
    if (called) { fail(name, "fired too early"); return; }
    p.feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(half), frame.end()));

    if (called)
        pass(name);
    else
        fail(name, "callback not fired");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_decoders_tim_tp ===\n");
    test_tim_tp_basic_decode();
    test_tim_tp_positive_q_err();
    test_tim_tp_wrong_length();
    test_tim_tp_fragmented();
    std::printf("Done.\n");
    return 0;
}
