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
// ubx_errors.h
//
// Error model for the ubx_parser library.
//
// Design:
//   - parser_error_code is a strongly-typed enum class (C++11+).
//   - parse_error_info bundles the code with contextual information for the
//     error callback, making diagnostic output straightforward.
//   - A free function error_message() provides a human-readable description
//     of each code for logging/debugging.

#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── parser_error_code ────────────────────────────────────────────────────────
//
// Enumeration of all error conditions the parser can encounter.
// Values are explicitly numbered to remain stable across refactors.

enum class parser_error_code : uint8_t
{
    /// No error. Included for completeness; never dispatched via error callback.
    none = 0u,

    /// CK_A or CK_B checksum mismatch. The frame header and payload were
    /// received, but the trailing checksum bytes did not match the computed
    /// Fletcher-8 checksum.  The frame is discarded and the parser re-syncs.
    checksum_mismatch = 1u,

    /// Declared payload length exceeds UBX_MAX_PAYLOAD_LEN.
    /// Indicates a framing error or corrupted length field.
    payload_too_large = 2u,

    /// A valid UBX sync sequence (0xB5 0x62) was found inside what was
    /// believed to be a payload, implying the prior frame was malformed.
    /// The parser aborts the current frame and begins re-parsing from the
    /// embedded sync bytes.
    unexpected_sync_in_payload = 3u,

    /// The internal byte ring-buffer or work buffer exceeded its capacity.
    /// This is a defensive error that should not occur in normal usage.
    buffer_overflow = 4u,
};

// ─── parse_error_info ─────────────────────────────────────────────────────────
//
// Context block delivered to the parse-error callback.

struct parse_error_info
{
    /// The error code describing what went wrong.
    parser_error_code code;

    /// The UBX message class byte at the time of the error (0 if not yet parsed).
    uint8_t msg_class;

    /// The UBX message ID byte at the time of the error (0 if not yet parsed).
    uint8_t msg_id;

    /// Human-readable description of the error for logging.
    std::string description;
};

// ─── error_message ────────────────────────────────────────────────────────────
//
// Returns a short, stable human-readable string for each error code.
// Useful for logging without pulling in a message catalogue.

inline std::string error_message(parser_error_code code)
{
    switch (code)
    {
        case parser_error_code::none:
            return "no error";
        case parser_error_code::checksum_mismatch:
            return "UBX checksum mismatch — frame discarded";
        case parser_error_code::payload_too_large:
            return "UBX payload length exceeds maximum — frame discarded";
        case parser_error_code::unexpected_sync_in_payload:
            return "unexpected UBX sync bytes in payload — resyncing";
        case parser_error_code::buffer_overflow:
            return "internal buffer overflow";
        default:
            return "unknown error";
    }
}

/// Callback type for parse errors.
/// The callback receives a fully populated parse_error_info by const-ref.
using parse_error_callback = std::function<void(const parse_error_info&)>;

} // namespace parser
} // namespace ubx
