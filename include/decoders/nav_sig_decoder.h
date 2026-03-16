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
// decoders/nav_sig_decoder.h
//
// Decoder for UBX-NAV-SIG (class 0x01, ID 0x43).

#pragma once

#include "decoders/i_message_decoder.h"
#include "messages/ubx_nav_sig.h"

namespace ubx
{
namespace parser
{

// ─── nav_sig_decoder ──────────────────────────────────────────────────────────
//
// Decodes the variable-length payload of UBX-NAV-SIG and fires the registered
// nav_sig_callback for each successfully decoded frame.

class nav_sig_decoder : public i_message_decoder
{
public:
    /// Construct with a callback invoked on successful decode.
    explicit nav_sig_decoder(nav_sig_callback cb);

    // i_message_decoder interface
    uint8_t msg_class() const override;
    uint8_t msg_id()    const override;
    bool    decode(const ubx_frame& frame) override;

private:
    nav_sig_callback cb_;
};

} // namespace parser
} // namespace ubx
