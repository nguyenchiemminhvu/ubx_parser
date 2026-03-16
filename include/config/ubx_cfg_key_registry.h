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
// config/ubx_cfg_key_registry.h
//
// A compile-time static lookup table that maps human-readable key name strings
// (as they appear in ubx_config_key.h) to their 32-bit key IDs.
//
// This is primarily used by INI config provider implementations to convert the
// key names read from the configuration file into the numeric IDs required by
// the VALGET / VALSET builders.
//
// Example:
//   uint32_t id;
//   if (ubx::config::ubx_cfg_key_registry::lookup_by_name("rate_meas", id))
//       // id == 0x30210001u
//
// Lookup is O(n) linear scan — called only at initialisation time.

#pragma once

#include <cstdint>
#include <cstddef>

namespace ubx
{
namespace config
{

struct key_info
{
    const char* name;
    uint32_t    key_id;
};

class ubx_cfg_key_registry
{
public:
    /// Look up a key_id by its string name (case-sensitive, snake_case).
    /// Returns true and writes to out_key_id if the name is found.
    static bool lookup_by_name(const char* name, uint32_t& out_key_id);

    /// Look up the string name for a given key ID.
    /// Returns non-null pointer to static string, or nullptr if not found.
    static const char* lookup_by_id(uint32_t key_id);

    /// Access the complete table.
    static const key_info* table();
    static std::size_t     table_size();
};

} // namespace config
} // namespace ubx
