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
// messages/ubx_nav_sig.h
//
// Decoded representation of the UBX-NAV-SIG message (class 0x01, ID 0x43).
//
// Reference: u-blox M10 Interface Description, Section UBX-NAV-SIG.
// Payload layout:
//   Header (8 bytes): iTOW(U4), version(U1), numSigs(U1), reserved(U1[2])
//   Per signal block (16 bytes) × numSigs:
//     gnssId(U1), svId(U1), sigId(U1), freqId(U1), prRes(I2), cno(U1),
//     qualInd(U1), corrSource(U1), ionoModel(U1), sigFlags(X2), reserved(U1[4])
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

/// Fixed header length of UBX-NAV-SIG payload.
static constexpr uint16_t UBX_NAV_SIG_HEADER_LEN = 8u;

/// Per-signal block length within UBX-NAV-SIG payload.
static constexpr uint16_t UBX_NAV_SIG_BLOCK_LEN = 16u;

// ─── nav_sig_info ─────────────────────────────────────────────────────────────
//
// Decoded per-signal block from a UBX-NAV-SIG message.

struct nav_sig_info
{
    uint8_t  gnss_id;      ///< GNSS identifier
    uint8_t  sv_id;        ///< Satellite identifier within GNSS
    uint8_t  sig_id;       ///< Signal identifier within GNSS/SV (see spec)
    uint8_t  freq_id;      ///< GLONASS frequency slot (+7); range 0–13; 255 = unknown
    int16_t  pr_res;       ///< Pseudorange residual [0.1 m]
    uint8_t  cno;          ///< Carrier-to-noise density ratio [dBHz]
    uint8_t  qual_ind;     ///< Signal quality indicator (0–7)
    uint8_t  corr_source;  ///< Correction source (0=none, 1=SBAS, 2=BeiDou, 3=RTCM2, 4=RTCM3, 5=QZSS)
    uint8_t  iono_model;   ///< Ionospheric model used (0=none, 1=Klobuchar GPS, 2=SBAS, 3=dual-freq, 8=Klobuchar BeiDou)
    uint16_t sig_flags;    ///< Signal flags bitmask (see spec)
};

// ─── ubx_nav_sig ──────────────────────────────────────────────────────────────
//
// Decoded UBX-NAV-SIG message.

struct ubx_nav_sig
{
    uint32_t i_tow;                      ///< GPS time of week [ms]
    uint8_t  version;                    ///< Message version (= 0x00)
    uint8_t  num_sigs;                   ///< Number of signals reported
    std::vector<nav_sig_info> signals;   ///< Per-signal data records
};

/// Callback for decoded UBX-NAV-SIG messages.
using nav_sig_callback = std::function<void(const ubx_nav_sig&)>;

} // namespace parser
} // namespace ubx
