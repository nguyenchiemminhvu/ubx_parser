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
// messages/ubx_esf_ins.h
//
// Decoded representation of the UBX-ESF-INS message (class 0x10, ID 0x15).
//
// Reference: u-blox ADR/UDR Interface Description, Section UBX-ESF-INS.
// Payload size: 36 bytes (fixed).
// Layout: bitfield0(X4), reserved1(U1[4]), iTOW(U4),
//         xAngRate(I4), yAngRate(I4), zAngRate(I4),
//         xAccel(I4), yAccel(I4), zAccel(I4)
//
// Provides compensated angular rates and accelerations from the IMU.
// Angular rates: [1e-3 deg/s], Accelerations: [1e-3 m/s^2].
//
// bitfield0 (bits 0-2): validity flags
//   bit 0 —  xAngRateValid
//   bit 1 —  yAngRateValid
//   bit 2 —  zAngRateValid
//   bit 3 —  xAccelValid
//   bit 4 —  yAccelValid
//   bit 5 —  zAccelValid

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-ESF-INS.
static constexpr uint16_t UBX_ESF_INS_PAYLOAD_LEN = 36u;

// ─── ubx_esf_ins ──────────────────────────────────────────────────────────────
//
// Decoded UBX-ESF-INS message.

struct ubx_esf_ins
{
    uint32_t bitfield0;    ///< Validity bitfield (see bit definitions below)
    uint32_t i_tow;        ///< GPS time of week [ms]
    int32_t  x_ang_rate;   ///< Compensated x-axis angular rate [1e-3 deg/s]
    int32_t  y_ang_rate;   ///< Compensated y-axis angular rate [1e-3 deg/s]
    int32_t  z_ang_rate;   ///< Compensated z-axis angular rate [1e-3 deg/s]
    int32_t  x_accel;      ///< Compensated x-axis acceleration [1e-3 m/s^2]
    int32_t  y_accel;      ///< Compensated y-axis acceleration [1e-3 m/s^2]
    int32_t  z_accel;      ///< Compensated z-axis acceleration [1e-3 m/s^2]

    // ── Validity bit masks (for bitfield0) ────────────────────────────────
    static constexpr uint32_t BF0_X_ANG_RATE_VALID = 0x01u;
    static constexpr uint32_t BF0_Y_ANG_RATE_VALID = 0x02u;
    static constexpr uint32_t BF0_Z_ANG_RATE_VALID = 0x04u;
    static constexpr uint32_t BF0_X_ACCEL_VALID    = 0x08u;
    static constexpr uint32_t BF0_Y_ACCEL_VALID    = 0x10u;
    static constexpr uint32_t BF0_Z_ACCEL_VALID    = 0x20u;
};

/// Callback for decoded UBX-ESF-INS messages.
using esf_ins_callback = std::function<void(const ubx_esf_ins&)>;

} // namespace parser
} // namespace ubx
