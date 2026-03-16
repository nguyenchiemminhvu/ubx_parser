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

#include "ubx_dispatcher.h"
#include <utility>

namespace ubx
{
namespace parser
{

ubx_dispatcher::ubx_dispatcher(const ubx_decoder_registry& registry)
    : registry_(registry)
{}

void ubx_dispatcher::set_raw_message_callback(raw_message_callback_t cb)
{
    raw_cb_ = std::move(cb);
}

void ubx_dispatcher::set_error_callback(parse_error_callback cb)
{
    error_cb_ = std::move(cb);
}

void ubx_dispatcher::dispatch(const ubx_frame& frame)
{
    i_message_decoder* decoder = registry_.find(
        frame.header.msg_class, frame.header.msg_id);

    if (decoder)
    {
        // Known message type — call its decoder.
        decoder->decode(frame);
    }

    if (raw_cb_)
    {
        ubx_raw_message raw;
        raw.frame = frame;
        raw_cb_(raw);
    }
}

void ubx_dispatcher::dispatch_error(const parse_error_info& info)
{
    if (error_cb_)
        error_cb_(info);
}

} // namespace parser
} // namespace ubx
