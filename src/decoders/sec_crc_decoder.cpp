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

#include "decoders/sec_crc_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

sec_crc_decoder::sec_crc_decoder(sec_crc_callback cb)
    : cb_(std::move(cb))
{}

uint8_t sec_crc_decoder::msg_class() const { return UBX_CLASS_SEC; }
uint8_t sec_crc_decoder::msg_id()    const { return UBX_ID_SEC_CRC; }

bool sec_crc_decoder::decode(const ubx_frame& frame)
{
    // UBX-SEC-CRC fixed payload: 8 bytes.
    if (frame.header.payload_length < UBX_SEC_CRC_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_sec_crc msg{};

    if (!r.read_u8(msg.version))   return false;
    if (!r.read_u8(msg.crc_type))  return false;
    if (!r.read_le16(msg.reserved)) return false;
    if (!r.read_le32(msg.crc32))   return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
