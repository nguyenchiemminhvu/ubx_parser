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
// messages/ubx_raw_message.h
//
// Wrapper for UBX messages that have no dedicated decoder registered.
//
// When the parser receives a valid (checksum-verified) UBX frame but there is
// no decoder registered for its class/ID combination, the frame is wrapped in
// ubx_raw_message and delivered to the raw message callback.  This lets the
// application handle unsupported messages without modifying the parser core,
// satisfying the Open/Closed Principle.

#pragma once

#include "ubx_types.h"
#include <functional>

namespace ubx
{
namespace parser
{

// ─── ubx_raw_message ──────────────────────────────────────────────────────────
//
// Thin wrapper around ubx_frame, providing a named type for the raw-message
// callback to distinguish it from the internal parser frame type.

struct ubx_raw_message
{
    ubx_frame frame; ///< The fully validated (checksum-ok) UBX frame.
};

/// Callback for valid UBX frames that have no registered decoder.
using raw_message_callback_t = std::function<void(const ubx_raw_message&)>;

} // namespace parser
} // namespace ubx
