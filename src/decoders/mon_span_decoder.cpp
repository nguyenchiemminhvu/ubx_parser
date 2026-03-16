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
// Decoder for UBX-MON-SPAN (class 0x0A, ID 0x31).
//
// Payload layout:
//   Bytes  0:     version       (U1)
//   Byte   1:     numRfBlocks   (U1)
//   Bytes  2-3:   reserved0     (U1[2])
//   Repeated numRfBlocks times (272 bytes each):
//     Bytes  0-255: spectrum    (U1[256])
//     Bytes 256-259: span       (U4)
//     Bytes 260-263: res        (U4)
//     Bytes 264-267: center     (U4)
//     Byte  268:    pga         (U1)
//     Bytes 269-271: reserved1  (U1[3])

#include "decoders/mon_span_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

mon_span_decoder::mon_span_decoder(mon_span_callback cb)
    : cb_(std::move(cb))
{}

uint8_t mon_span_decoder::msg_class() const { return UBX_CLASS_MON; }
uint8_t mon_span_decoder::msg_id()    const { return UBX_ID_MON_SPAN; }

bool mon_span_decoder::decode(const ubx_frame& frame)
{
    const uint16_t payload_len = frame.header.payload_length;

    // Minimum: 4-byte header + at least one block.
    if (payload_len < static_cast<uint16_t>(UBX_MON_SPAN_HEADER_LEN + UBX_MON_SPAN_BLOCK_LEN))
        return false;

    // Remaining length after the fixed header must be a multiple of block size.
    const uint16_t blocks_len = static_cast<uint16_t>(payload_len - UBX_MON_SPAN_HEADER_LEN);
    if ((blocks_len % UBX_MON_SPAN_BLOCK_LEN) != 0u)
        return false;

    const uint16_t num_rf_blocks = static_cast<uint16_t>(blocks_len / UBX_MON_SPAN_BLOCK_LEN);

    internal::byte_reader r(frame.payload);

    ubx_mon_span msg{};

    // Fixed header
    if (!r.read_u8(msg.version))       return false;
    if (!r.read_u8(msg.num_rf_blocks)) return false;
    if (!r.skip(2u))                   return false;  // reserved0

    // Cap decoded blocks to prevent OOB writes.
    const uint8_t decode_count = (num_rf_blocks <= UBX_MON_SPAN_MAX_RF_BLOCKS)
                                 ? static_cast<uint8_t>(num_rf_blocks)
                                 : UBX_MON_SPAN_MAX_RF_BLOCKS;
    msg.num_rf_blocks = decode_count;

    for (uint8_t i = 0u; i < decode_count; ++i)
    {
        mon_span_rf_block& blk = msg.rf_blocks[i];

        // Bytes 0-255: spectrum[256]
        for (uint16_t b = 0u; b < UBX_MON_SPAN_SPECTRUM_BINS; ++b)
        {
            if (!r.read_u8(blk.spectrum[b])) return false;
        }

        // Bytes 256-259: span
        if (!r.read_le32(blk.span))   return false;

        // Bytes 260-263: res
        if (!r.read_le32(blk.res))    return false;

        // Bytes 264-267: center
        if (!r.read_le32(blk.center)) return false;

        // Byte 268: pga
        if (!r.read_u8(blk.pga))     return false;

        // Bytes 269-271: reserved1
        if (!r.skip(3u))              return false;
    }

    // Skip any excess blocks beyond the cap.
    if (num_rf_blocks > decode_count)
    {
        const uint16_t skip_bytes = static_cast<uint16_t>(
            (num_rf_blocks - decode_count) * UBX_MON_SPAN_BLOCK_LEN);
        if (!r.skip(skip_bytes))
            return false;
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
