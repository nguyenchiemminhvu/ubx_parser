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
// messages/ubx_inf.h
//
// Decoded representation of UBX-INF-* messages (class 0x04).
//
// Reference: u-blox M10 Interface Description, Section UBX-INF.
//
// The INF class groups informational text output from the receiver.
// Each subtype shares the same payload format: a variable-length ASCII
// character string (not necessarily null-terminated).
//
// Supported message IDs:
//   0x00  INF-ERROR
//   0x01  INF-WARNING
//   0x02  INF-NOTICE
//   0x03  INF-TEST
//   0x04  INF-DEBUG

#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── inf_subtype ──────────────────────────────────────────────────────────────

/// Identifies which INF message ID was received.
enum class inf_subtype : uint8_t
{
    error   = 0x00u,
    warning = 0x01u,
    notice  = 0x02u,
    test    = 0x03u,
    debug   = 0x04u,
};

// ─── ubx_inf ──────────────────────────────────────────────────────────────────
//
// Decoded UBX-INF-* message.

struct ubx_inf
{
    inf_subtype subtype;  ///< Which INF message this is
    std::string text;     ///< Payload decoded as a UTF-8 / ASCII string
};

/// Callback for decoded UBX-INF-* messages.
using inf_callback = std::function<void(const ubx_inf&)>;

} // namespace parser
} // namespace ubx
