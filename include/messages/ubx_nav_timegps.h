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
// messages/ubx_nav_timegps.h
//
// Decoded representation of the UBX-NAV-TIMEGPS message (class 0x01, ID 0x20).
//
// Reference: u-blox M10 Interface Description, Section UBX-NAV-TIMEGPS.
// Payload size: 16 bytes (fixed).
// Layout: iTOW(U4), fTOW(I4), week(I2), leapS(I1), valid(X1), tAcc(U4)
//
// valid flags (bit mask):
//   bit 0 (towValid)  — iTOW and fTOW are valid
//   bit 1 (weekValid) — GPS week number is valid
//   bit 2 (leapSValid)— leap second count is valid

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV-TIMEGPS.
static constexpr uint16_t UBX_NAV_TIMEGPS_PAYLOAD_LEN = 16u;

// ─── ubx_nav_timegps ──────────────────────────────────────────────────────────
//
// Decoded UBX-NAV-TIMEGPS message.

struct ubx_nav_timegps
{
    uint32_t i_tow;    ///< GPS time of week [ms]
    int32_t  f_tow;    ///< Fractional part of iTOW [ns]; range ±500000
    int16_t  week;     ///< GPS week number
    int8_t   leap_s;   ///< GPS leap seconds (GPS-UTC) [s]
    uint8_t  valid;    ///< Validity flags bitmask (towValid, weekValid, leapSValid)
    uint32_t t_acc;    ///< Time accuracy estimate [ns]
};

/// Callback for decoded UBX-NAV-TIMEGPS messages.
using nav_timegps_callback = std::function<void(const ubx_nav_timegps&)>;

} // namespace parser
} // namespace ubx
