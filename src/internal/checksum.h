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
// internal/checksum.h
//
// Thin wrapper that re-exports the checksum function from ubx_protocol.h for
// use by internal parser components without pulling in the full protocol header.

#pragma once

#include "ubx_protocol.h"

namespace ubx
{
namespace parser
{
namespace internal
{

/// Compute UBX Fletcher-8 checksum.
/// @see ubx::parser::protocol::compute_checksum
inline void compute_ubx_checksum(const uint8_t* data, std::size_t len,
                                  uint8_t& ck_a, uint8_t& ck_b)
{
    protocol::compute_checksum(data, len, ck_a, ck_b);
}

/// Verify a decoded frame's checksum.
/// @param msg_class  Class byte (first byte covered by checksum)
/// @param msg_id     ID byte
/// @param payload_len_lo  Low byte of payload length
/// @param payload_len_hi  High byte of payload length
/// @param payload    Pointer to payload bytes
/// @param payload_len Number of payload bytes
/// @param expected_ck_a  Received CK_A
/// @param expected_ck_b  Received CK_B
/// @returns true if checksum matches
inline bool verify_checksum(uint8_t msg_class, uint8_t msg_id,
                             uint8_t payload_len_lo, uint8_t payload_len_hi,
                             const uint8_t* payload, uint16_t payload_len,
                             uint8_t expected_ck_a, uint8_t expected_ck_b)
{
    uint8_t ck_a = 0u;
    uint8_t ck_b = 0u;

    // Cover: class, id, len_lo, len_hi
    const uint8_t header_bytes[4] = { msg_class, msg_id, payload_len_lo, payload_len_hi };
    for (unsigned i = 0u; i < 4u; ++i)
    {
        ck_a = static_cast<uint8_t>(ck_a + header_bytes[i]);
        ck_b = static_cast<uint8_t>(ck_b + ck_a);
    }

    // Cover: payload
    for (uint16_t i = 0u; i < payload_len; ++i)
    {
        ck_a = static_cast<uint8_t>(ck_a + payload[i]);
        ck_b = static_cast<uint8_t>(ck_b + ck_a);
    }

    return (ck_a == expected_ck_a) && (ck_b == expected_ck_b);
}

} // namespace internal
} // namespace parser
} // namespace ubx
