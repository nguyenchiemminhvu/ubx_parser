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

#include "decoders/mon_ver_decoder.h"
#include "internal/byte_reader.h"
#include <utility>
#include <cstring>

namespace ubx
{
namespace parser
{

mon_ver_decoder::mon_ver_decoder(mon_ver_callback cb)
    : cb_(std::move(cb))
{}

uint8_t mon_ver_decoder::msg_class() const { return UBX_CLASS_MON; }
uint8_t mon_ver_decoder::msg_id()    const { return UBX_ID_MON_VER; }

// ─── helpers ──────────────────────────────────────────────────────────────────

namespace
{

/// Extract a null-terminated string from exactly `field_len` bytes.
/// Reads `field_len` bytes from the reader and returns the printable portion.
/// Returns false if not enough bytes remain.
bool read_fixed_string(internal::byte_reader& r,
                       std::size_t field_len,
                       std::string& out)
{
    if (r.remaining() < field_len) return false;

    // Advance position manually by reading byte-by-byte.
    std::string tmp;
    tmp.reserve(field_len);

    for (std::size_t i = 0u; i < field_len; ++i)
    {
        uint8_t ch = 0u;
        if (!r.read_u8(ch)) return false;
        if (ch != 0u)
            tmp.push_back(static_cast<char>(ch));
    }

    out = std::move(tmp);
    return true;
}

} // anonymous namespace

// ─── decode ───────────────────────────────────────────────────────────────────

bool mon_ver_decoder::decode(const ubx_frame& frame)
{
    // Minimum payload: swVersion(30) + hwVersion(10) = 40 bytes.
    if (frame.header.payload_length < UBX_MON_VER_MIN_PAYLOAD_LEN)
        return false;

    // Remaining bytes beyond the minimum must be a multiple of 30 (extension fields).
    const uint16_t extra = static_cast<uint16_t>(
        frame.header.payload_length - UBX_MON_VER_MIN_PAYLOAD_LEN);
    if (extra % UBX_MON_VER_EXT_LEN != 0u)
        return false;

    const uint16_t num_ext = extra / UBX_MON_VER_EXT_LEN;

    internal::byte_reader r(frame.payload);

    ubx_mon_ver msg{};

    // ── Bytes 0-29: swVersion ─────────────────────────────────────────────────
    if (!read_fixed_string(r, UBX_MON_VER_SW_LEN, msg.sw_version))
        return false;

    // ── Bytes 30-39: hwVersion ────────────────────────────────────────────────
    if (!read_fixed_string(r, UBX_MON_VER_HW_LEN, msg.hw_version))
        return false;

    // ── Extension fields (30 bytes each) ──────────────────────────────────────
    msg.extensions.reserve(num_ext);
    for (uint16_t i = 0u; i < num_ext; ++i)
    {
        std::string ext;
        if (!read_fixed_string(r, UBX_MON_VER_EXT_LEN, ext))
            return false;
        msg.extensions.push_back(std::move(ext));
    }

    if (cb_)
        cb_(msg);

    return true;
}

} // namespace parser
} // namespace ubx
