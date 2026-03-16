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

#include "decoders/cfg_valget_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

cfg_valget_decoder::cfg_valget_decoder(cfg_valget_callback cb)
    : cb_(std::move(cb))
{}

uint8_t cfg_valget_decoder::msg_class() const { return UBX_CLASS_CFG; }
uint8_t cfg_valget_decoder::msg_id()    const { return UBX_ID_CFG_VALGET; }

uint8_t cfg_valget_decoder::value_size_from_key(uint32_t key_id)
{
    // Bits 31-28 encode the data type / size.
    //   0x1 → L  (1-bit boolean, wire size 1 byte)
    //   0x2 → U1/I1/X1/E1 (1 byte)
    //   0x3 → U2/I2/X2/E2 (2 bytes)
    //   0x4 → U4/I4/X4/E4 (4 bytes)
    //   0x5 → U8/I8/X8/R8 (8 bytes)
    switch ((key_id >> 28u) & 0x0Fu)
    {
        case 0x1u: return 1u;
        case 0x2u: return 1u;
        case 0x3u: return 2u;
        case 0x4u: return 4u;
        case 0x5u: return 8u;
        default:   return 0u;   // unknown
    }
}

bool cfg_valget_decoder::decode(const ubx_frame& frame)
{
    // Minimum payload: 4 byte header.
    if (frame.header.payload_length < UBX_CFG_VALGET_HEADER_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_cfg_valget msg{};

    // ── Header (4 bytes) ─────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))  return false;
    if (!r.read_u8(msg.layer))    return false;
    if (!r.read_le16(msg.position)) return false;

    // ── Key-value pairs ───────────────────────────────────────────────────────
    // Each pair: 4-byte key_id followed by N value bytes (N from key size code).
    while (r.remaining() >= 4u)
    {
        cfg_valget_entry entry{};

        if (!r.read_le32(entry.key_id))
            return false;

        const uint8_t vsz = value_size_from_key(entry.key_id);
        if (vsz == 0u)
        {
            // Unknown size: cannot continue parsing this response safely.
            // Deliver what was decoded so far.
            break;
        }

        if (r.remaining() < static_cast<std::size_t>(vsz))
        {
            // Truncated value — payload length inconsistency.
            return false;
        }

        entry.value = 0u;
        switch (vsz)
        {
            case 1u:
            {
                uint8_t v = 0u;
                r.read_u8(v);
                entry.value = v;
                break;
            }
            case 2u:
            {
                uint16_t v = 0u;
                r.read_le16(v);
                entry.value = v;
                break;
            }
            case 4u:
            {
                uint32_t v = 0u;
                r.read_le32(v);
                entry.value = v;
                break;
            }
            case 8u:
            {
                uint64_t v = 0u;
                r.read_le64(v);
                entry.value = v;
                break;
            }
            default:
                break;
        }

        msg.entries.push_back(entry);
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
