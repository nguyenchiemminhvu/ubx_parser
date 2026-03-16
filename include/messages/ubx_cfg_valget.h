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
// messages/ubx_cfg_valget.h
//
// Decoded representation of a UBX-CFG-VALGET response message
// (class 0x06, ID 0x8B, version byte = 0x01).
//
// The response contains a header (4 bytes) followed by zero or more
// (key_id, value) pairs.  The value size for each key is encoded in the
// key_id's upper nibble (bits 31-28).
//
// Reference: u-blox Interface Description, Section UBX-CFG-VALGET.

#pragma once

#include <cstdint>
#include <vector>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── cfg_valget_entry ─────────────────────────────────────────────────────────
//
// A single decoded key-value pair from a UBX-CFG-VALGET response.
// The raw 64-bit value accommodates all UBX configuration value sizes
// (1, 2, 4, or 8 bytes), matching the design of ubx_config_types.h.

struct cfg_valget_entry
{
    uint32_t key_id; ///< 32-bit UBX configuration key ID
    uint64_t value;  ///< Raw value (size determined by bits 31-28 of key_id)
};

// ─── ubx_cfg_valget ───────────────────────────────────────────────────────────
//
// Decoded UBX-CFG-VALGET response message.

struct ubx_cfg_valget
{
    uint8_t  version; ///< Response version byte (should be 0x01 for responses)
    uint8_t  layer;   ///< Configuration layer from which values were read
    uint16_t position; ///< Starting position (for paged responses)

    /// Decoded key-value pairs extracted from the response payload.
    std::vector<cfg_valget_entry> entries;
};

/// Expected minimum payload length for UBX-CFG-VALGET response: 4 bytes header.
static constexpr uint16_t UBX_CFG_VALGET_HEADER_LEN = 4u;

/// Callback for decoded UBX-CFG-VALGET response messages.
using cfg_valget_callback = std::function<void(const ubx_cfg_valget&)>;

} // namespace parser
} // namespace ubx
