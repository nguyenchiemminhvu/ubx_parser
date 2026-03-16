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
// messages/ubx_sec_crc.h
//
// Decoded representation of the UBX-SEC-CRC message
// (class 0x27, ID 0x01).
//
// UBX-SEC-CRC reports the firmware or configuration CRC computed by the
// receiver's security subsystem.  The exact payload layout may vary across
// firmware versions; fields here follow the common definition.
//
// Reference: u-blox High-precision GNSS receiver interface description,
//            Section UBX-SEC-CRC.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── ubx_sec_crc ──────────────────────────────────────────────────────────────

struct ubx_sec_crc
{
    uint8_t  version;    ///< Message version
    uint8_t  crc_type;   ///< CRC type identifier
    uint16_t reserved;   ///< Reserved / padding

    /// CRC32 value computed over the indicated target region.
    uint32_t crc32;
};

/// Expected fixed payload length for UBX-SEC-CRC (version 0).
static constexpr uint16_t UBX_SEC_CRC_PAYLOAD_LEN = 8u;

/// Callback for decoded UBX-SEC-CRC messages.
using sec_crc_callback = std::function<void(const ubx_sec_crc&)>;

} // namespace parser
} // namespace ubx
