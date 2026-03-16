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
// decoders/inf_decoder.h
//
// Decoder for all UBX-INF-* messages (class 0x04, IDs 0x00–0x04).
//
// A single inf_decoder instance handles all five INF sub-types: ERROR,
// WARNING, NOTICE, TEST, and DEBUG.  The subtype is reported through the
// inf_subtype field of the decoded ubx_inf struct.

#pragma once

#include "decoders/i_message_decoder.h"
#include "messages/ubx_inf.h"

namespace ubx
{
namespace parser
{

// ─── inf_decoder ──────────────────────────────────────────────────────────────
//
// Decodes the variable-length text payload of any UBX-INF-* message and fires
// the registered inf_callback.
//
// Usage: register one inf_decoder per INF subtype (or one per desired severity
// level), each constructed with the target msg_id and a shared or separate cb.

class inf_decoder : public i_message_decoder
{
public:
    /// Construct for a specific INF subtype.
    /// @param id  The UBX message ID for this INF subtype (e.g. UBX_ID_INF_ERROR).
    /// @param cb  Callback invoked with the decoded ubx_inf on success.
    inf_decoder(uint8_t id, inf_callback cb);

    // i_message_decoder interface
    uint8_t msg_class() const override;
    uint8_t msg_id()    const override;
    bool    decode(const ubx_frame& frame) override;

private:
    uint8_t      id_;
    inf_callback cb_;
};

} // namespace parser
} // namespace ubx
