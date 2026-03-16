// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// internal/parser_state.h
//
// State machine state enumeration and parse context used by ubx_parser
// internally.
//
// The UBX framing state machine walks through these states sequentially for
// each incoming byte.  On completion or error it resets to WAIT_SYNC_1.

#pragma once

#include "ubx_types.h"
#include <cstdint>
#include <vector>

namespace ubx
{
namespace parser
{
namespace internal
{

// ─── parse_state ──────────────────────────────────────────────────────────────
//
// Enumeration of the parser state machine states.

enum class parse_state : uint8_t
{
    /// Waiting for the first UBX sync byte (0xB5).
    wait_sync_1 = 0u,

    /// Received 0xB5; waiting for the second sync byte (0x62).
    wait_sync_2,

    /// Waiting for the message class byte.
    wait_class,

    /// Waiting for the message ID byte.
    wait_id,

    /// Waiting for the low byte of the 16-bit payload length.
    wait_len_lo,

    /// Waiting for the high byte of the 16-bit payload length.
    wait_len_hi,

    /// Accumulating payload bytes.  Transitions to wait_ck_a once
    /// payload_buffer.size() == expected_payload_len.
    accumulate_payload,

    /// Waiting for the first checksum byte (CK_A).
    wait_ck_a,

    /// Waiting for the second checksum byte (CK_B).
    wait_ck_b,
};

// ─── parse_context ────────────────────────────────────────────────────────────
//
// All mutable state held by the parser between feed() calls.
// Reset to defaults on parser construction or after a completed/discarded frame.

struct parse_context
{
    /// Current state machine state.
    parse_state state;

    /// Partially decoded message header; populated incrementally.
    ubx_message_header header;

    /// Buffer accumulating the raw payload bytes.
    /// Reserved to expected_payload_len when the length field is decoded to
    /// avoid repeated re-allocations on the hot path.
    std::vector<uint8_t> payload_buffer;

    /// Received first checksum byte.
    uint8_t received_ck_a;

    /// Received second checksum byte.
    uint8_t received_ck_b;

    /// Convenience: cached low byte of payload length for checksum verification.
    uint8_t len_lo;

    /// Convenience: cached high byte of payload length for checksum verification.
    uint8_t len_hi;

    /// Reset all fields to their initial (idle) values.
    void reset()
    {
        state           = parse_state::wait_sync_1;
        header          = {};
        payload_buffer.clear();
        received_ck_a   = 0u;
        received_ck_b   = 0u;
        len_lo          = 0u;
        len_hi          = 0u;
    }

    parse_context()
    {
        reset();
    }
};

} // namespace internal
} // namespace parser
} // namespace ubx
