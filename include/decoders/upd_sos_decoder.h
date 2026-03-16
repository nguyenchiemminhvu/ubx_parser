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
// decoders/upd_sos_decoder.h
//
// Decoder for UBX-UPD-SOS backup creation acknowledge output
// (class 0x09, ID 0x14).
//
// Only the OUTPUT variant of UBX-UPD-SOS is decoded here.  The input
// commands (poll/save/clear) are built by ubx_upd_sos_builder and sent to
// the receiver; they are not expected to arrive in the incoming byte stream.
//
// The decoder validates:
//   - payload length >= UBX_UPD_SOS_ACK_PAYLOAD_LEN (8 bytes)
//   - version byte == UPD_SOS_VERSION_OUT (0x02)
//   - cmd byte   == UPD_SOS_CMD_ACK      (0x02)
// Any frame that fails these checks is silently rejected (returns false,
// callback not fired) so that the parser can re-synchronise.

#pragma once

#include "decoders/i_message_decoder.h"
#include "messages/ubx_upd_sos.h"

namespace ubx
{
namespace parser
{

// ─── upd_sos_decoder ─────────────────────────────────────────────────────────

class upd_sos_decoder : public i_message_decoder
{
public:
    explicit upd_sos_decoder(upd_sos_output_callback cb);

    uint8_t msg_class() const override;
    uint8_t msg_id()    const override;
    bool    decode(const ubx_frame& frame) override;

private:
    upd_sos_output_callback cb_;
};

} // namespace parser
} // namespace ubx
