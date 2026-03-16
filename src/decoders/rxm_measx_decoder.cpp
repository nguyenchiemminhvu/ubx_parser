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
// Decoder for UBX-RXM-MEASX (class 0x02, ID 0x14).
//
// Payload layout:
//   Bytes  0:    version (U1)
//   Bytes  1-3:  reserved1 (U1[3])
//   Bytes  4-7:  gpsTOW (U4)  [ms]
//   Bytes  8-11: gloTOW (U4)  [ms]
//   Bytes 12-15: bdsTOW (U4)  [ms]
//   Bytes 16-19: reserved2 (U1[4])
//   Bytes 20-23: qzssTOW (U4) [ms]
//   Bytes 24-25: gpsTOWacc (U2)  [ms/16]
//   Bytes 26-27: gloTOWacc (U2)  [ms/16]
//   Bytes 28-29: bdsTOWacc (U2)  [ms/16]
//   Bytes 30-31: reserved3 (U1[2])
//   Bytes 32-33: qzssTOWacc (U2) [ms/16]
//   Byte  34:    numSvs (U1)
//   Byte  35:    flags (X1)
//   Bytes 36-43: reserved4 (U1[8])
//   Per-SV blocks (24 bytes × numSvs):
//     Byte  0:    gnssId (U1)
//     Byte  1:    svId (U1)
//     Byte  2:    cNo (U1)   [dBHz]
//     Byte  3:    mpathIndic (U1)
//     Bytes 4-7:  dopplerMS (I4)  [m/s × 0.04]
//     Bytes 8-11: dopplerHz (I4)  [Hz × 0.2]
//     Bytes 12-13: wholeChips (U2)
//     Bytes 14-15: fracChips (U2)
//     Bytes 16-19: codePhase (U4)
//     Byte  20:   intCodePhase (U1)
//     Byte  21:   pseuRangeRMSErr (U1)
//     Bytes 22-23: reserved5 (U1[2])

#include "decoders/rxm_measx_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

rxm_measx_decoder::rxm_measx_decoder(rxm_measx_callback cb)
    : cb_(std::move(cb))
{}

uint8_t rxm_measx_decoder::msg_class() const { return UBX_CLASS_RXM; }
uint8_t rxm_measx_decoder::msg_id()    const { return UBX_ID_RXM_MEASX; }

bool rxm_measx_decoder::decode(const ubx_frame& frame)
{
    // Minimum payload: 44-byte fixed header.
    if (frame.header.payload_length < UBX_RXM_MEASX_HEADER_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_rxm_measx msg{};

    // ── Byte 0: version ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))       return false;

    // ── Bytes 1-3: reserved1 ─────────────────────────────────────────────────
    if (!r.skip(3u))                   return false;

    // ── Bytes 4-7: gpsTOW ────────────────────────────────────────────────────
    if (!r.read_le32(msg.gps_tow))     return false;

    // ── Bytes 8-11: gloTOW ───────────────────────────────────────────────────
    if (!r.read_le32(msg.glo_tow))     return false;

    // ── Bytes 12-15: bdsTOW ──────────────────────────────────────────────────
    if (!r.read_le32(msg.bds_tow))     return false;

    // ── Bytes 16-19: reserved2 ───────────────────────────────────────────────
    if (!r.skip(4u))                   return false;

    // ── Bytes 20-23: qzssTOW ─────────────────────────────────────────────────
    if (!r.read_le32(msg.qzss_tow))    return false;

    // ── Bytes 24-25: gpsTOWacc ───────────────────────────────────────────────
    if (!r.read_le16(msg.gps_tow_acc)) return false;

    // ── Bytes 26-27: gloTOWacc ───────────────────────────────────────────────
    if (!r.read_le16(msg.glo_tow_acc)) return false;

    // ── Bytes 28-29: bdsTOWacc ───────────────────────────────────────────────
    if (!r.read_le16(msg.bds_tow_acc)) return false;

    // ── Bytes 30-31: reserved3 ───────────────────────────────────────────────
    if (!r.skip(2u))                   return false;

    // ── Bytes 32-33: qzssTOWacc ──────────────────────────────────────────────
    if (!r.read_le16(msg.qzss_tow_acc)) return false;

    // ── Byte 34: numSvs ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.num_svs))       return false;

    // ── Byte 35: flags ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.flags))         return false;

    // ── Bytes 36-43: reserved4 ───────────────────────────────────────────────
    if (!r.skip(8u))                   return false;

    // Validate total payload length.
    const uint16_t expected_len = static_cast<uint16_t>(
        UBX_RXM_MEASX_HEADER_LEN +
        static_cast<uint32_t>(msg.num_svs) * UBX_RXM_MEASX_SV_BLOCK_LEN);
    if (frame.header.payload_length != expected_len)
        return false;

    // Cap at maximum to prevent OOB writes.
    const uint8_t sv_count = (msg.num_svs <= UBX_RXM_MEASX_MAX_SVS)
                             ? msg.num_svs
                             : UBX_RXM_MEASX_MAX_SVS;
    msg.num_svs = sv_count;

    // ── Per-SV blocks ────────────────────────────────────────────────────────
    for (uint8_t i = 0u; i < sv_count; ++i)
    {
        rxm_measx_sv& sv = msg.svs[i];

        // Byte 0: gnssId
        if (!r.read_u8(sv.gnss_id))            return false;

        // Byte 1: svId
        if (!r.read_u8(sv.sv_id))              return false;

        // Byte 2: cNo
        if (!r.read_u8(sv.cno))                return false;

        // Byte 3: mpathIndic
        if (!r.read_u8(sv.mpath_indic))        return false;

        // Bytes 4-7: dopplerMS (signed)
        if (!r.read_le32s(sv.doppler_ms))      return false;

        // Bytes 8-11: dopplerHz (signed)
        if (!r.read_le32s(sv.doppler_hz))      return false;

        // Bytes 12-13: wholeChips
        if (!r.read_le16(sv.whole_chips))      return false;

        // Bytes 14-15: fracChips
        if (!r.read_le16(sv.frac_chips))       return false;

        // Bytes 16-19: codePhase
        if (!r.read_le32(sv.code_phase))       return false;

        // Byte 20: intCodePhase
        if (!r.read_u8(sv.int_code_phase))     return false;

        // Byte 21: pseuRangeRMSErr
        if (!r.read_u8(sv.pseu_range_rms_err)) return false;

        // Bytes 22-23: reserved5
        if (!r.skip(2u))                       return false;
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
