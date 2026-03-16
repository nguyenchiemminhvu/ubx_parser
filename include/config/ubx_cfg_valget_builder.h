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
// config/ubx_cfg_valget_builder.h
//
// Builds binary UBX-CFG-VALGET poll-request messages.
//
// UBX-CFG-VALGET poll payload layout:
//   Byte  0     : version  = 0x00  (UBX_VALGET_VERSION_POLL from ubx_types.h)
//   Byte  1     : layer    : 0=RAM  1=BBR  2=Flash  7=Default
//   Bytes 2-3   : position = 0x0000  (entry offset for chunked responses)
//   Bytes 4+    : key IDs, 4 bytes each (little-endian uint32_t)
//
// The GNSS chip responds with a UBX-CFG-VALGET response (same class/id,
// version byte = UBX_VALGET_VERSION_RESP = 0x01) that contains the current
// values for each requested key.

#pragma once

#include "ubx_config_types.h"
#include <vector>

namespace ubx
{
namespace config
{

class ubx_cfg_valget_builder
{
public:
    ubx_cfg_valget_builder() = default;

    // Layer values for VALGET requests (UBX spec §3.4.6)
    static constexpr uint8_t LAYER_RAM     = 0u;
    static constexpr uint8_t LAYER_BBR     = 1u;
    static constexpr uint8_t LAYER_FLASH   = 2u;
    static constexpr uint8_t LAYER_DEFAULT = 7u;

    /// Build a complete, framed UBX-CFG-VALGET poll request.
    ///
    /// @param key_ids   Keys to poll from the chip.
    /// @param layer     Source layer to query (normally LAYER_RAM).
    /// @param position  First entry offset for large, chunked responses (0 = start).
    /// @return          Framed UBX message bytes, or empty if key_ids is empty.
    std::vector<uint8_t> build(const std::vector<uint32_t>& key_ids,
                                uint8_t  layer    = LAYER_RAM,
                                uint16_t position = 0u) const;
};

} // namespace config
} // namespace ubx
