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

#include "decoders/upd_sos_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

upd_sos_decoder::upd_sos_decoder(upd_sos_output_callback cb)
    : cb_(std::move(cb))
{}

uint8_t upd_sos_decoder::msg_class() const { return UBX_CLASS_UPD; }
uint8_t upd_sos_decoder::msg_id()    const { return UBX_ID_UPD_SOS; }

bool upd_sos_decoder::decode(const ubx_frame& frame)
{
    // Reject undersized payloads.
    if (frame.header.payload_length < UBX_UPD_SOS_OUTPUT_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_upd_sos_output msg{};

    // Payload layout: [cmd(1), reserved0(3), response(1), reserved1(3)]
    if (!r.read_u8(msg.cmd))  return false;
    if (!r.skip(3))           return false;  // reserved0[3]
    if (!r.read_u8(msg.response)) return false;
    if (!r.skip(3))           return false;  // reserved1[3]

    // Only the two receiver-output sub-commands are accepted.
    // Input-command frames (cmd=0x00 save, cmd=0x01 clear) must be rejected
    // should they somehow appear in the incoming stream.
    if (msg.cmd != UPD_SOS_CMD_ACK && msg.cmd != UPD_SOS_CMD_RESTORE)
        return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
