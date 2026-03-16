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

#include "decoders/sec_sig_decoder.h"
#include "internal/byte_reader.h"
#include <utility>

namespace ubx
{
namespace parser
{

sec_sig_decoder::sec_sig_decoder(sec_sig_callback cb)
    : cb_(std::move(cb))
{}

uint8_t sec_sig_decoder::msg_class() const { return UBX_CLASS_SEC; }
uint8_t sec_sig_decoder::msg_id()    const { return UBX_ID_SEC_SIG; }

bool sec_sig_decoder::decode(const ubx_frame& frame)
{
    // UBX-SEC-SIG fixed payload: 12 bytes (version 0x01).
    if (frame.header.payload_length < UBX_SEC_SIG_PAYLOAD_LEN)
        return false;

    internal::byte_reader r(frame.payload);

    ubx_sec_sig msg{};

    // ── Byte 0: version ─────────────────────────────────────────────────────
    if (!r.read_u8(msg.version))   return false;

    // ── Bytes 1-3: reserved0 (3 bytes) ──────────────────────────────────────
    if (!r.skip(3u))               return false;

    // ── Byte 4: jamFlags ────────────────────────────────────────────────────
    if (!r.read_u8(msg.jam_flags)) return false;
    msg.jam_det_enabled = (msg.jam_flags >> 0u) & 0x01u;  ///< bit 0
    msg.jamming_state   = (msg.jam_flags >> 1u) & 0x03u;  ///< bits 2..1

    // ── Bytes 5-7: reserved1 (3 bytes) ──────────────────────────────────────
    if (!r.skip(3u))               return false;

    // ── Byte 8: spfFlags ────────────────────────────────────────────────────
    if (!r.read_u8(msg.spf_flags)) return false;
    msg.spf_det_enabled  = (msg.spf_flags >> 0u) & 0x01u;  ///< bit 0
    msg.spoofing_state   = (msg.spf_flags >> 1u) & 0x07u;  ///< bits 3..1

    // ── Bytes 9-11: reserved2 (3 bytes) ─────────────────────────────────────
    if (!r.skip(3u))               return false;

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
