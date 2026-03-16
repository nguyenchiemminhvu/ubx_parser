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
// ubx_upd_sos_builder.h
//
// Builder for UBX-UPD-SOS (Save-on-Shutdown) command frames.
//
// UBX-UPD-SOS (class 0x09, ID 0x14) is used to manage the u-blox receiver's
// navigation data backup in flash memory.
//
//   Input messages (host → receiver):
//
//     Poll (0-byte payload):
//       Empty frame — queries the current SOS backup status.
//
//     Save/Clear (4-byte payload):
//       [0]     cmd         U1  = 0x00 (save) or 0x01 (clear)
//       [1..3]  reserved0   U1×3 = 0x00
//
//   Output messages (receiver → host) — decoded by upd_sos_decoder:
//     [0]     cmd         U1  = 0x02 (backup creation ack) or 0x03 (restore status)
//     [1..3]  reserved0   U1×3 = 0x00
//     [4]     response    U1  = 0 (not successful) or 1 (successful)
//     [5..7]  reserved1   U1×3 = 0x00
//
// Reference: u-blox ZED-F9R Interface Description UBX-19056845,
//            section UBX-UPD-SOS.
//
// Thread-safety:
//   All methods are static; no shared mutable state.

#pragma once

#include "ubx_types.h"
#include <cstdint>
#include <vector>

namespace ubx
{
namespace parser
{

// ─── ubx_upd_sos_builder ──────────────────────────────────────────────────────
//
// Provides static factory methods that produce fully framed, checksummed
// UBX-UPD-SOS byte vectors ready to be written to the GNSS UART.
//
// Usage:
//   // Trigger navigation data save before power-off:
//   auto frame = ubx_upd_sos_builder::build_save();
//   serial_port.write(frame.data(), frame.size());
//
//   // Then wait for the UBX-UPD-SOS ACK output via the parser.

class ubx_upd_sos_builder
{
public:
    // Non-instantiable: all methods are static.
    ubx_upd_sos_builder()  = delete;
    ~ubx_upd_sos_builder() = delete;

    // ── Factory methods ───────────────────────────────────────────────────────

    /// Build a UBX-UPD-SOS poll-request frame (empty payload, 8 bytes total).
    ///
    /// Sending this frame prompts the receiver to send back a
    /// UBX-UPD-SOS backup creation acknowledge output frame describing
    /// its current SOS status.
    ///
    /// @returns Fully framed, checksummed byte vector.
    static std::vector<uint8_t> build_poll();

    /// Build a UBX-UPD-SOS save-backup frame.
    ///
    /// Instructs the receiver to save current navigation data to flash
    /// for use as a SOS (Save-on-Shutdown) backup.  The receiver responds
    /// with a UBX-UPD-SOS backup creation acknowledge output frame.
    ///
    /// Payload: version=0x00, cmd=0x00, reserved=0x00, reserved=0x00
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_save();

    /// Build a UBX-UPD-SOS clear-backup frame.
    ///
    /// Instructs the receiver to erase the SOS backup from flash.
    ///
    /// Payload: version=0x00, cmd=0x01, reserved=0x00, reserved=0x00
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_clear();
};

} // namespace parser
} // namespace ubx
