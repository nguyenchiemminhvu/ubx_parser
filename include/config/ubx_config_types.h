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
// config/ubx_config_types.h
//
// Data types for the UBX configuration sub-feature.
//
// Framing constants (UBX_SYNC_CHAR_1, UBX_CLASS_CFG, UBX_ID_CFG_VALSET, …)
// are NOT redefined here — they are inherited from ubx_types.h which is part
// of the ubx_parser shared protocol layer.
//
// All config-specific types live in the ubx::config namespace to avoid
// ambiguity when this sub-feature is used alongside the parser and database
// layers that reside in ubx::parser and ubx::database.

#pragma once

#include <cstdint>
#include <vector>

namespace ubx
{
namespace config
{

// ─── Configuration storage layers ─────────────────────────────────────────────
// These values are bit flags as defined by the UBX specification.
// Multiple layers can be combined for VALSET (e.g., ram | bbr).

enum class config_layer : uint8_t
{
    ram   = 0x01u,
    bbr   = 0x02u,
    flash = 0x04u
};

// ─── config_value ─────────────────────────────────────────────────────────────
//
// Opaque, strongly typed storage for any UBX configuration value.
// The actual byte width written to the wire is determined by the key ID
// (bits 31-28 encode the storage size). All numeric types are stored as
// their raw bit pattern in a uint64_t to enable zero-overhead comparison.

struct config_value
{
    uint64_t raw;

    config_value()                    : raw(0u) {}

    // Unsigned integer constructors
    explicit config_value(bool     v) : raw(v ? 1ull : 0ull) {}
    explicit config_value(uint8_t  v) : raw(static_cast<uint64_t>(v)) {}
    explicit config_value(uint16_t v) : raw(static_cast<uint64_t>(v)) {}
    explicit config_value(uint32_t v) : raw(static_cast<uint64_t>(v)) {}
    explicit config_value(uint64_t v) : raw(v) {}

    // Signed integer constructors — stored as two's-complement bit pattern
    explicit config_value(int8_t  v)
        : raw(static_cast<uint64_t>(static_cast<uint8_t>(v)))  {}
    explicit config_value(int16_t v)
        : raw(static_cast<uint64_t>(static_cast<uint16_t>(v))) {}
    explicit config_value(int32_t v)
        : raw(static_cast<uint64_t>(static_cast<uint32_t>(v))) {}
    explicit config_value(int64_t v)
        : raw(static_cast<uint64_t>(v)) {}

    // Typed accessors
    bool     as_bool()  const { return raw != 0u; }
    uint8_t  as_u8()    const { return static_cast<uint8_t>(raw); }
    uint16_t as_u16()   const { return static_cast<uint16_t>(raw); }
    uint32_t as_u32()   const { return static_cast<uint32_t>(raw); }
    uint64_t as_u64()   const { return raw; }
    int8_t   as_i8()    const { return static_cast<int8_t>(as_u8()); }
    int16_t  as_i16()   const { return static_cast<int16_t>(as_u16()); }
    int32_t  as_i32()   const { return static_cast<int32_t>(as_u32()); }
    int64_t  as_i64()   const { return static_cast<int64_t>(raw); }

    bool operator==(const config_value& o) const { return raw == o.raw; }
    bool operator!=(const config_value& o) const { return raw != o.raw; }
};

// ─── config_entry ─────────────────────────────────────────────────────────────
// A single key-value pair used throughout the configuration subsystem.

struct config_entry
{
    uint32_t     key_id;
    config_value value;
};

} // namespace config
} // namespace ubx
