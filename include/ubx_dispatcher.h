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
// ubx_dispatcher.h
//
// Routes fully validated UBX frames to their registered decoder (or to the
// raw-message callback if no decoder is registered for that class/ID).
//
// The dispatcher also holds the raw-message callback and the parse-error
// callback, centralising all outbound notifications in one place.
//
// Thread-safety:
//   ubx_dispatcher is NOT thread-safe.  It must be used from the same thread
//   that calls ubx_parser::feed().

#pragma once

#include "ubx_types.h"
#include "ubx_errors.h"
#include "ubx_decoder_registry.h"
#include "messages/ubx_raw_message.h"

namespace ubx
{
namespace parser
{

// ─── ubx_dispatcher ───────────────────────────────────────────────────────────

class ubx_dispatcher
{
public:
    /// Construct with a reference to an external decoder registry.
    /// The registry must outlive the dispatcher.
    explicit ubx_dispatcher(const ubx_decoder_registry& registry);

    ~ubx_dispatcher() = default;

    // Non-copyable
    ubx_dispatcher(const ubx_dispatcher&)            = delete;
    ubx_dispatcher& operator=(const ubx_dispatcher&) = delete;

    // ── Callback registration ─────────────────────────────────────────────────

    /// Register (or replace) the callback for valid frames that have no
    /// registered decoder.
    /// @param cb  Callback.  Pass a null std::function to remove.
    void set_raw_message_callback(raw_message_callback_t cb);

    /// Register (or replace) the parse-error callback.
    /// @param cb  Callback.  Pass a null std::function to remove.
    void set_error_callback(parse_error_callback cb);

    // ── Dispatch ──────────────────────────────────────────────────────────────

    /// Dispatch a fully validated frame:
    ///   1. Wrap in ubx_raw_message and fire raw_message_callback_.
    ///   2. If a decoder is registered for (class, id), call decoder->decode().
    void dispatch(const ubx_frame& frame);

    /// Fire the error callback with the provided error information.
    /// Safe to call when no error callback is set (no-op).
    void dispatch_error(const parse_error_info& info);

private:
    const ubx_decoder_registry& registry_;
    raw_message_callback_t       raw_cb_;
    parse_error_callback         error_cb_;
};

} // namespace parser
} // namespace ubx
