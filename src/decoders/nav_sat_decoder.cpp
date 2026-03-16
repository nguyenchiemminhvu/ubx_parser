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

#include "decoders/nav_sat_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

nav_sat_decoder::nav_sat_decoder(nav_sat_callback cb)
    : cb_(std::move(cb))
{}

uint8_t nav_sat_decoder::msg_class() const { return UBX_CLASS_NAV; }
uint8_t nav_sat_decoder::msg_id()    const { return UBX_ID_NAV_SAT; }

bool nav_sat_decoder::decode(const ubx_frame& frame)
{
    // Minimum payload: 8-byte header.
    if (frame.header.payload_length < UBX_NAV_SAT_HEADER_LEN)
        return false;

    // Total payload must equal header + numSvs * block length.
    // We read numSvs from the header before validating the full length.
    internal::byte_reader r(frame.payload);

    ubx_nav_sat msg{};

    // ── Bytes 0-3: iTOW ─────────────────────────────────────────────────────
    if (!r.read_le32(msg.i_tow))   return false;

    // ── Byte 4: version ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))   return false;

    // ── Byte 5: numSvs ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.num_svs))   return false;

    // ── Bytes 6-7: reserved ───────────────────────────────────────────────────
    if (!r.skip(2u))               return false;

    // Validate total payload length against the declared number of SVs.
    const uint16_t expected_len =
        static_cast<uint16_t>(UBX_NAV_SAT_HEADER_LEN +
                               static_cast<uint32_t>(msg.num_svs) * UBX_NAV_SAT_BLOCK_LEN);
    if (frame.header.payload_length != expected_len)
        return false;

    // ── Per-SV blocks ─────────────────────────────────────────────────────────
    msg.svs.reserve(msg.num_svs);

    for (uint8_t i = 0u; i < msg.num_svs; ++i)
    {
        nav_sat_sv sv{};

        // Byte 0: gnssId
        if (!r.read_u8(sv.gnss_id))  return false;

        // Byte 1: svId
        if (!r.read_u8(sv.sv_id))    return false;

        // Byte 2: cno
        if (!r.read_u8(sv.cno))      return false;

        // Byte 3: elev (signed)
        {
            uint8_t raw = 0u;
            if (!r.read_u8(raw)) return false;
            sv.elev = static_cast<int8_t>(raw);
        }

        // Bytes 4-5: azim (signed)
        if (!r.read_le16s(sv.azim))  return false;

        // Bytes 6-7: prRes (signed)
        if (!r.read_le16s(sv.pr_res)) return false;

        // Bytes 8-11: flags
        if (!r.read_le32(sv.flags))  return false;

        msg.svs.push_back(sv);
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
