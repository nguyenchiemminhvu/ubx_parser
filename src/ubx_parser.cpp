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

#include "ubx_parser.h"
#include "internal/parser_state.h"
#include "internal/checksum.h"

#include <cstring>
#include <string>
#include <utility>

namespace ubx
{
namespace parser
{

// ─── construction / destruction ───────────────────────────────────────────────

ubx_parser::ubx_parser(ubx_decoder_registry registry)
    : registry_(std::move(registry))
    , dispatcher_(registry_)
    , ctx_(new internal::parse_context())
    , frames_decoded_(0u)
    , frames_discarded_(0u)
    , max_payload_len_(UBX_SAFE_MAX_PAYLOAD_LEN)
{}

ubx_parser::~ubx_parser()
{
    delete ctx_;
}

// ─── callback registration ────────────────────────────────────────────────────

void ubx_parser::set_raw_message_callback(raw_message_callback_t cb)
{
    dispatcher_.set_raw_message_callback(std::move(cb));
}

void ubx_parser::set_error_callback(parse_error_callback cb)
{
    dispatcher_.set_error_callback(std::move(cb));
}

// ─── counters / reset ─────────────────────────────────────────────────────────

uint64_t ubx_parser::frames_decoded()   const { return frames_decoded_;   }
uint64_t ubx_parser::frames_discarded() const { return frames_discarded_; }

void ubx_parser::set_max_payload_length(uint16_t max_len)
{
    // Silently raise 0 to 1 so the limit is never trivially zero.
    max_payload_len_ = (max_len == 0u) ? 1u : max_len;
}

uint16_t ubx_parser::max_payload_length() const
{
    return max_payload_len_;
}

void ubx_parser::reset()
{
    ctx_->reset();
    frames_decoded_   = 0u;
    frames_discarded_ = 0u;
}

// ─── data ingestion ───────────────────────────────────────────────────────────

void ubx_parser::feed(const std::vector<uint8_t>& data)
{
    for (const uint8_t byte : data)
        process_byte(byte);
}

// ─── state machine ────────────────────────────────────────────────────────────

void ubx_parser::process_byte(uint8_t byte)
{
    using internal::parse_state;

    switch (ctx_->state)
    {
    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_sync_1:
        // Scan until we see 0xB5.
        if (byte == UBX_SYNC_CHAR_1)
            ctx_->state = parse_state::wait_sync_2;
        // Any other byte: discard silently (noise / NMEA).
        break;

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_sync_2:
        if (byte == UBX_SYNC_CHAR_2)
        {
            ctx_->state = parse_state::wait_class;
        }
        else if (byte == UBX_SYNC_CHAR_1)
        {
            // Stay in wait_sync_2; another 0xB5 found while expecting 0x62.
            // e.g. 0xB5 0xB5 0x62 ...
        }
        else
        {
            // Not a valid second sync byte — return to scanning.
            ctx_->state = parse_state::wait_sync_1;
        }
        break;

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_class:
        ctx_->header.msg_class = byte;
        ctx_->state = parse_state::wait_id;
        break;

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_id:
        ctx_->header.msg_id = byte;
        ctx_->state = parse_state::wait_len_lo;
        break;

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_len_lo:
        ctx_->len_lo = byte;
        ctx_->state = parse_state::wait_len_hi;
        break;

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_len_hi:
    {
        ctx_->len_hi = byte;
        const uint16_t len =
            static_cast<uint16_t>(ctx_->len_lo)
            | (static_cast<uint16_t>(ctx_->len_hi) << 8u);

        ctx_->header.payload_length = len;

        // Reject payloads that exceed the configured safe ceiling.
        //
        // Historical note: the original check compared a uint16_t against
        // UBX_MAX_PAYLOAD_LEN (0xFFFF), which is a tautologically false
        // condition — a uint16_t can never exceed 0xFFFF.  That dead check
        // meant every frame claiming up to 65535 bytes was accepted, causing
        // an immediate 64 KiB heap reservation (payload_buffer.reserve) and
        // making the parser vulnerable to DoS from malformed frames.
        //
        // max_payload_len_ defaults to UBX_SAFE_MAX_PAYLOAD_LEN (4096) and is
        // independently configurable per ubx_parser instance.
        if (len > max_payload_len_)
        {
            discard_frame(parser_error_code::payload_too_large,
                          "payload length exceeds configured maximum");
            break;
        }

        if (len == 0u)
        {
            // Zero-length payload: skip accumulation, wait for checksum directly.
            ctx_->state = parse_state::wait_ck_a;
        }
        else
        {
            ctx_->payload_buffer.clear();
            ctx_->payload_buffer.reserve(static_cast<std::size_t>(len));
            ctx_->state = parse_state::accumulate_payload;
        }
        break;
    }

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::accumulate_payload:
        ctx_->payload_buffer.push_back(byte);

        if (ctx_->payload_buffer.size() ==
            static_cast<std::size_t>(ctx_->header.payload_length))
        {
            ctx_->state = parse_state::wait_ck_a;
        }
        break;

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_ck_a:
        ctx_->received_ck_a = byte;
        ctx_->state = parse_state::wait_ck_b;
        break;

    // ─────────────────────────────────────────────────────────────────────────
    case parse_state::wait_ck_b:
    {
        ctx_->received_ck_b = byte;

        // Verify checksum.
        const bool ok = internal::verify_checksum(
            ctx_->header.msg_class,
            ctx_->header.msg_id,
            ctx_->len_lo,
            ctx_->len_hi,
            ctx_->payload_buffer.data(),
            ctx_->header.payload_length,
            ctx_->received_ck_a,
            ctx_->received_ck_b);

        if (!ok)
        {
            discard_frame(parser_error_code::checksum_mismatch,
                          "CK_A/CK_B checksum mismatch");
        }
        else
        {
            on_frame_complete();
        }
        break;
    }

    } // switch
}

// ─── frame completion ─────────────────────────────────────────────────────────

void ubx_parser::on_frame_complete()
{
    ubx_frame frame;
    frame.header       = ctx_->header;
    frame.payload      = std::move(ctx_->payload_buffer);
    frame.ck_a         = ctx_->received_ck_a;
    frame.ck_b         = ctx_->received_ck_b;

    ctx_->reset();
    ++frames_decoded_;

    dispatcher_.dispatch(frame);
}

// ─── frame discard / error ────────────────────────────────────────────────────

void ubx_parser::discard_frame(parser_error_code code, const std::string& detail)
{
    parse_error_info info;
    info.code        = code;
    info.msg_class   = ctx_->header.msg_class;
    info.msg_id      = ctx_->header.msg_id;
    info.description = detail;

    ++frames_discarded_;
    ctx_->reset();

    // Release any over-allocated heap capacity that may have been reserved for
    // a large (possibly malicious) payload.  reset() only clears the vector;
    // shrink_to_fit() requests the allocator to actually free the memory.
    // This prevents a single fake oversized frame from locking in a large
    // persistent allocation for the lifetime of the parser.
    ctx_->payload_buffer.shrink_to_fit();

    dispatcher_.dispatch_error(info);
}

} // namespace parser
} // namespace ubx
