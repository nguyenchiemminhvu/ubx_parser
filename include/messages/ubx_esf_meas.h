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
// messages/ubx_esf_meas.h
//
// Decoded representation of the UBX-ESF-MEAS message (class 0x10, ID 0x02).
//
// Reference: u-blox ADR/UDR Interface Description, Section UBX-ESF-MEAS.
// Payload size: 8 + (4 × numMeas) bytes (variable), with optional 4-byte
//               calibTtag appended.
//
// Header (8 bytes): timeTag(U4), flags(X2), id(U2)
// Per-measurement datum (4 bytes):
//   data[bits 7:0]    = data bits [23:0] (24-bit signed/unsigned value)
//   dataType[bits 5:0] = sensor data type (bits [29:24])
//   (bits [31:30] are reserved)
//
// dataType values: see ubx_esf_sensor_types.h (esf_sensor_type enum).
// Reference: ZED-F9R Integration Manual (UBX-20039643), Section 3.2.7.5.

#pragma once

#include "messages/ubx_esf_sensor_types.h"
#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Maximum number of ESF measurements per message.
static constexpr uint8_t UBX_ESF_MEAS_MAX_DATA = 32u;

/// Minimum payload length for UBX-ESF-MEAS (header only, 0 measurements).
static constexpr uint16_t UBX_ESF_MEAS_MIN_PAYLOAD_LEN = 8u;

// ─── ubx_esf_meas_datum ───────────────────────────────────────────────────────

struct ubx_esf_meas_datum
{
    uint8_t  data_type;   ///< Sensor data type [bits 29:24 of raw word]
    int32_t  data_value;  ///< Signed 24-bit sensor data (sign-extended to 32-bit)
};

// ─── ubx_esf_meas ─────────────────────────────────────────────────────────────

struct ubx_esf_meas
{
    uint32_t time_tag;    ///< Time tag of measurement [ms] or receiver time
    uint16_t flags;       ///< Flags (bit 3 = timeMarkSent, bits [1:0] = timeTag type)
    uint16_t id;          ///< Identification number of data provider

    uint8_t  num_meas;    ///< Number of decoded measurement data entries

    /// Decoded measurement data; valid entries: [0, num_meas).
    ubx_esf_meas_datum data[UBX_ESF_MEAS_MAX_DATA];

    bool     has_calib_ttag;  ///< True if the optional calibTtag is present
    uint32_t calib_ttag;      ///< Receiver local time [ms] (optional)

    // ── flags bit masks ───────────────────────────────────────────────────
    static constexpr uint16_t FLAGS_TIME_TAG_TYPE_MASK  = 0x0003u;  ///< bits[1:0]
    static constexpr uint16_t FLAGS_TIME_MARK_SENT_MASK = 0x0018u;  ///< bits[4:3]
};

/// Callback for decoded UBX-ESF-MEAS messages.
using esf_meas_callback = std::function<void(const ubx_esf_meas&)>;

} // namespace parser
} // namespace ubx
