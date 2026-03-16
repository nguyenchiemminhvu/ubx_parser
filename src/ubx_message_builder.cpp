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

#include "ubx_message_builder.h"
#include "ubx_protocol.h"

namespace ubx
{
namespace parser
{

ubx_message_builder& ubx_message_builder::set_class(uint8_t msg_class)
{
    msg_class_ = msg_class;
    return *this;
}

ubx_message_builder& ubx_message_builder::set_id(uint8_t msg_id)
{
    msg_id_ = msg_id;
    return *this;
}

ubx_message_builder& ubx_message_builder::set_payload(const std::vector<uint8_t>& payload)
{
    payload_ = payload;
    return *this;
}

ubx_message_builder& ubx_message_builder::append_u8(uint8_t v)
{
    payload_.push_back(v);
    return *this;
}

ubx_message_builder& ubx_message_builder::append_le16(uint16_t v)
{
    protocol::write_le16(payload_, v);
    return *this;
}

ubx_message_builder& ubx_message_builder::append_le32(uint32_t v)
{
    protocol::write_le32(payload_, v);
    return *this;
}

ubx_message_builder& ubx_message_builder::clear_payload()
{
    payload_.clear();
    return *this;
}

std::vector<uint8_t> ubx_message_builder::build() const
{
    return protocol::build_ubx_frame(msg_class_, msg_id_, payload_);
}

// ── static factory helpers ────────────────────────────────────────────────────

std::vector<uint8_t> ubx_message_builder::make_frame(
    uint8_t  msg_class,
    uint8_t  msg_id,
    const std::vector<uint8_t>& payload)
{
    return protocol::build_ubx_frame(msg_class, msg_id, payload);
}

std::vector<uint8_t> ubx_message_builder::make_frame(uint8_t msg_class, uint8_t msg_id)
{
    return protocol::build_ubx_frame(msg_class, msg_id, {});
}

} // namespace parser
} // namespace ubx
