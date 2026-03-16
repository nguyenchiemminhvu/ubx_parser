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
// config/ubx_config_protocol.h
//
// Header-only bridge: config_value serialisation helpers that sit on top of
// the shared ubx::parser::protocol utilities (ubx_protocol.h).
//
// These helpers are intentionally kept in their own header so that the core
// ubx_protocol.h remains free of any dependency on config-specific types.
// Only the config sub-feature files need to include this header.

#pragma once

#include "ubx_config_types.h"
#include "ubx_protocol.h"   // ubx::parser::protocol::{write_*, read_*, value_byte_size}
#include <cstdint>
#include <vector>

namespace ubx
{
namespace config
{
namespace protocol
{

// Re-export the shared helpers under the config::protocol namespace so that
// config-layer code can use a consistent qualified name without reaching into
// ubx::parser::protocol directly.

using ubx::parser::protocol::write_u8;
using ubx::parser::protocol::write_le16;
using ubx::parser::protocol::write_le32;
using ubx::parser::protocol::write_le64;
using ubx::parser::protocol::read_le16;
using ubx::parser::protocol::read_le32;
using ubx::parser::protocol::read_le64;
using ubx::parser::protocol::compute_checksum;
using ubx::parser::protocol::build_ubx_frame;
using ubx::parser::protocol::value_byte_size;

// ─── config_value serialisation ───────────────────────────────────────────────

/// Serialise a config_value into the buffer according to its wire byte size.
inline void write_config_value(std::vector<uint8_t>& buf,
                                const config_value&   val,
                                uint8_t               byte_size)
{
    switch (byte_size)
    {
        case 1u: write_u8  (buf, val.as_u8());  break;
        case 2u: write_le16(buf, val.as_u16()); break;
        case 4u: write_le32(buf, val.as_u32()); break;
        case 8u: write_le64(buf, val.raw);       break;
        default: break;
    }
}

// ─── config_value deserialisation ────────────────────────────────────────────

/// Deserialise a config_value from raw bytes according to its wire byte size.
inline config_value read_config_value(const uint8_t* p, uint8_t byte_size)
{
    config_value v;
    switch (byte_size)
    {
        case 1u: v.raw = p[0];           break;
        case 2u: v.raw = read_le16(p);   break;
        case 4u: v.raw = read_le32(p);   break;
        case 8u: v.raw = read_le64(p);   break;
        default: break;
    }
    return v;
}

} // namespace protocol
} // namespace config
} // namespace ubx
