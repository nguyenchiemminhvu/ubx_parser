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
// messages/ubx_nav_pvt.h
//
// Decoded representation of the UBX-NAV-PVT message (class 0x01, ID 0x07).
//
// Reference: u-blox M10 Interface Description, Section 3.15 UBX-NAV-PVT.
// Payload size: 92 bytes (fixed).
//
// Field names and bit definitions follow the official specification and are
// mapped to snake_case per the project naming convention.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── nav_pvt_fix_type ─────────────────────────────────────────────────────────

/// GPS fix type enumeration — gnss_fix_ok flag in valid byte must be set.
enum class nav_pvt_fix_type : uint8_t
{
    no_fix     = 0u,
    dead_reck  = 1u,
    fix_2d     = 2u,
    fix_3d     = 3u,
    gnss_dr    = 4u,
    time_only  = 5u,
};

// ─── ubx_nav_pvt ──────────────────────────────────────────────────────────────
//
// Decoded UBX-NAV-PVT message.  All fields are in the units given in the
// official u-blox documentation.

struct ubx_nav_pvt
{
    // ── Time of week / UTC time fields ─────────────────────────────────────
    uint32_t i_tow;          ///< GPS time of week [ms]
    uint16_t year;           ///< UTC year [1 - 9999]
    uint8_t  month;          ///< UTC month [1 - 12]
    uint8_t  day;            ///< UTC day of month [1 - 31]
    uint8_t  hour;           ///< UTC hour [0 - 23]
    uint8_t  min;            ///< UTC minute [0 - 59]
    uint8_t  sec;            ///< UTC second [0 - 60] (60 during leap second)
    uint8_t  valid;          ///< Validity flags (see bit defs below)
    uint32_t t_acc;          ///< Time accuracy estimate [ns]
    int32_t  nano;           ///< Fraction of second [ns]; range ±500000000

    // ── Fix information ────────────────────────────────────────────────────
    nav_pvt_fix_type fix_type; ///< GNSS fix type
    uint8_t  flags;          ///< Fix status flags
    uint8_t  flags2;         ///< Additional flags
    uint8_t  num_sv;         ///< Number of SVs used in Nav solution

    // ── Position ──────────────────────────────────────────────────────────
    int32_t  lon;            ///< Longitude [1e-7 deg]
    int32_t  lat;            ///< Latitude  [1e-7 deg]
    int32_t  height;         ///< Height above ellipsoid [mm]
    int32_t  h_msl;          ///< Height above mean sea level [mm]
    uint32_t h_acc;          ///< Horizontal accuracy estimate [mm]
    uint32_t v_acc;          ///< Vertical accuracy estimate [mm]

    // ── Velocity / heading ─────────────────────────────────────────────────
    int32_t  vel_n;          ///< NED north velocity [mm/s]
    int32_t  vel_e;          ///< NED east  velocity [mm/s]
    int32_t  vel_d;          ///< NED down  velocity [mm/s]
    int32_t  g_speed;        ///< Ground speed (2D) [mm/s]
    int32_t  head_mot;       ///< Heading of motion (2D) [1e-5 deg]
    uint32_t s_acc;          ///< Speed accuracy estimate [mm/s]
    uint32_t head_acc;       ///< Heading accuracy estimate [1e-5 deg]
    uint16_t p_dop;          ///< Position DOP [0.01]
    uint8_t  flags3;         ///< Additional flags
    // 5 reserved bytes skipped
    int32_t  head_veh;       ///< Heading of vehicle (2D) [1e-5 deg]
    int16_t  mag_dec;        ///< Magnetic declination [1e-2 deg]
    uint16_t mag_acc;        ///< Magnetic declination accuracy [1e-2 deg]

    // ── Validity bit masks (for the 'valid' field) ─────────────────────────
    static constexpr uint8_t VALID_DATE    = 0x01u;  ///< Valid UTC date
    static constexpr uint8_t VALID_TIME    = 0x02u;  ///< Valid UTC time of day
    static constexpr uint8_t VALID_FULLY   = 0x04u;  ///< Fully resolved UTC time
    static constexpr uint8_t VALID_MAG_DEC = 0x08u;  ///< Valid magnetic declination

    // ── Fix flags bit masks (for the 'flags' field) ───────────────────────
    static constexpr uint8_t FLAGS_GNSS_FIX_OK    = 0x01u;  ///< Valid fix
    static constexpr uint8_t FLAGS_DIFF_SOLN      = 0x02u;  ///< Differential corrections applied
    static constexpr uint8_t FLAGS_PSM_STATE_MASK = 0x1Cu;  ///< Power save mode state
    static constexpr uint8_t FLAGS_HEAD_VEH_VALID = 0x20u;  ///< Heading of vehicle valid
    static constexpr uint8_t FLAGS_CARR_SOLN_MASK = 0xC0u;  ///< Carrier phase solution
};

/// Expected fixed payload length for UBX-NAV-PVT.
static constexpr uint16_t UBX_NAV_PVT_PAYLOAD_LEN = 92u;

/// Callback for decoded UBX-NAV-PVT messages.
using nav_pvt_callback = std::function<void(const ubx_nav_pvt&)>;

} // namespace parser
} // namespace ubx
