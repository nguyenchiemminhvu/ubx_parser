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
// messages/ubx_sec_sig.h
//
// Decoded representation of the UBX-SEC-SIG message
// (class 0x27, ID 0x09).
//
// UBX-SEC-SIG reports signal security information relating to the availability
// and integrity of GNSS signals, including jamming/interference detection state
// and spoofing detection state.
//
// Payload layout (12 bytes, version 0x01):
//
//   Offset  Type     Name         Description
//   ──────  ──────   ──────────   ─────────────────────────────────────────
//     0     U1       version      Message version (0x01 for this version)
//     1     U1[3]    reserved0    Reserved
//     4     X1       jam_flags    Jamming/interference flags byte
//                     bit 0        jam_det_enabled  (jamming detection enabled)
//                     bits 2..1    jamming_state    (0=Unknown, 1=OK, 2=Warning, 3=Critical)
//     5     U1[3]    reserved1    Reserved
//     8     X1       spf_flags    Spoofing flags byte
//                     bit 0        spf_det_enabled  (spoofing detection enabled)
//                     bits 3..1    spoofing_state   (0=Unknown, 1=None, 2=Indicated, 3=Affirmed)
//     9     U1[3]    reserved2    Reserved
//
// Reference: u-blox F9 DBD 1.30 Interface Description UBX-23006360,
//            Section UBX-SEC-SIG.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── ubx_sec_sig ──────────────────────────────────────────────────────────────

struct ubx_sec_sig
{
    uint8_t version;           ///< Message version (0x01)

    uint8_t jam_flags;         ///< Raw jamFlags byte (X1)
    uint8_t jam_det_enabled;   ///< bit 0 of jamFlags: jamming detection enabled (0/1)
    uint8_t jamming_state;     ///< bits 2..1 of jamFlags: 0=Unknown, 1=OK, 2=Warning, 3=Critical

    uint8_t spf_flags;         ///< Raw spfFlags byte (X1)
    uint8_t spf_det_enabled;   ///< bit 0 of spfFlags: spoofing detection enabled (0/1)
    uint8_t spoofing_state;    ///< bits 3..1 of spfFlags: 0=Unknown, 1=None, 2=Indicated, 3=Affirmed
};

/// Expected fixed payload length for UBX-SEC-SIG (version 0x01).
static constexpr uint16_t UBX_SEC_SIG_PAYLOAD_LEN = 12u;

/// Callback for decoded UBX-SEC-SIG messages.
using sec_sig_callback = std::function<void(const ubx_sec_sig&)>;

} // namespace parser
} // namespace ubx
