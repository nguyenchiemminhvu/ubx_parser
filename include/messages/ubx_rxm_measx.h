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
// messages/ubx_rxm_measx.h
//
// Decoded representation of the UBX-RXM-MEASX message (class 0x02, ID 0x14).
//
// Reference: u-blox F9 DBD 1.30 Interface Description, Section UBX-RXM-MEASX.
// Payload size: 44 + 24 × numSvs bytes (variable).
//
// Header (44 bytes):
//   version    (U1)     message version (0)
//   reserved1  (U1[3])
//   gpsTOW     (U4)     GPS time of week [ms]
//   gloTOW     (U4)     GLONASS time of week [ms]
//   bdsTOW     (U4)     BeiDou time of week [ms]
//   reserved2  (U1[4])
//   qzssTOW    (U4)     QZSS time of week [ms]
//   gpsTOWacc  (U2)     GPS TOW accuracy [ms/16]
//   gloTOWacc  (U2)     GLONASS TOW accuracy [ms/16]
//   bdsTOWacc  (U2)     BeiDou TOW accuracy [ms/16]
//   reserved3  (U1[2])
//   qzssTOWacc (U2)     QZSS TOW accuracy [ms/16]
//   numSvs     (U1)     number of satellites
//   flags      (X1)     flags
//   reserved4  (U1[8])
//
// Per-SV block (24 bytes × numSvs):
//   gnssId     (U1)     GNSS identifier
//   svId       (U1)     satellite identifier
//   cNo        (U1)     carrier-to-noise ratio [dBHz]
//   mpathIndic (U1)     multipath indicator (0=no multipath, 1=possible, 2=probable)
//   dopplerMS  (I4)     Doppler velocity [m/s × 0.04]
//   dopplerHz  (I4)     Doppler frequency [Hz × 0.2]
//   wholeChips (U2)     whole value of the code phase measurement [chips]
//   fracChips  (U2)     fractional value of the code phase measurement [chips/1024]
//   codePhase  (U4)     code phase measurement [ms/2^21]
//   intCodePhase (U1)   integer part of code phase measurement [ms]
//   pseuRangeRMSErr (U1) estimated pseudorange RMS error index
//   reserved5  (U1[2])

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Maximum number of SVs in a single UBX-RXM-MEASX message.
static constexpr uint8_t  UBX_RXM_MEASX_MAX_SVS      = 64u;
/// Fixed header length of UBX-RXM-MEASX payload.
static constexpr uint16_t UBX_RXM_MEASX_HEADER_LEN   = 44u;
/// Per-SV block length within UBX-RXM-MEASX payload.
static constexpr uint16_t UBX_RXM_MEASX_SV_BLOCK_LEN = 24u;

// ─── rxm_measx_sv ────────────────────────────────────────────────────────────

struct rxm_measx_sv
{
    uint8_t  gnss_id;             ///< GNSS identifier
    uint8_t  sv_id;               ///< Satellite identifier
    uint8_t  cno;                 ///< Carrier-to-noise ratio [dBHz]
    uint8_t  mpath_indic;         ///< Multipath indicator (0=no, 1=possible, 2=probable)
    int32_t  doppler_ms;          ///< Doppler velocity [m/s × 0.04]
    int32_t  doppler_hz;          ///< Doppler frequency [Hz × 0.2]
    uint16_t whole_chips;         ///< Whole code phase [chips]
    uint16_t frac_chips;          ///< Fractional code phase [chips/1024]
    uint32_t code_phase;          ///< Code phase measurement [ms/2^21]
    uint8_t  int_code_phase;      ///< Integer part of code phase [ms]
    uint8_t  pseu_range_rms_err;  ///< Pseudorange RMS error index
};

// ─── ubx_rxm_measx ───────────────────────────────────────────────────────────

struct ubx_rxm_measx
{
    uint8_t  version;      ///< Message version (= 0)
    uint32_t gps_tow;      ///< GPS time of week [ms]
    uint32_t glo_tow;      ///< GLONASS time of week [ms]
    uint32_t bds_tow;      ///< BeiDou time of week [ms]
    uint32_t qzss_tow;     ///< QZSS time of week [ms]
    uint16_t gps_tow_acc;  ///< GPS TOW accuracy [ms/16]
    uint16_t glo_tow_acc;  ///< GLONASS TOW accuracy [ms/16]
    uint16_t bds_tow_acc;  ///< BeiDou TOW accuracy [ms/16]
    uint16_t qzss_tow_acc; ///< QZSS TOW accuracy [ms/16]
    uint8_t  num_svs;      ///< Number of satellites
    uint8_t  flags;        ///< Flags

    /// Per-SV data; valid entries: [0, num_svs).
    rxm_measx_sv svs[UBX_RXM_MEASX_MAX_SVS];
};

/// Callback for decoded UBX-RXM-MEASX messages.
using rxm_measx_callback = std::function<void(const ubx_rxm_measx&)>;

} // namespace parser
} // namespace ubx
