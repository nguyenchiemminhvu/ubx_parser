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
// messages/ubx_upd_sos.h
//
// Decoded representation of UBX-UPD-SOS output messages
// (class 0x09, ID 0x14).
//
// Two output sub-types share the same 8-byte layout:
//   cmd=0x02  Backup creation acknowledge  (UPD_SOS_CMD_ACK)
//   cmd=0x03  System restored from backup  (UPD_SOS_CMD_RESTORE)
//
// Payload layout (8 bytes):
//   [0]     cmd         U1  — 0x02 or 0x03
//   [1..3]  reserved0   U1×3 — always 0x00, not validated
//   [4]     response    U1  — 0 = not successful (UPD_SOS_RESP_NOT_ACK)
//                             1 = successful     (UPD_SOS_RESP_ACK)
//   [5..7]  reserved1   U1×3 — always 0x00, not validated
//
// Reference: u-blox ZED-F9R Interface Description UBX-19056845,
//            section UBX-UPD-SOS.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── ubx_upd_sos_output ──────────────────────────────────────────────────────
//
// Decoded UBX-UPD-SOS output message (covers both cmd=0x02 ack and
// cmd=0x03 restore-status variants).  Reserved bytes are consumed by
// the decoder but not stored here.

struct ubx_upd_sos_output
{
    /// Sub-command identifier:
    ///   UPD_SOS_CMD_ACK     (0x02) — backup creation acknowledge
    ///   UPD_SOS_CMD_RESTORE (0x03) — system restored from backup
    uint8_t cmd;

    /// Response indicator:
    ///   UPD_SOS_RESP_NOT_ACK (0) — operation not completed / not restored
    ///   UPD_SOS_RESP_ACK     (1) — operation completed / restoration confirmed
    uint8_t response;
};

/// Fixed payload length for all UBX-UPD-SOS output messages.
static constexpr uint16_t UBX_UPD_SOS_OUTPUT_PAYLOAD_LEN = 8u;

/// Callback type for decoded UBX-UPD-SOS output messages.
using upd_sos_output_callback = std::function<void(const ubx_upd_sos_output&)>;

} // namespace parser
} // namespace ubx
