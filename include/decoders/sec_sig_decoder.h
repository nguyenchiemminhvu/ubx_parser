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
// decoders/sec_sig_decoder.h
//
// Decoder for UBX-SEC-SIG (class 0x27, ID 0x09).

#pragma once

#include "decoders/i_message_decoder.h"
#include "messages/ubx_sec_sig.h"

namespace ubx
{
namespace parser
{

// ─── sec_sig_decoder ──────────────────────────────────────────────────────────
//
// Decodes the 12-byte fixed payload of UBX-SEC-SIG (version 0x01) and fires
// the registered sec_sig_callback.

class sec_sig_decoder : public i_message_decoder
{
public:
    /// Construct with a callback.  The callback is invoked from decode().
    /// @param cb  Callback to fire on successful decode.  May be a null
    ///            std::function — decode() will silently succeed without calling it.
    explicit sec_sig_decoder(sec_sig_callback cb);

    // i_message_decoder interface
    uint8_t msg_class() const override;
    uint8_t msg_id()    const override;
    bool    decode(const ubx_frame& frame) override;

private:
    sec_sig_callback cb_;
};

} // namespace parser
} // namespace ubx
