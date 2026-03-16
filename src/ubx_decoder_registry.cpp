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

#include "ubx_decoder_registry.h"

namespace ubx
{
namespace parser
{

void ubx_decoder_registry::register_decoder(std::unique_ptr<i_message_decoder> decoder)
{
    if (!decoder)
        return;

    const key_type k = make_key(decoder->msg_class(), decoder->msg_id());
    decoders_[k] = std::move(decoder);
}

i_message_decoder* ubx_decoder_registry::find(uint8_t msg_class, uint8_t msg_id) const
{
    const key_type k = make_key(msg_class, msg_id);
    auto it = decoders_.find(k);
    if (it == decoders_.end())
        return nullptr;
    return it->second.get();
}

bool ubx_decoder_registry::has(uint8_t msg_class, uint8_t msg_id) const
{
    return find(msg_class, msg_id) != nullptr;
}

std::size_t ubx_decoder_registry::size() const
{
    return decoders_.size();
}

} // namespace parser
} // namespace ubx
