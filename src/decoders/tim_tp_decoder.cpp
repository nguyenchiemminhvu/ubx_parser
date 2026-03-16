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

#include "decoders/tim_tp_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

tim_tp_decoder::tim_tp_decoder(tim_tp_callback cb)
    : cb_(std::move(cb))
{}

uint8_t tim_tp_decoder::msg_class() const { return UBX_CLASS_TIM; }
uint8_t tim_tp_decoder::msg_id()    const { return UBX_ID_TIM_TP; }

bool tim_tp_decoder::decode(const ubx_frame& frame)
{
    // UBX-TIM-TP payload must be exactly 16 bytes.
    if (frame.header.payload_length != UBX_TIM_TP_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_tim_tp msg{};

    // ── Bytes 0-3: towMS ────────────────────────────────────────────────────
    if (!r.read_le32(msg.tow_ms))     return false;

    // ── Bytes 4-7: towSubMS ──────────────────────────────────────────────────
    if (!r.read_le32(msg.tow_sub_ms)) return false;

    // ── Bytes 8-11: qErr (signed) ────────────────────────────────────────────
    if (!r.read_le32s(msg.q_err))     return false;

    // ── Bytes 12-13: week ────────────────────────────────────────────────────
    if (!r.read_le16(msg.week))       return false;

    // ── Byte 14: flags ───────────────────────────────────────────────────────
    if (!r.read_u8(msg.flags))        return false;

    // ── Byte 15: refInfo ─────────────────────────────────────────────────────
    if (!r.read_u8(msg.ref_info))     return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
