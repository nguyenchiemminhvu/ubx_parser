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

#include "decoders/nav_eell_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

nav_eell_decoder::nav_eell_decoder(nav_eell_callback cb)
    : cb_(std::move(cb))
{}

uint8_t nav_eell_decoder::msg_class() const { return UBX_CLASS_NAV; }
uint8_t nav_eell_decoder::msg_id()    const { return UBX_ID_NAV_EELL; }

bool nav_eell_decoder::decode(const ubx_frame& frame)
{
    // UBX-NAV-EELL payload must be exactly 16 bytes.
    if (frame.header.payload_length != UBX_NAV_EELL_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_nav_eell msg{};

    // ── Bytes 0-3: iTOW ─────────────────────────────────────────────────────
    if (!r.read_le32(msg.i_tow))      return false;

    // ── Byte 4: version ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))      return false;

    // ── Bytes 5-7: reserved1 (3 bytes) ───────────────────────────────────────
    if (!r.skip(3u))                  return false;

    // ── Bytes 8-9: errMaj ────────────────────────────────────────────────────
    if (!r.read_le16(msg.err_maj))    return false;

    // ── Bytes 10-11: errMin ──────────────────────────────────────────────────
    if (!r.read_le16(msg.err_min))    return false;

    // ── Bytes 12-13: errOrient ───────────────────────────────────────────────
    if (!r.read_le16(msg.err_orient)) return false;

    // ── Bytes 14-15: reserved2 (2 bytes) ─────────────────────────────────────
    if (!r.skip(2u))                  return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
