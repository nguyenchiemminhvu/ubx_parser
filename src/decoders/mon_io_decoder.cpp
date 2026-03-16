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
// Decoder for UBX-MON-IO (class 0x0A, ID 0x02).
//
// Payload layout: numPorts × 20-byte blocks (no fixed header).
// numPorts is inferred from payload_length / UBX_MON_IO_BLOCK_LEN.
//
// Per-port block (20 bytes):
//   Bytes  0-3:  rxBytes    (U4)
//   Bytes  4-7:  txBytes    (U4)
//   Bytes  8-9:  parityErrs (U2)
//   Bytes 10-11: framingErrs(U2)
//   Bytes 12-13: overrunErrs(U2)
//   Bytes 14-15: breakCond  (U2)
//   Byte  16:    rxBusy     (U1)
//   Byte  17:    txBusy     (U1)
//   Bytes 18-19: reserved1  (U1[2])

#include "decoders/mon_io_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

mon_io_decoder::mon_io_decoder(mon_io_callback cb)
    : cb_(std::move(cb))
{}

uint8_t mon_io_decoder::msg_class() const { return UBX_CLASS_MON; }
uint8_t mon_io_decoder::msg_id()    const { return UBX_ID_MON_IO; }

bool mon_io_decoder::decode(const ubx_frame& frame)
{
    const uint16_t payload_len = frame.header.payload_length;

    // Payload must be a non-zero multiple of the block size.
    if (payload_len == 0u || (payload_len % UBX_MON_IO_BLOCK_LEN) != 0u)
        return false;

    const uint16_t num_ports = static_cast<uint16_t>(payload_len / UBX_MON_IO_BLOCK_LEN);

    internal::byte_reader r(frame.payload);

    ubx_mon_io msg{};

    // Cap at maximum to prevent OOB writes.
    const uint8_t decode_count = (num_ports <= UBX_MON_IO_MAX_PORTS)
                                 ? static_cast<uint8_t>(num_ports)
                                 : UBX_MON_IO_MAX_PORTS;
    msg.num_ports = decode_count;

    for (uint8_t i = 0u; i < decode_count; ++i)
    {
        mon_io_port& p = msg.ports[i];

        // Bytes 0-3: rxBytes
        if (!r.read_le32(p.rx_bytes))     return false;

        // Bytes 4-7: txBytes
        if (!r.read_le32(p.tx_bytes))     return false;

        // Bytes 8-9: parityErrs
        if (!r.read_le16(p.parity_errs))  return false;

        // Bytes 10-11: framingErrs
        if (!r.read_le16(p.framing_errs)) return false;

        // Bytes 12-13: overrunErrs
        if (!r.read_le16(p.overrun_errs)) return false;

        // Bytes 14-15: breakCond
        if (!r.read_le16(p.break_cond))   return false;

        // Byte 16: rxBusy
        if (!r.read_u8(p.rx_busy))        return false;

        // Byte 17: txBusy
        if (!r.read_u8(p.tx_busy))        return false;

        // Bytes 18-19: reserved1
        if (!r.skip(2u))                  return false;
    }

    // Skip any excess port blocks beyond the cap.
    if (num_ports > decode_count)
    {
        const uint16_t skip_bytes = static_cast<uint16_t>(
            (num_ports - decode_count) * UBX_MON_IO_BLOCK_LEN);
        if (!r.skip(skip_bytes))
            return false;
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
