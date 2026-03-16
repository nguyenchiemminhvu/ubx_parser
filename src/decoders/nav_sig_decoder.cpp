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

#include "decoders/nav_sig_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

nav_sig_decoder::nav_sig_decoder(nav_sig_callback cb)
    : cb_(std::move(cb))
{}

uint8_t nav_sig_decoder::msg_class() const { return UBX_CLASS_NAV; }
uint8_t nav_sig_decoder::msg_id()    const { return UBX_ID_NAV_SIG; }

bool nav_sig_decoder::decode(const ubx_frame& frame)
{
    // Minimum payload: 8-byte header.
    if (frame.header.payload_length < UBX_NAV_SIG_HEADER_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_nav_sig msg{};

    // ── Bytes 0-3: iTOW ─────────────────────────────────────────────────────
    if (!r.read_le32(msg.i_tow))    return false;

    // ── Byte 4: version ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))    return false;

    // ── Byte 5: numSigs ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.num_sigs))   return false;

    // ── Bytes 6-7: reserved ───────────────────────────────────────────────────
    if (!r.skip(2u))                return false;

    // Validate total payload length against the declared number of signals.
    const uint16_t expected_len =
        static_cast<uint16_t>(UBX_NAV_SIG_HEADER_LEN +
                               static_cast<uint32_t>(msg.num_sigs) * UBX_NAV_SIG_BLOCK_LEN);
    if (frame.header.payload_length != expected_len)
        return false;

    // ── Per-signal blocks ─────────────────────────────────────────────────────
    msg.signals.reserve(msg.num_sigs);

    for (uint8_t i = 0u; i < msg.num_sigs; ++i)
    {
        nav_sig_info sig{};

        // Byte 0: gnssId
        if (!r.read_u8(sig.gnss_id))     return false;

        // Byte 1: svId
        if (!r.read_u8(sig.sv_id))       return false;

        // Byte 2: sigId
        if (!r.read_u8(sig.sig_id))      return false;

        // Byte 3: freqId
        if (!r.read_u8(sig.freq_id))     return false;

        // Bytes 4-5: prRes (signed)
        if (!r.read_le16s(sig.pr_res))   return false;

        // Byte 6: cno
        if (!r.read_u8(sig.cno))         return false;

        // Byte 7: qualInd
        if (!r.read_u8(sig.qual_ind))    return false;

        // Byte 8: corrSource
        if (!r.read_u8(sig.corr_source)) return false;

        // Byte 9: ionoModel
        if (!r.read_u8(sig.iono_model))  return false;

        // Bytes 10-11: sigFlags
        if (!r.read_le16(sig.sig_flags)) return false;

        // Bytes 12-15: reserved
        if (!r.skip(4u))                 return false;

        msg.signals.push_back(sig);
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
