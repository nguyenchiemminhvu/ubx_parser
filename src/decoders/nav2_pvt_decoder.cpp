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

#include "decoders/nav2_pvt_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

nav2_pvt_decoder::nav2_pvt_decoder(nav2_pvt_callback cb)
    : cb_(std::move(cb))
{}

uint8_t nav2_pvt_decoder::msg_class() const { return UBX_CLASS_NAV2; }
uint8_t nav2_pvt_decoder::msg_id()    const { return UBX_ID_NAV2_PVT; }

bool nav2_pvt_decoder::decode(const ubx_frame& frame)
{
    // UBX-NAV2-PVT payload must be exactly 92 bytes.
    if (frame.header.payload_length != UBX_NAV2_PVT_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_nav2_pvt msg{};

    // ── Bytes 0-3: iTOW ─────────────────────────────────────────────────────
    if (!r.read_le32(msg.i_tow))   return false;

    // ── Bytes 4-5: year ─────────────────────────────────────────────────────
    if (!r.read_le16(msg.year))    return false;

    // ── Byte 6: month ────────────────────────────────────────────────────────
    if (!r.read_u8(msg.month))     return false;

    // ── Byte 7: day ──────────────────────────────────────────────────────────
    if (!r.read_u8(msg.day))       return false;

    // ── Byte 8: hour ─────────────────────────────────────────────────────────
    if (!r.read_u8(msg.hour))      return false;

    // ── Byte 9: min ──────────────────────────────────────────────────────────
    if (!r.read_u8(msg.min))       return false;

    // ── Byte 10: sec ─────────────────────────────────────────────────────────
    if (!r.read_u8(msg.sec))       return false;

    // ── Byte 11: valid ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.valid))     return false;

    // ── Bytes 12-15: tAcc ────────────────────────────────────────────────────
    if (!r.read_le32(msg.t_acc))   return false;

    // ── Bytes 16-19: nano ────────────────────────────────────────────────────
    if (!r.read_le32s(msg.nano))   return false;

    // ── Byte 20: fixType ─────────────────────────────────────────────────────
    {
        uint8_t ft = 0u;
        if (!r.read_u8(ft)) return false;
        msg.fix_type = static_cast<nav2_pvt_fix_type>(ft);
    }

    // ── Byte 21: flags ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.flags))     return false;

    // ── Byte 22: flags2 ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.flags2))    return false;

    // ── Byte 23: numSV ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.num_sv))    return false;

    // ── Bytes 24-27: lon ─────────────────────────────────────────────────────
    if (!r.read_le32s(msg.lon))    return false;

    // ── Bytes 28-31: lat ─────────────────────────────────────────────────────
    if (!r.read_le32s(msg.lat))    return false;

    // ── Bytes 32-35: height ──────────────────────────────────────────────────
    if (!r.read_le32s(msg.height)) return false;

    // ── Bytes 36-39: hMSL ────────────────────────────────────────────────────
    if (!r.read_le32s(msg.h_msl))  return false;

    // ── Bytes 40-43: hAcc ────────────────────────────────────────────────────
    if (!r.read_le32(msg.h_acc))   return false;

    // ── Bytes 44-47: vAcc ────────────────────────────────────────────────────
    if (!r.read_le32(msg.v_acc))   return false;

    // ── Bytes 48-51: velN ────────────────────────────────────────────────────
    if (!r.read_le32s(msg.vel_n))  return false;

    // ── Bytes 52-55: velE ────────────────────────────────────────────────────
    if (!r.read_le32s(msg.vel_e))  return false;

    // ── Bytes 56-59: velD ────────────────────────────────────────────────────
    if (!r.read_le32s(msg.vel_d))  return false;

    // ── Bytes 60-63: gSpeed ──────────────────────────────────────────────────
    if (!r.read_le32s(msg.g_speed)) return false;

    // ── Bytes 64-67: headMot ─────────────────────────────────────────────────
    if (!r.read_le32s(msg.head_mot)) return false;

    // ── Bytes 68-71: sAcc ────────────────────────────────────────────────────
    if (!r.read_le32(msg.s_acc))   return false;

    // ── Bytes 72-75: headAcc ─────────────────────────────────────────────────
    if (!r.read_le32(msg.head_acc)) return false;

    // ── Bytes 76-77: pDOP ────────────────────────────────────────────────────
    if (!r.read_le16(msg.p_dop))   return false;

    // ── Byte 78: flags3 ──────────────────────────────────────────────────────
    if (!r.read_u8(msg.flags3))    return false;

    // ── Bytes 79-83: reserved (5 bytes) ──────────────────────────────────────
    if (!r.skip(5u))               return false;

    // ── Bytes 84-87: headVeh ─────────────────────────────────────────────────
    if (!r.read_le32s(msg.head_veh)) return false;

    // ── Bytes 88-89: magDec ──────────────────────────────────────────────────
    if (!r.read_le16s(msg.mag_dec)) return false;

    // ── Bytes 90-91: magAcc ──────────────────────────────────────────────────
    if (!r.read_le16(msg.mag_acc)) return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
