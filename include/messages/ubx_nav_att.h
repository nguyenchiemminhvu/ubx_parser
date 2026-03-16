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
// messages/ubx_nav_att.h
//
// Decoded representation of the UBX-NAV-ATT message (class 0x01, ID 0x05).
//
// Reference: u-blox F9 DBD 1.30 Interface Description, Section UBX-NAV-ATT.
// Payload size: 32 bytes (fixed).
// Layout:
//   iTOW     (U4) : GPS time of week [ms]
//   version  (U1) : Message version (0 for this version)
//   reserved (U1[3])
//   roll     (I4) : Vehicle roll [deg × 1e-5]
//   pitch    (I4) : Vehicle pitch [deg × 1e-5]
//   heading  (I4) : Vehicle heading [deg × 1e-5]
//   accRoll  (U4) : Vehicle roll accuracy [deg × 1e-5]
//   accPitch (U4) : Vehicle pitch accuracy [deg × 1e-5]
//   accHeading (U4) : Vehicle heading accuracy [deg × 1e-5]

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV-ATT.
static constexpr uint16_t UBX_NAV_ATT_PAYLOAD_LEN = 32u;

// ─── ubx_nav_att ─────────────────────────────────────────────────────────────

struct ubx_nav_att
{
    uint32_t i_tow;       ///< GPS time of week [ms]
    uint8_t  version;     ///< Message version (= 0)
    int32_t  roll;        ///< Vehicle roll [deg × 1e-5]
    int32_t  pitch;       ///< Vehicle pitch [deg × 1e-5]
    int32_t  heading;     ///< Vehicle heading [deg × 1e-5]
    uint32_t acc_roll;    ///< Vehicle roll accuracy [deg × 1e-5]
    uint32_t acc_pitch;   ///< Vehicle pitch accuracy [deg × 1e-5]
    uint32_t acc_heading; ///< Vehicle heading accuracy [deg × 1e-5]
};

/// Callback for decoded UBX-NAV-ATT messages.
using nav_att_callback = std::function<void(const ubx_nav_att&)>;

} // namespace parser
} // namespace ubx
