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
// messages/ubx_nav_eell.h
//
// Decoded representation of the UBX-NAV-EELL message (class 0x01, ID 0x3D).
//
// Reference: u-blox HPG/M10 Interface Description, Section UBX-NAV-EELL.
// Payload size: 16 bytes (fixed).
// Layout:
//   iTOW(U4), version(U1), reserved1(U1[3]),
//   errMaj(U2), errMin(U2), errOrient(U2), reserved2(U2)
//
// errMaj/errMin: 1-sigma horizontal error-ellipse semi-axes [mm], scaled 0.1 mm/LSB.
// errOrient: orientation of semi-major axis from N, clockwise [deg], scaled 0.01 deg/LSB.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV-EELL.
static constexpr uint16_t UBX_NAV_EELL_PAYLOAD_LEN = 16u;

// ─── ubx_nav_eell ─────────────────────────────────────────────────────────────
//
// Decoded UBX-NAV-EELL message.

struct ubx_nav_eell
{
    uint32_t i_tow;       ///< GPS time of week [ms]
    uint8_t  version;     ///< Message version (= 0x00)
    uint16_t err_maj;     ///< 1-sigma semi-major axis of position error ellipse [0.1 mm]
    uint16_t err_min;     ///< 1-sigma semi-minor axis of position error ellipse [0.1 mm]
    uint16_t err_orient;  ///< Orientation of semi-major axis, from north, clockwise [0.01 deg]
};

/// Callback for decoded UBX-NAV-EELL messages.
using nav_eell_callback = std::function<void(const ubx_nav_eell&)>;

} // namespace parser
} // namespace ubx
