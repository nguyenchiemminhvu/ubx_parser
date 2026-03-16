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
// ubx_message_builder.h
//
// Utility for constructing custom UBX binary frames.
//
// Provides a fluent API to compose a frame from class, ID and payload bytes,
// then obtain the complete, checksummed wire-format vector.
//
// Note: This utility ONLY builds frames; it does NOT send them.
// Transport operations are the responsibility of the caller (serial_helper).
//
// Thread-safety:
//   ubx_message_builder instances are NOT thread-safe.  Create a separate
//   instance per thread, or guard access with a mutex.

#pragma once

#include "ubx_types.h"
#include <cstdint>
#include <vector>

namespace ubx
{
namespace parser
{

// ─── ubx_message_builder ──────────────────────────────────────────────────────

class ubx_message_builder
{
public:
    ubx_message_builder()  = default;
    ~ubx_message_builder() = default;

    // Non-copyable — builder holds mutable internal state.
    ubx_message_builder(const ubx_message_builder&)            = delete;
    ubx_message_builder& operator=(const ubx_message_builder&) = delete;

    // ── Configuration ─────────────────────────────────────────────────────────

    /// Set the UBX message class byte.
    ubx_message_builder& set_class(uint8_t msg_class);

    /// Set the UBX message ID byte.
    ubx_message_builder& set_id(uint8_t msg_id);

    /// Replace the payload with the provided bytes.
    ubx_message_builder& set_payload(const std::vector<uint8_t>& payload);

    /// Append a single byte to the payload.
    ubx_message_builder& append_u8(uint8_t v);

    /// Append a 2-byte little-endian unsigned integer to the payload.
    ubx_message_builder& append_le16(uint16_t v);

    /// Append a 4-byte little-endian unsigned integer to the payload.
    ubx_message_builder& append_le32(uint32_t v);

    /// Clear the payload buffer.
    ubx_message_builder& clear_payload();

    // ── Build ─────────────────────────────────────────────────────────────────

    /// Build and return the complete UBX frame:
    ///   [0xB5] [0x62] [class] [id] [len_lo] [len_hi] [payload...] [CK_A] [CK_B]
    ///
    /// The builder state is NOT reset after calling build(); you can modify and
    /// call build() again.
    ///
    /// @returns Fully framed, checksummed byte vector.
    std::vector<uint8_t> build() const;

    // ── Factory helpers ───────────────────────────────────────────────────────

    /// Build a complete UBX frame directly from class, ID and payload in one call.
    /// @returns Fully framed, checksummed byte vector.
    static std::vector<uint8_t> make_frame(uint8_t  msg_class,
                                            uint8_t  msg_id,
                                            const std::vector<uint8_t>& payload);

    /// Build a complete UBX frame with an empty payload.
    static std::vector<uint8_t> make_frame(uint8_t msg_class, uint8_t msg_id);

private:
    uint8_t              msg_class_ = 0u;
    uint8_t              msg_id_    = 0u;
    std::vector<uint8_t> payload_;
};

} // namespace parser
} // namespace ubx
