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
// ubx_esf_meas_builder.h
//
// Static factory for building UBX-ESF-MEAS output frames
// (class 0x10, ID 0x02).
//
// UBX-ESF-MEAS is the "host → receiver" message that feeds external sensor
// measurements (IMU, wheel-tick, speed, etc.) into the u-blox dead-reckoning
// engine.  The chip requires one measurement datum per sensor type per message.
//
// Payload layout  (8 + numMeas×4 bytes, no calibTtag):
//   Bytes 0-3 : timeTag  (U4 LE) — sensor-generated time tag [ms]
//   Bytes 4-5 : flags    (X2 LE) — bits[15:11] = numMeas (optional), rest = 0
//   Bytes 6-7 : id       (U2 LE) — data provider identifier
//   Repeated (numMeas × 4 bytes):
//     bits[23:0]  = dataField  (24-bit sensor value, signed or direction-flagged)
//     bits[29:24] = dataType   (6-bit sensor type enum)
//     bits[31:30] = 0          (reserved)
//
// Usage:
//   ubx_esf_meas msg{};
//   msg.time_tag  = time_ms;
//   msg.id        = 0u;
//   msg.num_meas  = n;
//   for (uint8_t i = 0u; i < n; ++i)
//   {
//       msg.data[i].data_type  = sensor_type;
//       msg.data[i].data_value = raw_24bit_int;  // already scaled
//   }
//   auto frame = ubx_esf_meas_builder::build(msg);
//   transport.send_async(frame);

#pragma once

#include "messages/ubx_esf_meas.h"
#include "ubx_types.h"

#include <cstdint>
#include <vector>

namespace ubx
{
namespace parser
{

// ─── ubx_esf_meas_builder ─────────────────────────────────────────────────────
//
// Non-instantiable static builder — all methods are class-level.
// Accepts a fully populated ubx_esf_meas struct (with data_value fields
// already expressed as signed 24-bit integers) and serialises it into a
// complete, checksummed UBX frame ready for transmission.
//
// Responsibilities:
//   • Serialise the 8-byte header (timeTag, flags, id).
//   • Pack each datum: (dataType << 24) | (dataValue & 0x00FFFFFF).
//   • Add UBX sync bytes, checksum, and framing via ubx_message_builder.
//
// Value encoding (caller's responsibility before calling build):
//   Gyro angular rate  (types  5, 12, 13) : raw = (int32_t)(deg_per_s  * 4096.0)
//   Accelerometer      (types 14, 16, 17) : raw = (int32_t)(m_s2       * 1024.0)
//   Gyro temperature   (type  11)         : raw = (int32_t)(celsius    *  100.0)
//   Speed              (type  10 as m/s)  : raw = (int32_t)(m_per_s    * 1000.0)
//   Wheel-tick         (types  6-9)       : raw = (tick_count & 0x7FFFFF) | (dir << 23)

class ubx_esf_meas_builder
{
public:
    ubx_esf_meas_builder()  = delete;
    ~ubx_esf_meas_builder() = delete;

    ubx_esf_meas_builder(const ubx_esf_meas_builder&)            = delete;
    ubx_esf_meas_builder& operator=(const ubx_esf_meas_builder&) = delete;

    /// Build a complete UBX-ESF-MEAS frame from a populated ubx_esf_meas struct.
    /// @param meas  Measurement data with data_value fields in raw 24-bit integer form.
    /// @return      Complete UBX frame bytes including sync, header, payload, checksum.
    ///              Returns an empty vector if meas.num_meas == 0 or exceeds
    ///              UBX_ESF_MEAS_MAX_DATA.
    static std::vector<uint8_t> build(const ubx_esf_meas& meas);
};

} // namespace parser
} // namespace ubx
