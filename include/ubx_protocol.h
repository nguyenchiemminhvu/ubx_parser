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
// ubx_protocol.h
//
// Header-only UBX binary protocol utilities for the ubx_parser library.
//
// Covers:
//   - Fletcher-8 checksum calculation
//   - Little-endian multi-byte read helpers
//   - UBX frame construction (for use by ubx_message_builder)
//
// All functions are inline; no translation unit required.
//
// This file mirrors ubx_protocol_utils.h from the ubx_config library but lives
// in the ubx::parser namespace to avoid ODR violations if both libraries are
// linked together.

#pragma once

#include "ubx_types.h"
#include <cstddef>
#include <vector>

namespace ubx
{
namespace parser
{
namespace protocol
{

// ─── Little-endian read helpers ───────────────────────────────────────────────

/// Read a 2-byte little-endian value from pointer 'p'.
inline uint16_t read_le16(const uint8_t* p)
{
    return static_cast<uint16_t>(p[0])
         | (static_cast<uint16_t>(p[1]) << 8u);
}

/// Read a 4-byte little-endian value from pointer 'p'.
inline uint32_t read_le32(const uint8_t* p)
{
    return  static_cast<uint32_t>(p[0])
         | (static_cast<uint32_t>(p[1]) <<  8u)
         | (static_cast<uint32_t>(p[2]) << 16u)
         | (static_cast<uint32_t>(p[3]) << 24u);
}

/// Read an 8-byte little-endian value from pointer 'p'.
inline uint64_t read_le64(const uint8_t* p)
{
    uint64_t v = 0u;
    for (unsigned i = 0u; i < 8u; ++i)
        v |= (static_cast<uint64_t>(p[i]) << (8u * i));
    return v;
}

/// Read a 4-byte little-endian signed integer from pointer 'p'.
inline int32_t read_le32s(const uint8_t* p)
{
    return static_cast<int32_t>(read_le32(p));
}

// ─── Little-endian write helpers ──────────────────────────────────────────────

/// Append a single byte to a vector.
inline void write_u8(std::vector<uint8_t>& buf, uint8_t v)
{
    buf.push_back(v);
}

/// Append a 2-byte little-endian value to a vector.
inline void write_le16(std::vector<uint8_t>& buf, uint16_t v)
{
    buf.push_back(static_cast<uint8_t>( v        & 0xFFu));
    buf.push_back(static_cast<uint8_t>((v >>  8u) & 0xFFu));
}

/// Append a 4-byte little-endian value to a vector.
inline void write_le32(std::vector<uint8_t>& buf, uint32_t v)
{
    buf.push_back(static_cast<uint8_t>( v        & 0xFFu));
    buf.push_back(static_cast<uint8_t>((v >>  8u) & 0xFFu));
    buf.push_back(static_cast<uint8_t>((v >> 16u) & 0xFFu));
    buf.push_back(static_cast<uint8_t>((v >> 24u) & 0xFFu));
}

/// Append an 8-byte little-endian value to a vector.
inline void write_le64(std::vector<uint8_t>& buf, uint64_t v)
{
    for (unsigned i = 0u; i < 8u; ++i)
        buf.push_back(static_cast<uint8_t>((v >> (8u * i)) & 0xFFu));
}

// ─── UBX Fletcher-8 checksum ──────────────────────────────────────────────────
//
// The checksum is computed over all bytes starting from the class byte through
// the last byte of the payload (i.e. everything between the sync chars and the
// checksum bytes themselves).
//
// Algorithm (8-bit Fletcher):
//   CK_A = 0; CK_B = 0
//   for each byte b:
//       CK_A += b
//       CK_B += CK_A

/// Compute the UBX Fletcher-8 checksum in-place.
/// @param data   Pointer to the first byte to include (usually the class byte).
/// @param len    Number of bytes to cover.
/// @param ck_a   Output: first checksum byte.
/// @param ck_b   Output: second checksum byte.
inline void compute_checksum(const uint8_t* data, std::size_t len,
                              uint8_t& ck_a, uint8_t& ck_b)
{
    ck_a = 0u;
    ck_b = 0u;
    for (std::size_t i = 0u; i < len; ++i)
    {
        ck_a = static_cast<uint8_t>(ck_a + data[i]);
        ck_b = static_cast<uint8_t>(ck_b + ck_a);
    }
}

// ─── UBX frame assembly ───────────────────────────────────────────────────────
//
// Assembles a complete, checksummed UBX binary frame from class, ID and payload:
//
//   [0xB5] [0x62] [class] [id] [len_lo] [len_hi] [payload...] [CK_A] [CK_B]
//
// @param msg_class  UBX message class byte.
// @param msg_id     UBX message ID byte.
// @param payload    Raw payload bytes (may be empty).
// @returns  Fully framed, checksummed byte vector ready for transmission.

inline std::vector<uint8_t> build_ubx_frame(uint8_t msg_class,
                                              uint8_t msg_id,
                                              const std::vector<uint8_t>& payload)
{
    const uint16_t payload_len = static_cast<uint16_t>(payload.size());

    std::vector<uint8_t> frame;
    frame.reserve(static_cast<std::size_t>(UBX_FRAME_OVERHEAD + payload_len));

    frame.push_back(UBX_SYNC_CHAR_1);
    frame.push_back(UBX_SYNC_CHAR_2);
    frame.push_back(msg_class);
    frame.push_back(msg_id);
    write_le16(frame, payload_len);
    frame.insert(frame.end(), payload.begin(), payload.end());

    // Checksum covers: class, id, len_lo, len_hi, payload
    uint8_t ck_a = 0u, ck_b = 0u;
    compute_checksum(frame.data() + 2u,          // start at class byte
                     static_cast<std::size_t>(4u + payload_len),  // class+id+len+payload
                     ck_a, ck_b);

    frame.push_back(ck_a);
    frame.push_back(ck_b);

    return frame;
}

// ─── UBX frame field accessors ────────────────────────────────────────────────
//
// These helpers extract individual header fields from a raw, fully-framed UBX
// byte vector.  They assume the caller has already verified the frame has at
// least UBX_FRAME_OVERHEAD bytes and starts with the two sync characters.
//
// Frame layout reminder:
//   [0] 0xB5  sync1
//   [1] 0x62  sync2
//   [2]       class
//   [3]       id
//   [4..5]    payload length (little-endian)
//   [6..N-3]  payload
//   [N-2]     CK_A
//   [N-1]     CK_B

/// Extract the message-class byte from a raw UBX frame.
/// @param frame  Complete frame bytes.
/// @returns      Message class byte, or 0 if the frame is too short.
inline uint8_t get_frame_msg_class(const std::vector<uint8_t>& frame)
{
    if (frame.size() < static_cast<std::size_t>(UBX_FRAME_OVERHEAD))
        return 0u;
    return frame[2u];
}

/// Extract the message-ID byte from a raw UBX frame.
/// @param frame  Complete frame bytes.
/// @returns      Message ID byte, or 0 if the frame is too short.
inline uint8_t get_frame_msg_id(const std::vector<uint8_t>& frame)
{
    if (frame.size() < static_cast<std::size_t>(UBX_FRAME_OVERHEAD))
        return 0u;
    return frame[3u];
}

/// Extract both message-class and message-ID from a raw UBX frame in one call.
/// @param frame      Complete frame bytes.
/// @param msg_class  Output: message class byte.
/// @param msg_id     Output: message ID byte.
/// @returns          true on success, false if the frame is too short.
inline bool get_frame_msg_class_and_id(const std::vector<uint8_t>& frame,
                                    uint8_t& msg_class,
                                    uint8_t& msg_id)
{
    if (frame.size() < static_cast<std::size_t>(UBX_FRAME_OVERHEAD))
        return false;
    msg_class = frame[2u];
    msg_id    = frame[3u];
    return true;
}

/// Verify the checksum of an already-framed UBX message.
/// @param frame  Complete frame bytes (including sync and checksum bytes).
/// @returns true if the checksum is valid.
inline bool verify_frame_checksum(const std::vector<uint8_t>& frame)
{
    // Minimum: 8 bytes
    if (frame.size() < static_cast<std::size_t>(UBX_FRAME_OVERHEAD))
        return false;

    const std::size_t payload_len = frame.size() - static_cast<std::size_t>(UBX_FRAME_OVERHEAD);
    uint8_t ck_a = 0u, ck_b = 0u;
    compute_checksum(frame.data() + 2u, 4u + payload_len, ck_a, ck_b);

    const std::size_t ck_a_pos = frame.size() - 2u;
    const std::size_t ck_b_pos = frame.size() - 1u;
    return (frame[ck_a_pos] == ck_a) && (frame[ck_b_pos] == ck_b);
}

// ─── UBX configuration key-ID value size ─────────────────────────────────────
//
// UBX configuration key-ID format (32 bits):
//   Bits 31-28  : size code
//       1 → L   : 1-bit boolean, stored as 1 byte
//       2 → U1/I1/X1/E1 : 1 byte
//       3 → U2/I2/X2/E2 : 2 bytes
//       4 → U4/I4/X4/E4 : 4 bytes
//       5 → U8/I8/X8/R8 : 8 bytes
//   Bits 27-16  : group ID
//   Bits 15- 0  : item ID
//
// Returns 0 for unrecognised size codes.

inline uint8_t value_byte_size(uint32_t key_id)
{
    switch ((key_id >> 28u) & 0x0Fu)
    {
        case 1u: return 1u;
        case 2u: return 1u;
        case 3u: return 2u;
        case 4u: return 4u;
        case 5u: return 8u;
        default: return 0u;
    }
}

} // namespace protocol
} // namespace parser
} // namespace ubx
