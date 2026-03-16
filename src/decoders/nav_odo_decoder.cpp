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

#include "decoders/nav_odo_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

nav_odo_decoder::nav_odo_decoder(nav_odo_callback cb)
    : cb_(std::move(cb))
{}

uint8_t nav_odo_decoder::msg_class() const { return UBX_CLASS_NAV; }
uint8_t nav_odo_decoder::msg_id()    const { return UBX_ID_NAV_ODO; }

bool nav_odo_decoder::decode(const ubx_frame& frame)
{
    // UBX-NAV-ODO payload must be exactly 20 bytes.
    if (frame.header.payload_length != UBX_NAV_ODO_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_nav_odo msg{};

    // ── Byte 0: version ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))          return false;

    // ── Bytes 1-3: reserved1 (3 bytes) ───────────────────────────────────────
    if (!r.skip(3u))                      return false;

    // ── Bytes 4-7: iTOW ─────────────────────────────────────────────────────
    if (!r.read_le32(msg.i_tow))          return false;

    // ── Bytes 8-11: distance ─────────────────────────────────────────────────
    if (!r.read_le32(msg.distance))       return false;

    // ── Bytes 12-15: totalDistance ───────────────────────────────────────────
    if (!r.read_le32(msg.total_distance)) return false;

    // ── Bytes 16-19: distanceStd ─────────────────────────────────────────────
    if (!r.read_le32(msg.distance_std))   return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
