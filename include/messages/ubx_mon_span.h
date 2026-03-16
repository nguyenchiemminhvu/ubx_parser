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
// messages/ubx_mon_span.h
//
// Decoded representation of the UBX-MON-SPAN message (class 0x0A, ID 0x31).
//
// Reference: u-blox F9 Interface Description, Section UBX-MON-SPAN.
// Payload size: 4 + numRfBlocks × 272 bytes (variable).
//
// Fixed header (4 bytes):
//   version    (U1)    : Message version (0x00)
//   numRfBlocks(U1)    : Number of RF blocks
//   reserved0  (U1[2]) : Reserved
//
// Per-RF-block (272 bytes × numRfBlocks):
//   spectrum   (U1[256]) : Spectrum amplitude data (scale 2^-2 dB)
//   span       (U4)      : Spectrum span [Hz]
//   res        (U4)      : Frequency bin resolution [Hz]
//   center     (U4)      : Center frequency [Hz]
//   pga        (U1)      : Programmable gain amplifier value [dB]
//   reserved1  (U1[3])   : Reserved

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Number of spectrum amplitude bins per RF block.
static constexpr uint16_t UBX_MON_SPAN_SPECTRUM_BINS = 256u;
/// Maximum number of RF blocks in a single UBX-MON-SPAN message.
static constexpr uint8_t  UBX_MON_SPAN_MAX_RF_BLOCKS = 4u;
/// Fixed header length within UBX-MON-SPAN payload.
static constexpr uint8_t  UBX_MON_SPAN_HEADER_LEN    = 4u;
/// Per-RF-block length within UBX-MON-SPAN payload.
static constexpr uint16_t UBX_MON_SPAN_BLOCK_LEN     = 272u;

// ─── mon_span_rf_block ────────────────────────────────────────────────────────

struct mon_span_rf_block
{
    uint8_t  spectrum[UBX_MON_SPAN_SPECTRUM_BINS]; ///< Amplitude bins (2^-2 dB scale)
    uint32_t span;                                  ///< Spectrum span [Hz]
    uint32_t res;                                   ///< Frequency bin resolution [Hz]
    uint32_t center;                                ///< Center frequency [Hz]
    uint8_t  pga;                                   ///< Programmable gain amplifier [dB]
};

// ─── ubx_mon_span ─────────────────────────────────────────────────────────────

struct ubx_mon_span
{
    uint8_t          version;                             ///< Message version (0x00)
    uint8_t          num_rf_blocks;                       ///< Number of RF blocks in payload
    mon_span_rf_block rf_blocks[UBX_MON_SPAN_MAX_RF_BLOCKS]; ///< Per-RF data; valid: [0, num_rf_blocks)
};

/// Callback for decoded UBX-MON-SPAN messages.
using mon_span_callback = std::function<void(const ubx_mon_span&)>;

} // namespace parser
} // namespace ubx
