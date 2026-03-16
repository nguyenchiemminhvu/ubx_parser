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
// messages/ubx_nav2_eell.h
//
// Decoded representation of the UBX-NAV2-EELL message (class 0x29, ID 0x3D).
//
// Reference: u-blox Interface Description (M10/ADR), UBX-NAV2-EELL section.
// Payload size: 16 bytes (fixed) — identical layout to UBX-NAV-EELL.
//
// errMaj/errMin: 1-sigma horizontal error-ellipse semi-axes [0.1 mm].
// errOrient: orientation of semi-major axis from N, clockwise [0.01 deg].

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV2-EELL.
static constexpr uint16_t UBX_NAV2_EELL_PAYLOAD_LEN = 16u;

// ─── ubx_nav2_eell ────────────────────────────────────────────────────────────
//
// Decoded UBX-NAV2-EELL message.

struct ubx_nav2_eell
{
    uint32_t i_tow;       ///< GPS time of week [ms]
    uint8_t  version;     ///< Message version (= 0x00)
    uint16_t err_maj;     ///< 1-sigma semi-major axis of position error ellipse [0.1 mm]
    uint16_t err_min;     ///< 1-sigma semi-minor axis of position error ellipse [0.1 mm]
    uint16_t err_orient;  ///< Orientation of semi-major axis from north, clockwise [0.01 deg]
};

/// Callback for decoded UBX-NAV2-EELL messages.
using nav2_eell_callback = std::function<void(const ubx_nav2_eell&)>;

} // namespace parser
} // namespace ubx
