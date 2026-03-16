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
// messages/ubx_mon_io.h
//
// Decoded representation of the UBX-MON-IO message (class 0x0A, ID 0x02).
//
// Reference: u-blox F9 DBD 1.30 Interface Description, Section UBX-MON-IO.
// Payload size: 0 + 20 × numPorts bytes (variable, no fixed header).
//
// Per-port block (20 bytes × numPorts):
//   rxBytes    (U4) : Number of bytes ever received
//   txBytes    (U4) : Number of bytes ever sent
//   parityErrs (U2) : Number of parity errors
//   framingErrs(U2) : Number of framing errors
//   overrunErrs(U2) : Number of overrun errors
//   breakCond  (U2) : Number of break conditions
//   rxBusy     (U1) : Receive busy flag
//   txBusy     (U1) : Transmit busy flag
//   reserved1  (U1[2])

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Maximum number of ports in a single UBX-MON-IO message.
static constexpr uint8_t  UBX_MON_IO_MAX_PORTS    = 8u;
/// Per-port block length within UBX-MON-IO payload.
static constexpr uint16_t UBX_MON_IO_BLOCK_LEN    = 20u;

// ─── mon_io_port ─────────────────────────────────────────────────────────────

struct mon_io_port
{
    uint32_t rx_bytes;      ///< Number of bytes ever received
    uint32_t tx_bytes;      ///< Number of bytes ever sent
    uint16_t parity_errs;   ///< Number of parity errors
    uint16_t framing_errs;  ///< Number of framing errors
    uint16_t overrun_errs;  ///< Number of overrun errors
    uint16_t break_cond;    ///< Number of break conditions
    uint8_t  rx_busy;       ///< Receive busy flag
    uint8_t  tx_busy;       ///< Transmit busy flag
};

// ─── ubx_mon_io ──────────────────────────────────────────────────────────────

struct ubx_mon_io
{
    uint8_t     num_ports;                    ///< Number of port blocks in payload
    mon_io_port ports[UBX_MON_IO_MAX_PORTS];  ///< Per-port data; valid: [0, num_ports)
};

/// Callback for decoded UBX-MON-IO messages.
using mon_io_callback = std::function<void(const ubx_mon_io&)>;

} // namespace parser
} // namespace ubx
