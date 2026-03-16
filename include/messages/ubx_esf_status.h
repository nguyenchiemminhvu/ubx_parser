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
// messages/ubx_esf_status.h
//
// Decoded representation of the UBX-ESF-STATUS message (class 0x10, ID 0x10).
//
// Reference: u-blox ADR/UDR Interface Description, Section UBX-ESF-STATUS.
// Payload size: 16 + (4 × numSens) bytes (variable).
//
// Header (16 bytes): iTOW(U4), version(U1), reserved1(U1[7]),
//                    fusionMode(U1), reserved2(U1[2]), numSens(U1)
// Per-sensor block (4 bytes each):
//   sensStatus1(X1), sensStatus2(X1), freq(U1), faults(X1)
//
// fusionMode values:
//   0 = Initialization mode
//   1 = Fusion mode (calibrated)
//   2 = Suspended fusion mode (temporarily no IMU data)
//   3 = Disabled (no IMU receivers configured)
//
// sensStatus1 bits:
//   bits 0-4: sensor type
//   bit 5:    used (sensor data used in navigation)
//   bit 6:    ready (sensor data ready)
//
// sensStatus2 bits:
//   bits 0-1: calibration status (0=not calibrated, 1=calibrating, 2/3=calibrated)
//   bits 2-3: time status (0=no time, 1=first byte, 2=event, 3=time tag)

#pragma once

#include "messages/ubx_esf_sensor_types.h"
#include <cstdint>
#include <functional>
#include <vector>

namespace ubx
{
namespace parser
{

/// Maximum number of ESF sensors we allocate space for.
static constexpr uint8_t UBX_ESF_STATUS_MAX_SENS = 32u;

/// Minimum payload length for UBX-ESF-STATUS (header only, 0 sensors).
static constexpr uint16_t UBX_ESF_STATUS_MIN_PAYLOAD_LEN = 16u;

// ─── ubx_esf_status_sensor ────────────────────────────────────────────────────

struct ubx_esf_status_sensor
{
    uint8_t sens_status1;  ///< Sensor status 1 (type, used, ready flags)
    uint8_t sens_status2;  ///< Calibration + time status
    uint8_t freq;          ///< Received sensor data frequency [Hz]
    uint8_t faults;        ///< Sensor faults (bad meas, bad tag, missing meas, noisy data)

    // ── sensStatus1 bit masks ────────────────────────────────────────────
    static constexpr uint8_t TYPE_MASK  = 0x1Fu;  ///< bits[4:0] sensor type
    static constexpr uint8_t USED       = 0x20u;  ///< bit 5: sensor data used in navigation
    static constexpr uint8_t READY      = 0x40u;  ///< bit 6: sensor data ready

    // ── sensStatus2 bit masks ────────────────────────────────────────────
    static constexpr uint8_t CALIB_STATUS_MASK = 0x03u;  ///< bits[1:0] calibration status
    static constexpr uint8_t TIME_STATUS_MASK  = 0x0Cu;  ///< bits[3:2] time status
};

// ─── ubx_esf_status ──────────────────────────────────────────────────────────

struct ubx_esf_status
{
    uint32_t i_tow;        ///< GPS time of week [ms]
    uint8_t  version;      ///< Message version (= 0x02)
    uint8_t  fusion_mode;  ///< Fusion mode (0=init, 1=fusion, 2=suspended, 3=disabled)
    uint8_t  num_sens;     ///< Number of sensor blocks in payload

    /// Per-sensor info; valid entries: [0, num_sens).
    ubx_esf_status_sensor sensors[UBX_ESF_STATUS_MAX_SENS];
};

/// Callback for decoded UBX-ESF-STATUS messages.
using esf_status_callback = std::function<void(const ubx_esf_status&)>;

} // namespace parser
} // namespace ubx
