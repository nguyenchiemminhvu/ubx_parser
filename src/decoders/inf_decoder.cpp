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

#include "decoders/inf_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

inf_decoder::inf_decoder(uint8_t id, inf_callback cb)
    : id_(id), cb_(std::move(cb))
{}

uint8_t inf_decoder::msg_class() const { return UBX_CLASS_INF; }
uint8_t inf_decoder::msg_id()    const { return id_; }

bool inf_decoder::decode(const ubx_frame& frame)
{
    // INF payload is a variable-length ASCII string (may be empty).
    // No minimum length restriction beyond the frame being valid.
    ubx_inf msg{};

    // Map the UBX message ID to the inf_subtype enum.
    switch (frame.header.msg_id)
    {
        case UBX_ID_INF_ERROR:   msg.subtype = inf_subtype::error;   break;
        case UBX_ID_INF_WARNING: msg.subtype = inf_subtype::warning; break;
        case UBX_ID_INF_NOTICE:  msg.subtype = inf_subtype::notice;  break;
        case UBX_ID_INF_TEST:    msg.subtype = inf_subtype::test;    break;
        case UBX_ID_INF_DEBUG:   msg.subtype = inf_subtype::debug;   break;
        default:
            // Unknown INF subtype — reject gracefully.
            return false;
    }

    // Decode the payload bytes as an ASCII string.
    // The spec does not guarantee null-termination, so copy all bytes.
    msg.text.reserve(frame.header.payload_length);
    for (uint16_t i = 0u; i < frame.header.payload_length; ++i)
    {
        const uint8_t ch = frame.payload[i];
        if (ch == 0u)
            break;  // stop at first null byte if present
        msg.text.push_back(static_cast<char>(ch));
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
