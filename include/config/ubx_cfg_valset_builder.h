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
// config/ubx_cfg_valset_builder.h
//
// Builds binary UBX-CFG-VALSET messages.
//
// UBX-CFG-VALSET payload layout:
//   Byte  0     : version   = 0x00  (UBX_VALSET_VERSION from ubx_types.h)
//   Byte  1     : layers    = bitmask (RAM=0x01, BBR=0x02, FLASH=0x04)
//   Bytes 2-3   : reserved  = 0x0000
//   Bytes 4+    : key-value pairs
//                   key   : 4 bytes little-endian (uint32_t)
//                   value : N bytes little-endian, size from key ID bits 31-28
//
// Multiple key-value pairs can be packed into a single message.

#pragma once

#include "ubx_config_types.h"
#include <vector>

namespace ubx
{
namespace config
{

class ubx_cfg_valset_builder
{
public:
    ubx_cfg_valset_builder() = default;

    /// Build a complete, framed UBX-CFG-VALSET message for the provided entries.
    ///
    /// @param entries  One or more key-value pairs to write to the chip.
    /// @param layer    Target storage layer (RAM, BBR, or FLASH).
    /// @return         Framed UBX message bytes, or an empty vector if entries
    ///                 is empty or all keys have an unrecognised size code.
    std::vector<uint8_t> build(const std::vector<config_entry>& entries,
                                config_layer layer) const;
};

} // namespace config
} // namespace ubx
