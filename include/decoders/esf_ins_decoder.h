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
//
// decoders/esf_ins_decoder.h
//
// Decoder for UBX-ESF-INS (class 0x10, ID 0x15).

#pragma once

#include "decoders/i_message_decoder.h"
#include "messages/ubx_esf_ins.h"

namespace ubx
{
namespace parser
{

// ─── esf_ins_decoder ──────────────────────────────────────────────────────────
//
// Decodes the 36-byte fixed payload of UBX-ESF-INS and fires the registered
// esf_ins_callback.

class esf_ins_decoder : public i_message_decoder
{
public:
    explicit esf_ins_decoder(esf_ins_callback cb);

    uint8_t msg_class() const override;
    uint8_t msg_id()    const override;
    bool    decode(const ubx_frame& frame) override;

private:
    esf_ins_callback cb_;
};

} // namespace parser
} // namespace ubx
