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
// ubx_cfg_rst_builder.h
//
// Builder for UBX-CFG-RST reset command frames.
//
// UBX-CFG-RST (class 0x06, ID 0x04) is a 4-byte command that instructs the
// u-blox receiver to perform a GNSS reset with configurable BBR clearing:
//
//   Payload layout (4 bytes):
//     [0-1]  navBbrMask  U2  Which BBR data to clear before reset
//     [2]    resetMode   U1  Reset type (0x01 = controlled software reset)
//     [3]    reserved1   U1  Reserved (always 0x00)
//
//   navBbrMask values (see ubx_types.h):
//     NAV_BBR_HOT_START   0x0000  – retain all aiding data
//     NAV_BBR_WARM_START  0x0001  – discard ephemeris only
//     NAV_BBR_COLD_START  0xFFFF  – erase all BBR data
//
// Reference: u-blox ZED-F9R Interface Description UBX-19056845,
//            section UBX-CFG-RST.
//
// Thread-safety:
//   All methods are static and produce independent vectors.  Instances of
//   this class are not needed; the class is a pure namespace grouping.

#pragma once

#include "ubx_types.h"
#include <cstdint>
#include <vector>

namespace ubx
{
namespace parser
{

// ─── ubx_cfg_rst_builder ──────────────────────────────────────────────────────
//
// Provides static factory methods that produce fully framed, checksummed
// UBX-CFG-RST byte vectors ready to be written to the GNSS UART.
//
// Usage:
//   auto frame = ubx_cfg_rst_builder::build_hot_start();
//   serial_port.write(frame.data(), frame.size());

class ubx_cfg_rst_builder
{
public:
    // Non-instantiable: all methods are static.
    ubx_cfg_rst_builder()  = delete;
    ~ubx_cfg_rst_builder() = delete;

    // ── Factory methods ───────────────────────────────────────────────────────

    /// Build a UBX-CFG-RST hot-stop frame.
    ///
    /// navBbrMask = 0x0000 (all aiding retained), resetMode = 0x00 (controlled
    /// software stop).  The receiver performs a controlled software stop, but
    /// retains all aiding data.  This is not a reset, but it can be used to
    /// stop the receiver while preserving aiding data for a subsequent hot start.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_hot_stop();

    /// Build a UBX-CFG-RST warm-stop frame.
    ///
    /// navBbrMask = 0x0001 (ephemeris cleared), resetMode = 0x00 (controlled
    /// software stop).  The receiver performs a controlled software stop, but
    /// discards ephemeris.  Almanac, position, time, and clock data are retained.
    /// This is not a reset, but it can be used to stop the receiver while preserving
    /// non-ephemeris aiding data for a subsequent warm start.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_warm_stop();

    /// Build a UBX-CFG-RST cold-stop frame.
    ///
    /// navBbrMask = 0xFFFF (all BBR cleared), resetMode = 0x00 (controlled
    /// software stop).  The receiver performs a controlled software stop, but
    /// discards all aiding data.  This is not a reset, but it can be used to
    /// stop the receiver while preserving no aiding data for a subsequent cold start.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_cold_stop();

    /// Build a UBX-CFG-RST hot-start frame.
    ///
    /// navBbrMask = 0x0000 (all aiding retained), resetMode = 0x01 (controlled
    /// software reset).  The receiver restarts GNSS acquisition using all
    /// previously stored navigation data.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_hot_start();

    /// Build a UBX-CFG-RST warm-start frame.
    ///
    /// navBbrMask = 0x0001 (ephemeris cleared), resetMode = 0x01 (controlled
    /// software reset).  Almanac, position, time, and clock data are retained;
    /// ephemeris is discarded, which forces re-download.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_warm_start();

    /// Build a UBX-CFG-RST cold-start frame.
    ///
    /// navBbrMask = 0xFFFF (all BBR cleared), resetMode = 0x01 (controlled
    /// software reset).  The receiver discards all aiding data and starts a
    /// full cold search.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_cold_start();

    /// Build a UBX-CFG-RST hot start frame with reset.
    ///
    /// navBbrMask = 0x0000 (all aiding retained), resetMode = 0x01 (controlled
    /// software reset).  The receiver performs a controlled software reset, but retains all aiding data.  This is effectively the same as a hot start, but it also resets the receiver's internal state machine, which can be useful for recovering from certain error conditions without losing aiding data.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_hot_reset();

    /// Build a UBX-CFG-RST warm start frame with reset.
    ///
    /// navBbrMask = 0x0001 (ephemeris cleared), resetMode = 0x01 (controlled software reset).  The receiver performs a controlled software reset, discards ephemeris, but retains almanac, position, time, and clock data.  This can be useful for recovering from certain error conditions while still forcing a re-download of ephemeris.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_warm_reset();

    /// Build a UBX-CFG-RST cold start frame with reset.
    ///
    /// navBbrMask = 0xFFFF (all BBR cleared), resetMode = 0x01 (controlled software reset).  The receiver performs a controlled software reset, discards all aiding data, and starts a full cold search.  This can be useful for recovering from certain error conditions when a full reset is desired.
    ///
    /// @returns Fully framed, checksummed byte vector (12 bytes total).
    static std::vector<uint8_t> build_cold_reset();

    /// Common helper: assemble payload and delegate to ubx_message_builder.
    static std::vector<uint8_t> build(uint16_t nav_bbr_mask, uint8_t reset_mode);
};

} // namespace parser
} // namespace ubx
