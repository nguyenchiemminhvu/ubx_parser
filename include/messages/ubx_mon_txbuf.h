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
// messages/ubx_mon_txbuf.h
//
// Decoded representation of the UBX-MON-TXBUF message (class 0x0A, ID 0x08).
//
// Reference: u-blox F9 DBD 1.30 Interface Description, Section UBX-MON-TXBUF.
// Payload size: 28 bytes (fixed).
// Layout:
//   pending[6]  (U2[6]) : Number of bytes pending in TX buffer for each target [bytes]
//   usage[6]    (U1[6]) : Maximum usage TX buffer during the last sysmon period [%]
//   peakUsage[6](U1[6]) : Maximum usage TX buffer [%]
//   tUsed       (U1)    : Number of bytes used in the transmitter buffer [%]
//   tPeak       (U1)    : Maximum tUsed [%]
//   errors      (X1)    : Error flags
//   reserved1   (U1)    : Reserved
//
// Note: 6 targets correspond to UART1, UART2, USB, SPI, DDC/I2C, RESERVED.
//
// errors bits:
//   bits [1:0] (mem)   – buffer allocation error (0x01 = alloc limit reached)
//   bit  2 (alloc)     – TX buffer allocation error

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-MON-TXBUF.
static constexpr uint16_t UBX_MON_TXBUF_PAYLOAD_LEN = 28u;

/// Number of target ports reported in UBX-MON-TXBUF.
static constexpr uint8_t  UBX_MON_TXBUF_NUM_TARGETS = 6u;

// ─── ubx_mon_txbuf ───────────────────────────────────────────────────────────

struct ubx_mon_txbuf
{
    uint16_t pending[UBX_MON_TXBUF_NUM_TARGETS];    ///< Bytes pending in TX buffer
    uint8_t  usage[UBX_MON_TXBUF_NUM_TARGETS];      ///< Current buffer usage [%]
    uint8_t  peak_usage[UBX_MON_TXBUF_NUM_TARGETS]; ///< Peak buffer usage [%]
    uint8_t  t_used;                                 ///< TX buffer usage [%]
    uint8_t  t_peak;                                 ///< Max TX buffer usage [%]
    uint8_t  errors;                                 ///< Error flags
};

/// Callback for decoded UBX-MON-TXBUF messages.
using mon_txbuf_callback = std::function<void(const ubx_mon_txbuf&)>;

} // namespace parser
} // namespace ubx
