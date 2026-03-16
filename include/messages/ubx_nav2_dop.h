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
// messages/ubx_nav2_dop.h
//
// Decoded representation of the UBX-NAV2-DOP message (class 0x29, ID 0x04).
//
// Reference: u-blox Interface Description (M10/ADR), UBX-NAV2-DOP section.
// Payload size: 18 bytes (fixed) — identical layout to UBX-NAV-DOP.
//
// Scaling: all DOP values are dimensionless, scaled by 0.01.

#pragma once

#include <cstdint>
#include <functional>

namespace ubx
{
namespace parser
{

/// Fixed payload length for UBX-NAV2-DOP.
static constexpr uint16_t UBX_NAV2_DOP_PAYLOAD_LEN = 18u;

// ─── ubx_nav2_dop ─────────────────────────────────────────────────────────────
//
// Decoded UBX-NAV2-DOP message.
// All DOP values are raw (multiply by 0.01 for the real value).

struct ubx_nav2_dop
{
    uint32_t i_tow;  ///< GPS time of week [ms]
    uint16_t g_dop;  ///< Geometric DOP [0.01]
    uint16_t p_dop;  ///< Position DOP [0.01]
    uint16_t t_dop;  ///< Time DOP [0.01]
    uint16_t v_dop;  ///< Vertical DOP [0.01]
    uint16_t h_dop;  ///< Horizontal DOP [0.01]
    uint16_t n_dop;  ///< Northing DOP [0.01]
    uint16_t e_dop;  ///< Easting DOP [0.01]
};

/// Callback for decoded UBX-NAV2-DOP messages.
using nav2_dop_callback = std::function<void(const ubx_nav2_dop&)>;

} // namespace parser
} // namespace ubx
