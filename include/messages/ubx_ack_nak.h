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
// messages/ubx_ack_nak.h
//
// Decoded representation of the UBX-ACK-NAK message (class 0x05, ID 0x00).
//
// Sent by the receiver as soon as possible (but within one second) to
// indicate that an input message was not acknowledged.
//
// Payload size: 2 bytes (fixed).
// Layout: clsID(U1), msgID(U1)
//
// Reference: u-blox Interface Description, Section UBX-ACK-NAK.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-ACK-NAK.
static constexpr uint16_t UBX_ACK_NAK_PAYLOAD_LEN = 2u;

// ─── ubx_ack_nak ──────────────────────────────────────────────────────────────
//
// Decoded UBX-ACK-NAK message.

struct ubx_ack_nak
{
    uint8_t cls_id;  ///< Class ID of the not-acknowledged message
    uint8_t msg_id;  ///< Message ID of the not-acknowledged message
};

/// Callback for decoded UBX-ACK-NAK messages.
using ack_nak_callback = std::function<void(const ubx_ack_nak&)>;

} // namespace parser
} // namespace ubx
