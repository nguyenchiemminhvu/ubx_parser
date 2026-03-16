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
// messages/ubx_nav_sat.h
//
// Decoded representation of the UBX-NAV-SAT message (class 0x01, ID 0x35).
//
// Reference: u-blox M10 Interface Description, Section UBX-NAV-SAT.
// Payload layout:
//   Header (8 bytes): iTOW(U4), version(U1), numSvs(U1), reserved(U1[2])
//   Per SV block (12 bytes) × numSvs:
//     gnssId(U1), svId(U1), cno(U1), elev(I1), azim(I2), prRes(I2), flags(X4)
//
// Field names follow the official specification mapped to snake_case.

#pragma once

#include <cstdint>
#include <vector>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── Payload size constants ────────────────────────────────────────────────────

/// Fixed header length of UBX-NAV-SAT payload.
static constexpr uint16_t UBX_NAV_SAT_HEADER_LEN = 8u;

/// Per-SV block length within UBX-NAV-SAT payload.
static constexpr uint16_t UBX_NAV_SAT_BLOCK_LEN = 12u;

// ─── nav_sat_sv ───────────────────────────────────────────────────────────────
//
// Decoded per-satellite block from a UBX-NAV-SAT message.

struct nav_sat_sv
{
    uint8_t  gnss_id;   ///< GNSS identifier (0=GPS, 1=SBAS, 2=Galileo, 3=BeiDou, 5=QZSS, 6=GLONASS)
    uint8_t  sv_id;     ///< Satellite identifier within GNSS
    uint8_t  cno;       ///< Carrier-to-noise density ratio [dBHz]
    int8_t   elev;      ///< Elevation angle [deg]; range ±90
    int16_t  azim;      ///< Azimuth angle [deg]; range ±180
    int16_t  pr_res;    ///< Pseudorange residual [0.1 m]
    uint32_t flags;     ///< Bitmask of signal and health flags (see spec)
};

// ─── ubx_nav_sat ──────────────────────────────────────────────────────────────
//
// Decoded UBX-NAV-SAT message.

struct ubx_nav_sat
{
    uint32_t i_tow;                  ///< GPS time of week [ms]
    uint8_t  version;                ///< Message version (= 0x01)
    uint8_t  num_svs;                ///< Number of satellites reported
    std::vector<nav_sat_sv> svs;     ///< Per-satellite data records
};

/// Callback for decoded UBX-NAV-SAT messages.
using nav_sat_callback = std::function<void(const ubx_nav_sat&)>;

} // namespace parser
} // namespace ubx
