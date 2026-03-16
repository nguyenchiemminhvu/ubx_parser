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
// messages/ubx_mon_ver.h
//
// Decoded representation of the UBX-MON-VER message (class 0x0A, ID 0x04).
//
// Reference: u-blox M10 Interface Description, Section UBX-MON-VER.
// Payload layout (variable length):
//   swVersion  (CH[30])  — Software version string, null-terminated
//   hwVersion  (CH[10])  — Hardware version string, null-terminated
//   extension  (CH[30])  × N — Optional extension strings, 0 or more
//
// Minimum payload: 40 bytes (swVersion + hwVersion).
// Each additional extension adds exactly 30 bytes.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace ubx
{
namespace parser
{

// ─── Payload size constants ────────────────────────────────────────────────────

/// Length of the swVersion field in UBX-MON-VER.
static constexpr uint16_t UBX_MON_VER_SW_LEN = 30u;

/// Length of the hwVersion field in UBX-MON-VER.
static constexpr uint16_t UBX_MON_VER_HW_LEN = 10u;

/// Length of each extension string field in UBX-MON-VER.
static constexpr uint16_t UBX_MON_VER_EXT_LEN = 30u;

/// Minimum payload length for UBX-MON-VER (swVersion + hwVersion).
static constexpr uint16_t UBX_MON_VER_MIN_PAYLOAD_LEN =
    UBX_MON_VER_SW_LEN + UBX_MON_VER_HW_LEN;

// ─── ubx_mon_ver ──────────────────────────────────────────────────────────────
//
// Decoded UBX-MON-VER message.

struct ubx_mon_ver
{
    std::string              sw_version;   ///< Software version string
    std::string              hw_version;   ///< Hardware version string
    std::vector<std::string> extensions;   ///< Optional extension strings (0 or more)
};

/// Callback for decoded UBX-MON-VER messages.
using mon_ver_callback = std::function<void(const ubx_mon_ver&)>;

} // namespace parser
} // namespace ubx
