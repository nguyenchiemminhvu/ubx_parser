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
// messages/ubx_nav_odo.h
//
// Decoded representation of the UBX-NAV-ODO message (class 0x01, ID 0x09).
//
// Reference: u-blox M10 Interface Description, Section UBX-NAV-ODO.
// Payload size: 20 bytes (fixed).
// Layout: version(U1), reserved1(U1[3]), iTOW(U4), distance(U4),
//         totalDistance(U4), distanceStd(U4)
//
// Provides odometer solution produced by the GNSS/DR engine.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV-ODO.
static constexpr uint16_t UBX_NAV_ODO_PAYLOAD_LEN = 20u;

// ─── ubx_nav_odo ──────────────────────────────────────────────────────────────
//
// Decoded UBX-NAV-ODO message.

struct ubx_nav_odo
{
    uint8_t  version;         ///< Message version (= 0x00)
    uint32_t i_tow;           ///< GPS time of week [ms]
    uint32_t distance;        ///< Ground distance since last reset [m]
    uint32_t total_distance;  ///< Total cumulative ground distance [m]
    uint32_t distance_std;    ///< Ground distance accuracy (1-sigma) [m]
};

/// Callback for decoded UBX-NAV-ODO messages.
using nav_odo_callback = std::function<void(const ubx_nav_odo&)>;

} // namespace parser
} // namespace ubx
