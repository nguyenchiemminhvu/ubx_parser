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
// Decoder for UBX-ESF-STATUS (class 0x10, ID 0x10).
//
// Payload layout:
//   Bytes 0-3:   iTOW (U4)
//   Byte  4:     version (U1)
//   Bytes 5-11:  reserved1 (7 bytes)
//   Byte  12:    fusionMode (U1)
//   Bytes 13-14: reserved2 (2 bytes)
//   Byte  15:    numSens (U1)
//   Per-sensor block (4 bytes × numSens):
//     Byte 0: sensStatus1 (X1)
//     Byte 1: sensStatus2 (X1)
//     Byte 2: freq (U1)
//     Byte 3: faults (X1)

#include "decoders/esf_status_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

esf_status_decoder::esf_status_decoder(esf_status_callback cb)
    : cb_(std::move(cb))
{}

uint8_t esf_status_decoder::msg_class() const { return UBX_CLASS_ESF; }
uint8_t esf_status_decoder::msg_id()    const { return UBX_ID_ESF_STATUS; }

bool esf_status_decoder::decode(const ubx_frame& frame)
{
    // Minimum payload: 16-byte header.
    if (frame.header.payload_length < UBX_ESF_STATUS_MIN_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_esf_status msg{};

    // ── Bytes 0-3: iTOW ─────────────────────────────────────────────────────
    if (!r.read_le32(msg.i_tow))      return false;

    // ── Byte 4: version ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))      return false;

    // ── Bytes 5-11: reserved1 (7 bytes) ──────────────────────────────────────
    if (!r.skip(7u))                  return false;

    // ── Byte 12: fusionMode ──────────────────────────────────────────────────
    if (!r.read_u8(msg.fusion_mode))  return false;

    // ── Bytes 13-14: reserved2 (2 bytes) ─────────────────────────────────────
    if (!r.skip(2u))                  return false;

    // ── Byte 15: numSens ─────────────────────────────────────────────────────
    if (!r.read_u8(msg.num_sens))     return false;

    // Validate: remaining bytes must accommodate numSens × 4-byte blocks.
    const uint16_t expected_len = static_cast<uint16_t>(
        UBX_ESF_STATUS_MIN_PAYLOAD_LEN + 4u * msg.num_sens);
    if (frame.header.payload_length < expected_len)
        return false;

    // Cap at maximum to prevent out-of-bounds writes.
    const uint8_t sens_count = (msg.num_sens <= UBX_ESF_STATUS_MAX_SENS)
                                ? msg.num_sens
                                : UBX_ESF_STATUS_MAX_SENS;
    msg.num_sens = sens_count;

    // ── Per-sensor blocks ────────────────────────────────────────────────────
    for (uint8_t i = 0u; i < sens_count; ++i)
    {
        if (!r.read_u8(msg.sensors[i].sens_status1)) return false;
        if (!r.read_u8(msg.sensors[i].sens_status2)) return false;
        if (!r.read_u8(msg.sensors[i].freq))         return false;
        if (!r.read_u8(msg.sensors[i].faults))       return false;
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
