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
// ubx_parser.h
//
// Top-level UBX stream parser — the primary public API of this library.
//
// Responsibilities:
//   1. Accept incremental byte chunks via feed().
//   2. Run a byte-level state machine to frame UBX messages.
//   3. Validate each frame's Fletcher-8 checksum.
//   4. Dispatch valid frames to ubx_dispatcher, which routes them to the
//      appropriate decoder or raw-message callback.
//   5. Dispatch parse errors via the error callback.
//
// Real-device behaviour:
//   The u-blox GNSS chip may split a single UBX message across multiple
//   serial read() calls.  ubx_parser preserves all in-progress state between
//   feed() calls and waits for more bytes when an incomplete frame is detected.
//   Spurious bytes (NMEA sentences, noise) are skipped during resynchronisation.
//
// Thread-safety:
//   ubx_parser is NOT thread-safe.  All feed() calls must originate from the
//   same thread.  If multiple threads produce data, the caller must serialise
//   access (e.g. with a mutex or message queue).
//
// Typical usage:
//
//   ubx::parser::ubx_decoder_registry reg;
//   reg.register_decoder(std::unique_ptr<ubx::parser::nav_pvt_decoder>(
//       new ubx::parser::nav_pvt_decoder([](const ubx::parser::ubx_nav_pvt& m) {
//           // handle PVT
//       })));
//
//   ubx::parser::ubx_parser parser(std::move(reg));
//   parser.set_error_callback([](const ubx::parser::parse_error_info& e) { ... });
//
//   // Called from UART receive thread/callback:
//   parser.feed(chunk);

#pragma once

#include "ubx_types.h"
#include "ubx_errors.h"
#include "ubx_decoder_registry.h"
#include "ubx_dispatcher.h"
#include "messages/ubx_raw_message.h"

#include <cstdint>
#include <vector>

namespace ubx
{
namespace parser
{

// Forward-declare internal state type to keep this header lean.
namespace internal { struct parse_context; }

// ─── ubx_parser ───────────────────────────────────────────────────────────────

class ubx_parser
{
public:
    /// Construct with an already-populated decoder registry.
    /// The parser takes ownership of the registry.
    ///
    /// @param registry  Decoder registry.  All decoders should be registered
    ///                  before calling feed() for the first time.
    explicit ubx_parser(ubx_decoder_registry registry);

    ~ubx_parser();

    // Non-copyable (owns parse state and registry)
    ubx_parser(const ubx_parser&)            = delete;
    ubx_parser& operator=(const ubx_parser&) = delete;

    // ── Callback registration ─────────────────────────────────────────────────

    /// Set the callback for valid frames that have no registered decoder.
    /// Call before the first feed().
    void set_raw_message_callback(raw_message_callback_t cb);

    /// Set the callback for parse errors.
    /// Call before the first feed().
    void set_error_callback(parse_error_callback cb);

    // ── Data ingestion ────────────────────────────────────────────────────────

    /// Feed a chunk of bytes received from the serial/UART layer.
    ///
    /// The parser processes bytes one at a time through the UBX framing state
    /// machine.  The method handles:
    ///   - multiple complete frames in one chunk,
    ///   - frames split across multiple feed() calls,
    ///   - noise / NMEA data between UBX frames (resynchronises on next 0xB5 0x62).
    ///
    /// Callbacks are fired synchronously from within this call.
    ///
    /// @param data  Byte chunk received from the transport layer.
    void feed(const std::vector<uint8_t>& data);

    // ── Introspection ─────────────────────────────────────────────────────────

    /// @returns the number of successfully decoded frames since construction.
    uint64_t frames_decoded() const;

    /// @returns the number of frames discarded (checksum failures, etc.) since
    ///          construction.
    uint64_t frames_discarded() const;

    /// Reset the parser state machine and all counters.
    /// Does NOT remove registered decoders or callbacks.
    void reset();

    // ── Payload-size protection ───────────────────────────────────────────────

    /// Set the maximum payload length the parser will accept.
    ///
    /// Frames claiming a payload larger than @p max_len are immediately
    /// rejected with a payload_too_large error and the parser re-synchronises.
    /// This prevents unbounded buffer growth caused by malformed or malicious
    /// input while still accepting every legitimate u-blox message.
    ///
    /// Default value: UBX_SAFE_MAX_PAYLOAD_LEN (4096 bytes).
    /// Maximum value: UBX_MAX_PAYLOAD_LEN (65535 bytes).
    ///
    /// @param max_len  Maximum accepted payload length in bytes.
    ///                 Must be ≥ 1; values of 0 are silently raised to 1.
    /// Call before the first feed().
    void set_max_payload_length(uint16_t max_len);

    /// Return the currently configured maximum payload length.
    uint16_t max_payload_length() const;

private:
    // Note: max_payload_len_ is intentionally listed first in the private
    // section so it is clearly visible as a tuneable policy parameter.
    /// Process a single byte through the state machine.
    void process_byte(uint8_t byte);

    /// Called when a complete frame has been accumulated and validated.
    void on_frame_complete();

    /// Called when the current frame must be discarded (checksum error, etc.).
    void discard_frame(parser_error_code code, const std::string& detail);

    ubx_decoder_registry             registry_;
    ubx_dispatcher                   dispatcher_;
    internal::parse_context*         ctx_;       // heap-allocated to keep header lean

    uint64_t frames_decoded_;
    uint64_t frames_discarded_;
    uint16_t max_payload_len_;  ///< Reject frames claiming a payload larger than this.
};

} // namespace parser
} // namespace ubx
