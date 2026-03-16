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

#include "decoders/nav_dop_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

nav_dop_decoder::nav_dop_decoder(nav_dop_callback cb)
    : cb_(std::move(cb))
{}

uint8_t nav_dop_decoder::msg_class() const { return UBX_CLASS_NAV; }
uint8_t nav_dop_decoder::msg_id()    const { return UBX_ID_NAV_DOP; }

bool nav_dop_decoder::decode(const ubx_frame& frame)
{
    // UBX-NAV-DOP payload must be exactly 18 bytes.
    if (frame.header.payload_length != UBX_NAV_DOP_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_nav_dop msg{};

    // ── Bytes 0-3: iTOW ─────────────────────────────────────────────────────
    if (!r.read_le32(msg.i_tow))  return false;

    // ── Bytes 4-5: gDOP ──────────────────────────────────────────────────────
    if (!r.read_le16(msg.g_dop))  return false;

    // ── Bytes 6-7: pDOP ──────────────────────────────────────────────────────
    if (!r.read_le16(msg.p_dop))  return false;

    // ── Bytes 8-9: tDOP ──────────────────────────────────────────────────────
    if (!r.read_le16(msg.t_dop))  return false;

    // ── Bytes 10-11: vDOP ────────────────────────────────────────────────────
    if (!r.read_le16(msg.v_dop))  return false;

    // ── Bytes 12-13: hDOP ────────────────────────────────────────────────────
    if (!r.read_le16(msg.h_dop))  return false;

    // ── Bytes 14-15: nDOP ────────────────────────────────────────────────────
    if (!r.read_le16(msg.n_dop))  return false;

    // ── Bytes 16-17: eDOP ────────────────────────────────────────────────────
    if (!r.read_le16(msg.e_dop))  return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
