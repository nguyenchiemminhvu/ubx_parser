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
// ubx_decoder_registry.h
//
// Registry that maps (msg_class, msg_id) pairs to decoder instances.
//
// Ownership model:
//   The registry takes ownership of each decoder via unique_ptr.  This
//   ensures deterministic destruction order and simplifies lifetime management
//   in the dispatcher.
//
// Thread-safety:
//   ubx_decoder_registry is NOT thread-safe.  All registrations must occur
//   before the first call to ubx_parser::feed() and from the same thread.
//   Look-ups during parsing are read-only and safe provided no concurrent
//   modifications occur.
//
// Extensibility (Open/Closed Principle):
//   Adding a new message type requires only:
//     1. Deriving a new class from i_message_decoder.
//     2. Calling registry.register_decoder(std::unique_ptr<new_decoder>(...)).
//   No changes to ubx_parser, ubx_dispatcher, or this registry are needed.

#pragma once

#include "decoders/i_message_decoder.h"
#include <cstdint>
#include <map>
#include <memory>
#include <utility>

namespace ubx
{
namespace parser
{

// ─── ubx_decoder_registry ─────────────────────────────────────────────────────

class ubx_decoder_registry
{
public:
    ubx_decoder_registry()  = default;
    ~ubx_decoder_registry() = default;

    // Non-copyable (owns unique_ptrs)
    ubx_decoder_registry(const ubx_decoder_registry&)            = delete;
    ubx_decoder_registry& operator=(const ubx_decoder_registry&) = delete;

    // Movable
    ubx_decoder_registry(ubx_decoder_registry&&)            = default;
    ubx_decoder_registry& operator=(ubx_decoder_registry&&) = default;

    // ── Registration ──────────────────────────────────────────────────────────

    /// Register a decoder.  If a decoder for the same (class, id) already
    /// exists it is replaced.
    ///
    /// @param decoder  A heap-allocated decoder.  Ownership is transferred.
    void register_decoder(std::unique_ptr<i_message_decoder> decoder);

    // ── Look-up ───────────────────────────────────────────────────────────────

    /// Find the decoder registered for (msg_class, msg_id).
    ///
    /// @returns non-owning pointer to the decoder, or nullptr if not found.
    i_message_decoder* find(uint8_t msg_class, uint8_t msg_id) const;

    /// @returns true if a decoder exists for (msg_class, msg_id).
    bool has(uint8_t msg_class, uint8_t msg_id) const;

    /// @returns the number of registered decoders.
    std::size_t size() const;

private:
    /// Key type: packed (class << 8 | id) for compact map storage.
    using key_type = uint16_t;

    static key_type make_key(uint8_t msg_class, uint8_t msg_id)
    {
        return static_cast<key_type>(
            (static_cast<uint16_t>(msg_class) << 8u) | static_cast<uint16_t>(msg_id));
    }

    std::map<key_type, std::unique_ptr<i_message_decoder>> decoders_;
};

} // namespace parser
} // namespace ubx
