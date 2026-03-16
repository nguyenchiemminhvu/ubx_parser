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
// messages/ubx_tim_tp.h
//
// Decoded representation of the UBX-TIM-TP message (class 0x0D, ID 0x01).
//
// Reference: u-blox M10 Interface Description, Section UBX-TIM-TP.
// Payload size: 16 bytes (fixed).
// Layout: towMS(U4), towSubMS(U4), qErr(I4), week(U2), flags(X1), refInfo(X1)
//
// towSubMS scale: 2^−32 ms per LSB (i.e. raw value / 2^32 gives fractional ms).
// qErr: quantisation error of timepulse [ps].
// flags bits:
//   bits 0-1: timeBase (0=GNSS, 1=UTC, 2=GLONASS, 3=BeiDou time)
//   bit 2:    utcAvail (1 if UTC info available)
//   bit 3:    timeRefGnss (GNSS the TP is aligned to: 0=GPS, 1=GLONASS, 2=BeiDou, 3=Galileo, 15=unknown)

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-TIM-TP.
static constexpr uint16_t UBX_TIM_TP_PAYLOAD_LEN = 16u;

// ─── ubx_tim_tp ───────────────────────────────────────────────────────────────
//
// Decoded UBX-TIM-TP message.

struct ubx_tim_tp
{
    uint32_t tow_ms;      ///< Timepulse time of week according to time base [ms]
    uint32_t tow_sub_ms;  ///< Sub-millisecond part of towMS; scale 2^−32 ms/LSB
    int32_t  q_err;       ///< Quantisation error of timepulse [ps]
    uint16_t week;        ///< Timepulse week number according to time base
    uint8_t  flags;       ///< Flags (timeBase, utcAvail, timeRefGnss bits)
    uint8_t  ref_info;    ///< Time reference information (GNSS ID + UTC standard)
};

/// Callback for decoded UBX-TIM-TP messages.
using tim_tp_callback = std::function<void(const ubx_tim_tp&)>;

} // namespace parser
} // namespace ubx
