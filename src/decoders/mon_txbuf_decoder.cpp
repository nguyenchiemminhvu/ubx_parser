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
// Decoder for UBX-MON-TXBUF (class 0x0A, ID 0x08).
//
// Payload layout (28 bytes, fixed):
//   Bytes  0-11: pending[6]   (U2[6]) bytes pending per target
//   Bytes 12-17: usage[6]     (U1[6]) current usage [%]
//   Bytes 18-23: peakUsage[6] (U1[6]) peak usage [%]
//   Byte  24:    tUsed        (U1)    transmitter buffer usage [%]
//   Byte  25:    tPeak        (U1)    max tUsed [%]
//   Byte  26:    errors       (X1)    error flags
//   Byte  27:    reserved1    (U1)

#include "decoders/mon_txbuf_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

mon_txbuf_decoder::mon_txbuf_decoder(mon_txbuf_callback cb)
    : cb_(std::move(cb))
{}

uint8_t mon_txbuf_decoder::msg_class() const { return UBX_CLASS_MON; }
uint8_t mon_txbuf_decoder::msg_id()    const { return UBX_ID_MON_TXBUF; }

bool mon_txbuf_decoder::decode(const ubx_frame& frame)
{
    // UBX-MON-TXBUF payload must be exactly 28 bytes.
    if (frame.header.payload_length != UBX_MON_TXBUF_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_mon_txbuf msg{};

    // ── Bytes 0-11: pending[6] ───────────────────────────────────────────────
    for (uint8_t i = 0u; i < UBX_MON_TXBUF_NUM_TARGETS; ++i)
    {
        if (!r.read_le16(msg.pending[i])) return false;
    }

    // ── Bytes 12-17: usage[6] ────────────────────────────────────────────────
    for (uint8_t i = 0u; i < UBX_MON_TXBUF_NUM_TARGETS; ++i)
    {
        if (!r.read_u8(msg.usage[i])) return false;
    }

    // ── Bytes 18-23: peakUsage[6] ────────────────────────────────────────────
    for (uint8_t i = 0u; i < UBX_MON_TXBUF_NUM_TARGETS; ++i)
    {
        if (!r.read_u8(msg.peak_usage[i])) return false;
    }

    // ── Byte 24: tUsed ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.t_used))   return false;

    // ── Byte 25: tPeak ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.t_peak))   return false;

    // ── Byte 26: errors ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.errors))   return false;

    // ── Byte 27: reserved1 ───────────────────────────────────────────────────
    if (!r.skip(1u))              return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
