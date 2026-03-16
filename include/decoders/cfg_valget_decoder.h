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
// decoders/cfg_valget_decoder.h
//
// Decoder for UBX-CFG-VALGET response (class 0x06, ID 0x8B).

#pragma once

#include "decoders/i_message_decoder.h"
#include "messages/ubx_cfg_valget.h"

namespace ubx
{
namespace parser
{

// ─── cfg_valget_decoder ───────────────────────────────────────────────────────
//
// Decodes the variable-length payload of UBX-CFG-VALGET (version 0x01 response)
// and fires the registered cfg_valget_callback.
//
// The payload structure is:
//   [version:1] [layer:1] [position:2] [key_id:4 value:N] ...
// where N is determined by bits 31-28 of the key_id (1, 2, 4, or 8 bytes).

class cfg_valget_decoder : public i_message_decoder
{
public:
    explicit cfg_valget_decoder(cfg_valget_callback cb);

    uint8_t msg_class() const override;
    uint8_t msg_id()    const override;
    bool    decode(const ubx_frame& frame) override;

private:
    cfg_valget_callback cb_;

    /// Extract the value byte size from bits 31-28 of key_id.
    /// Returns 0 for unknown size codes.
    static uint8_t value_size_from_key(uint32_t key_id);
};

} // namespace parser
} // namespace ubx
