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
// Decoder for UBX-ESF-MEAS (class 0x10, ID 0x02).
//
// Payload layout:
//   Bytes 0-3:   timeTag (U4)
//   Bytes 4-5:   flags (X2)
//   Bytes 6-7:   id (U2)
//   Repeating groups of 4 bytes (numMeas items):
//     data    = bits [23:0]  (signed 24-bit sensor value)
//     dataType= bits [29:24] (6-bit sensor data type)
//     bits [31:30] = reserved
//   Optional trailing 4 bytes:
//     calibTtag (U4), present when (flags bits [1:0] != 0)
//
// numMeas is inferred from (payload_length - header_size) / 4, excluding
// the optional calibTtag.

#include "decoders/esf_meas_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

esf_meas_decoder::esf_meas_decoder(esf_meas_callback cb)
    : cb_(std::move(cb))
{}

uint8_t esf_meas_decoder::msg_class() const { return UBX_CLASS_ESF; }
uint8_t esf_meas_decoder::msg_id()    const { return UBX_ID_ESF_MEAS; }

bool esf_meas_decoder::decode(const ubx_frame& frame)
{
    // Minimum valid payload: 8-byte header.
    if (frame.header.payload_length < UBX_ESF_MEAS_MIN_PAYLOAD_LEN)
        return false;

    const uint16_t payload_len  = frame.header.payload_length;
    const uint16_t data_bytes   = static_cast<uint16_t>(payload_len - UBX_ESF_MEAS_MIN_PAYLOAD_LEN);

    // The calibTtag is present when the time-tag type field in flags != 0.
    // We detect it by checking if the remaining data bytes are not evenly
    // divisible by 4 (it is always last 4 bytes when present).
    // Per spec: payload length = 8 + numMeas*4 [+ 4 for calibTtag].
    // We determine presence by whether flags bits[1:0] != 0 after reading header.

    internal::byte_reader r(frame.payload);

    ubx_esf_meas msg{};

    // ── Bytes 0-3: timeTag ───────────────────────────────────────────────────
    if (!r.read_le32(msg.time_tag)) return false;

    // ── Bytes 4-5: flags ─────────────────────────────────────────────────────
    if (!r.read_le16(msg.flags))    return false;

    // ── Bytes 6-7: id ────────────────────────────────────────────────────────
    if (!r.read_le16(msg.id))       return false;

    // Determine number of measurement data words.
    // calibTtag is present when flags bits[1:0] != 0.
    const bool has_calib_ttag = ((msg.flags & ubx_esf_meas::FLAGS_TIME_TAG_TYPE_MASK) != 0u);
    const uint16_t calib_bytes = has_calib_ttag ? 4u : 0u;

    if (data_bytes < calib_bytes)
        return false;

    const uint16_t meas_bytes = static_cast<uint16_t>(data_bytes - calib_bytes);
    if ((meas_bytes % 4u) != 0u)
        return false;

    const uint16_t num_meas = static_cast<uint16_t>(meas_bytes / 4u);
    // Cap at maximum to prevent OOB writes.
    const uint16_t decode_count = (num_meas <= UBX_ESF_MEAS_MAX_DATA)
                                  ? num_meas
                                  : UBX_ESF_MEAS_MAX_DATA;
    msg.num_meas = static_cast<uint8_t>(decode_count);

    // ── Measurement data words ────────────────────────────────────────────────
    for (uint16_t i = 0u; i < decode_count; ++i)
    {
        uint32_t word = 0u;
        if (!r.read_le32(word)) return false;

        msg.data[i].data_type = static_cast<uint8_t>((word >> 24u) & 0x3Fu);

        // Sign-extend 24-bit data value to int32_t.
        const uint32_t raw24 = word & 0x00FFFFFFu;
        if (raw24 & 0x00800000u)
        {
            // Negative: extend sign bit.
            msg.data[i].data_value = static_cast<int32_t>(raw24 | 0xFF000000u);
        }
        else
        {
            msg.data[i].data_value = static_cast<int32_t>(raw24);
        }
    }

    // Skip any excess measurement words beyond the cap.
    const uint16_t skip_count = num_meas - decode_count;
    if (skip_count > 0u)
    {
        if (!r.skip(static_cast<std::size_t>(skip_count) * 4u)) return false;
    }

    // ── Optional calibTtag ───────────────────────────────────────────────────
    msg.has_calib_ttag = has_calib_ttag;
    if (has_calib_ttag)
    {
        if (!r.read_le32(msg.calib_ttag)) return false;
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
