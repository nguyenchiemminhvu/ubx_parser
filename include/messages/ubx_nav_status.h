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
// messages/ubx_nav_status.h
//
// Decoded representation of the UBX-NAV-STATUS message (class 0x01, ID 0x03).
//
// Reference: u-blox M10 Interface Description, Section UBX-NAV-STATUS.
// Payload size: 16 bytes (fixed).
// Layout: iTOW(U4), gpsFix(U1), flags(X1), fixStat(X1), flags2(X1), ttff(U4), msss(U4)

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV-STATUS.
static constexpr uint16_t UBX_NAV_STATUS_PAYLOAD_LEN = 16u;

// ─── nav_status_gps_fix ───────────────────────────────────────────────────────

/// gpsFix enumeration values for UBX-NAV-STATUS.
enum class nav_status_gps_fix : uint8_t
{
    no_fix       = 0x00u,  ///< No fix
    dead_reck    = 0x01u,  ///< Dead-reckoning only
    fix_2d       = 0x02u,  ///< 2D fix
    fix_3d       = 0x03u,  ///< 3D fix
    gnss_dr      = 0x04u,  ///< GNSS + dead-reckoning combined
    time_only    = 0x05u,  ///< Time-only fix
};

// ─── ubx_nav_status ───────────────────────────────────────────────────────────
//
// Decoded UBX-NAV-STATUS message.

struct ubx_nav_status
{
    uint32_t           i_tow;    ///< GPS time of week [ms]
    nav_status_gps_fix gps_fix;  ///< GPS fix type
    uint8_t            flags;    ///< Navigation status flags (bit 0 = gpsFixOk)
    uint8_t            fix_stat; ///< Fix status information (differential corrections, DGPS)
    uint8_t            flags2;   ///< Further information about navigation output (power save mode)
    uint32_t           ttff;     ///< Time to first fix [ms]
    uint32_t           msss;     ///< Milliseconds since startup / reset [ms]
};

/// Callback for decoded UBX-NAV-STATUS messages.
using nav_status_callback = std::function<void(const ubx_nav_status&)>;

} // namespace parser
} // namespace ubx
