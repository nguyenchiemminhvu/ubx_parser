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

#include "decoders/ack_ack_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

ack_ack_decoder::ack_ack_decoder(ack_ack_callback cb)
    : cb_(std::move(cb))
{}

uint8_t ack_ack_decoder::msg_class() const { return UBX_CLASS_ACK; }
uint8_t ack_ack_decoder::msg_id()    const { return UBX_ID_ACK_ACK; }

bool ack_ack_decoder::decode(const ubx_frame& frame)
{
    // UBX-ACK-ACK payload must be exactly 2 bytes.
    if (frame.header.payload_length != UBX_ACK_ACK_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_ack_ack msg{};

    // ── Byte 0: clsID ────────────────────────────────────────────────────────
    if (!r.read_u8(msg.cls_id)) return false;

    // ── Byte 1: msgID ────────────────────────────────────────────────────────
    if (!r.read_u8(msg.msg_id)) return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
