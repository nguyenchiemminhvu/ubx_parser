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
// decoders/i_message_decoder.h
//
// Pure-virtual decoder interface — the core extension point for new UBX message
// types.
//
// Design (Open/Closed Principle):
//   - To add support for a new UBX message type, derive from i_message_decoder,
//     implement decode(), and register the derived object with ubx_decoder_registry.
//   - The parser core (ubx_parser) and dispatcher (ubx_dispatcher) never need to
//     be modified when adding new message types.
//
// Thread-safety:
//   - i_message_decoder implementations must be callable from the same thread
//     that calls ubx_parser::feed().  No internal synchronisation is required
//     by the interface itself; implementations should document their own policy.

#pragma once

#include "ubx_types.h"

namespace ubx
{
namespace parser
{

// ─── i_message_decoder ────────────────────────────────────────────────────────
//
// Base class for all per-message-type decoders.

class i_message_decoder
{
public:
    virtual ~i_message_decoder() = default;

    // Prevent accidental copy/move slicing.
    i_message_decoder()                                    = default;
    i_message_decoder(const i_message_decoder&)            = delete;
    i_message_decoder& operator=(const i_message_decoder&) = delete;

    // ── Identity ──────────────────────────────────────────────────────────────

    /// Returns the UBX message class byte this decoder handles.
    virtual uint8_t msg_class() const = 0;

    /// Returns the UBX message ID byte this decoder handles.
    virtual uint8_t msg_id() const = 0;

    // ── Core decode operation ─────────────────────────────────────────────────

    /// Decode a fully validated UBX frame and invoke any registered callbacks.
    ///
    /// @param frame  A fully validated UBX frame (checksum verified by the
    ///               parser before this method is called).
    ///               The payload vector contains exactly header.payload_length bytes.
    ///
    /// @returns true  if the frame was decoded successfully and callbacks fired.
    /// @returns false if the payload was malformed (e.g. wrong length, invalid
    ///                field values).  The caller logs the error; the decoder
    ///                should not throw.
    virtual bool decode(const ubx_frame& frame) = 0;
};

} // namespace parser
} // namespace ubx
