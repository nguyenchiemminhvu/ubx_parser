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
// messages/ubx_nav_timeutc.h
//
// Decoded representation of the UBX-NAV-TIMEUTC message (class 0x01, ID 0x21).
//
// Reference: u-blox F9 DBD 1.30 Interface Description, Section UBX-NAV-TIMEUTC.
// Payload size: 20 bytes (fixed).
// Layout:
//   iTOW  (U4) : GPS time of week [ms]
//   tAcc  (U4) : Time accuracy estimate [ns]
//   nano  (I4) : Fraction of second, range -1e9 .. 1e9 [ns]
//   year  (U2) : Year (UTC)
//   month (U1) : Month (UTC), range 1..12
//   day   (U1) : Day of month (UTC), range 1..31
//   hour  (U1) : Hour of day (UTC), range 0..23
//   min   (U1) : Minute of hour (UTC), range 0..59
//   sec   (U1) : Second of minute (UTC), range 0..60
//   valid (X1) : Validity flags
//
// valid bits:
//   bit 0 (validTOW)  – iTOW is valid
//   bit 1 (validWKN)  – week number is valid
//   bit 2 (validUTC)  – UTC time is valid
//   bits 6:4 (utcStandard) – UTC standard identifier

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV-TIMEUTC.
static constexpr uint16_t UBX_NAV_TIMEUTC_PAYLOAD_LEN = 20u;

// ─── ubx_nav_timeutc ─────────────────────────────────────────────────────────

struct ubx_nav_timeutc
{
    uint32_t i_tow;   ///< GPS time of week [ms]
    uint32_t t_acc;   ///< Time accuracy estimate [ns]
    int32_t  nano;    ///< Fraction of second [ns]; range -1e9 .. 1e9
    uint16_t year;    ///< Year (UTC)
    uint8_t  month;   ///< Month of year (UTC), 1..12
    uint8_t  day;     ///< Day of month (UTC), 1..31
    uint8_t  hour;    ///< Hour of day (UTC), 0..23
    uint8_t  min;     ///< Minute of hour (UTC), 0..59
    uint8_t  sec;     ///< Second of minute (UTC), 0..60
    uint8_t  valid;   ///< Validity flags (validTOW, validWKN, validUTC, utcStandard)
};

/// Callback for decoded UBX-NAV-TIMEUTC messages.
using nav_timeutc_callback = std::function<void(const ubx_nav_timeutc&)>;

} // namespace parser
} // namespace ubx
