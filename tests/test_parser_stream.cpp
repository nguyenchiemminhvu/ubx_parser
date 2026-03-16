// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_parser_stream.cpp
//
// Tests for the ubx_parser feed() stream parsing behaviour:
//   - single complete frame
//   - fragmented frame across multiple feeds
//   - multiple frames in one feed
//   - invalid checksum triggers error callback
//   - noise / resync recovery
//   - unsupported message routed to raw callback
//   - incomplete frame buffering (parser waits, then completes on subsequent feed)

#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "ubx_protocol.h"
#include "ubx_types.h"
#include "ubx_errors.h"
#include "messages/ubx_raw_message.h"

#include <cstdio>
#include <string>
#include <memory>

using namespace ubx::parser;

// ─── helpers ──────────────────────────────────────────────────────────────────

static void pass(const std::string& name)
{
    std::printf("[PASS] %s\n", name.c_str());
}

static void fail(const std::string& name, const std::string& reason)
{
    std::printf("[FAIL] %s: %s\n", name.c_str(), reason.c_str());
}

/// Build a simple known-class/id frame with given payload.
static std::vector<uint8_t> build_frame(uint8_t cls, uint8_t id,
                                         const std::vector<uint8_t>& payload)
{
    return ubx_message_builder::make_frame(cls, id, payload);
}

/// Create a fresh parser with a raw callback counter.
struct test_parser
{
    int raw_count    = 0;
    int error_count  = 0;

    ubx_decoder_registry reg;
    ubx_parser* p = nullptr;

    test_parser()
    {
        p = new ubx_parser(std::move(reg));
        p->set_raw_message_callback([this](const ubx_raw_message&) { ++raw_count; });
        p->set_error_callback([this](const parse_error_info&) { ++error_count; });
    }

    ~test_parser() { delete p; }
};

// ─── test: single complete frame routed to raw callback ───────────────────────

static void test_single_frame()
{
    const char* name = "single_frame";
    test_parser tp;

    const auto frame = build_frame(0x0Au, 0x04u, { 0x11u, 0x22u });
    tp.p->feed(frame);

    if (tp.raw_count == 1 && tp.error_count == 0)
        pass(name);
    else
        fail(name, "expected 1 raw callback");
}

// ─── test: fragmented frame (feed 1 byte at a time) ──────────────────────────

static void test_fragmented_frame()
{
    const char* name = "fragmented_frame";
    test_parser tp;

    const auto frame = build_frame(0x0Au, 0x04u, { 0x01u, 0x02u, 0x03u });

    // Feed one byte per call
    for (uint8_t byte : frame)
        tp.p->feed({ byte });

    if (tp.raw_count == 1 && tp.error_count == 0)
        pass(name);
    else
        fail(name, "expected 1 raw callback after byte-by-byte feed");
}

// ─── test: multiple frames in one chunk ───────────────────────────────────────

static void test_multiple_frames_one_chunk()
{
    const char* name = "multiple_frames_one_chunk";
    test_parser tp;

    const auto f1 = build_frame(0x0Au, 0x04u, { 0x01u });
    const auto f2 = build_frame(0x0Au, 0x04u, { 0x02u });
    const auto f3 = build_frame(0x0Au, 0x04u, { 0x03u });

    std::vector<uint8_t> chunk;
    chunk.insert(chunk.end(), f1.begin(), f1.end());
    chunk.insert(chunk.end(), f2.begin(), f2.end());
    chunk.insert(chunk.end(), f3.begin(), f3.end());

    tp.p->feed(chunk);

    if (tp.raw_count == 3 && tp.error_count == 0)
        pass(name);
    else
        fail(name, "expected 3 raw callbacks");
}

// ─── test: invalid checksum triggers error callback ───────────────────────────

static void test_invalid_checksum()
{
    const char* name = "invalid_checksum";
    test_parser tp;

    auto frame = build_frame(0x01u, 0x07u, std::vector<uint8_t>(8u, 0u));
    frame.back() ^= 0xFFu; // corrupt CK_B

    tp.p->feed(frame);

    if (tp.raw_count == 0 && tp.error_count == 1)
        pass(name);
    else
        fail(name, "expected 0 raw, 1 error");
}

// ─── test: noise bytes before valid frame (resync) ────────────────────────────

static void test_noise_resync()
{
    const char* name = "noise_resync";
    test_parser tp;

    const auto frame = build_frame(0x0Au, 0x04u, { 0xAAu });

    // Prepend random noise
    std::vector<uint8_t> data = { 0xFFu, 0x00u, 0x11u, 0x22u, 0x33u };
    data.insert(data.end(), frame.begin(), frame.end());

    tp.p->feed(data);

    if (tp.raw_count == 1 && tp.error_count == 0)
        pass(name);
    else
        fail(name, "expected 1 raw callback after resync");
}

// ─── test: incomplete frame waits, completes on next feed ────────────────────

static void test_incomplete_frame_buffering()
{
    const char* name = "incomplete_frame_buffering";
    test_parser tp;

    const auto frame = build_frame(0x0Au, 0x04u, { 0x01u, 0x02u });

    // Feed only the first half
    const std::size_t half = frame.size() / 2u;
    tp.p->feed(std::vector<uint8_t>(frame.begin(), frame.begin() + static_cast<long>(half)));

    // No callback yet
    if (tp.raw_count != 0)
    {
        fail(name, "callback fired before complete frame");
        return;
    }

    // Feed the rest
    tp.p->feed(std::vector<uint8_t>(frame.begin() + static_cast<long>(half), frame.end()));

    if (tp.raw_count == 1 && tp.error_count == 0)
        pass(name);
    else
        fail(name, "expected callback after second feed completes frame");
}

// ─── test: noise + valid + noise + valid ─────────────────────────────────────

static void test_noise_interleaved_with_frames()
{
    const char* name = "noise_interleaved_with_frames";
    test_parser tp;

    const auto f1 = build_frame(0x0Au, 0x04u, { 0x01u });
    const auto f2 = build_frame(0x0Au, 0x04u, { 0x02u });

    std::vector<uint8_t> stream = { 0xAAu, 0xBBu };
    stream.insert(stream.end(), f1.begin(), f1.end());
    stream.push_back(0xCCu);
    stream.insert(stream.end(), f2.begin(), f2.end());
    stream.push_back(0xDDu);

    tp.p->feed(stream);

    if (tp.raw_count == 2 && tp.error_count == 0)
        pass(name);
    else
        fail(name, "expected 2 raw callbacks with noise");
}

// ─── test: frame counters ────────────────────────────────────────────────────

static void test_frame_counters()
{
    const char* name = "frame_counters";
    test_parser tp;

    const auto good  = build_frame(0x0Au, 0x04u, { 0x01u });
    auto bad = build_frame(0x0Au, 0x04u, { 0x01u });
    bad.back() ^= 0x01u; // corrupt

    tp.p->feed(good);
    tp.p->feed(good);
    tp.p->feed(bad);

    if (tp.p->frames_decoded() == 2u && tp.p->frames_discarded() == 1u)
        pass(name);
    else
        fail(name, "counters incorrect");
}

// ─── test: parser reset clears counters ──────────────────────────────────────

static void test_parser_reset()
{
    const char* name = "parser_reset";
    test_parser tp;

    const auto frame = build_frame(0x0Au, 0x04u, { 0x01u });
    tp.p->feed(frame);
    tp.p->reset();

    if (tp.p->frames_decoded() == 0u && tp.p->frames_discarded() == 0u)
        pass(name);
    else
        fail(name, "counters not reset");
}

// ─── test: payload exactly at safe limit is accepted ─────────────────────────

static void test_payload_at_safe_limit_accepted()
{
    const char* name = "payload_at_safe_limit_accepted";
    test_parser tp;

    // Use the default safe limit as the payload size — must be accepted.
    const uint16_t safe_limit = static_cast<uint16_t>(tp.p->max_payload_length());
    const std::vector<uint8_t> payload(safe_limit, 0xAAu);
    const auto frame = build_frame(0x01u, 0x07u, payload);
    tp.p->feed(frame);

    if (tp.raw_count == 1 && tp.error_count == 0)
        pass(name);
    else
        fail(name, "expected frame at the limit to be accepted");
}

// ─── test: payload one byte over safe limit is rejected ──────────────────────

static void test_payload_over_safe_limit_rejected()
{
    const char* name = "payload_over_safe_limit_rejected";
    test_parser tp;

    const uint16_t over = static_cast<uint16_t>(tp.p->max_payload_length() + 1u);

    // Build a raw stream with the oversized length field but without a real
    // payload — the error must fire as soon as the length bytes are decoded.
    // Inject only the 6-byte header (sync1, sync2, class, id, len_lo, len_hi).
    std::vector<uint8_t> header_only = {
        0xB5u, 0x62u,
        0x01u, 0x07u,
        static_cast<uint8_t>(over & 0xFFu),
        static_cast<uint8_t>((over >> 8u) & 0xFFu)
    };
    tp.p->feed(header_only);

    if (tp.raw_count == 0 && tp.error_count == 1)
        pass(name);
    else
        fail(name, "expected payload_too_large error when length exceeds configured max");
}

// ─── test: custom max_payload_length setter is honoured ──────────────────────

static void test_custom_max_payload_length()
{
    const char* name = "custom_max_payload_length";
    test_parser tp;

    // Lower the limit to 16 bytes.
    tp.p->set_max_payload_length(16u);

    // A 16-byte payload must be accepted.
    const std::vector<uint8_t> ok_payload(16u, 0x11u);
    tp.p->feed(build_frame(0x0Au, 0x04u, ok_payload));
    if (tp.raw_count != 1 || tp.error_count != 0)
    {
        fail(name, "16-byte frame (at limit) must be accepted");
        return;
    }

    // A 17-byte payload must be rejected.
    const std::vector<uint8_t> over_payload(17u, 0x11u);
    std::vector<uint8_t> header_only = {
        0xB5u, 0x62u,
        0x0Au, 0x04u,
        17u, 0u
    };
    tp.p->feed(header_only);
    if (tp.raw_count == 1 && tp.error_count == 1)
        pass(name);
    else
        fail(name, "17-byte frame (over limit) must be rejected");
}

// ─── test: memory is reclaimed after oversized-payload discard ───────────────
//
// We cannot inspect heap capacity directly in portable C++, but we can verify
// that the parser remains fully functional (correct counters, correct callbacks)
// after repeated oversized-frame attacks — confirming discard_frame does not
// leave the context in an inconsistent state.

static void test_repeated_oversize_attack_then_valid()
{
    const char* name = "repeated_oversize_attack_then_valid";
    test_parser tp;

    // Send 10 fake oversized headers.
    const uint16_t over = static_cast<uint16_t>(tp.p->max_payload_length() + 1u);
    const std::vector<uint8_t> attack_header = {
        0xB5u, 0x62u,
        0x01u, 0x07u,
        static_cast<uint8_t>(over & 0xFFu),
        static_cast<uint8_t>((over >> 8u) & 0xFFu)
    };

    for (int i = 0; i < 10; ++i)
        tp.p->feed(attack_header);

    if (tp.error_count != 10)
    {
        fail(name, "expected 10 error callbacks for 10 attack headers");
        return;
    }

    // After the attack, a valid frame must still be dispatched correctly.
    tp.p->feed(build_frame(0x0Au, 0x04u, { 0x01u }));

    if (tp.raw_count == 1 && tp.p->frames_decoded() == 1u)
        pass(name);
    else
        fail(name, "parser not functional after repeated oversize attacks");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_parser_stream ===\n");
    test_single_frame();
    test_fragmented_frame();
    test_multiple_frames_one_chunk();
    test_invalid_checksum();
    test_noise_resync();
    test_incomplete_frame_buffering();
    test_noise_interleaved_with_frames();
    test_frame_counters();
    test_parser_reset();
    test_payload_at_safe_limit_accepted();
    test_payload_over_safe_limit_rejected();
    test_custom_max_payload_length();
    test_repeated_oversize_attack_then_valid();
    std::printf("Done.\n");
    return 0;
}
