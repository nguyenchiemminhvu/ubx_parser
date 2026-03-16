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
// config/i_ubx_config_repository.h
//
// Interface for in-process storage of the GNSS receiver's current configuration.
//
// The repository is populated when UBX-CFG-VALGET responses arrive from the
// chip (via ubx_config_sync_service::on_valget_response). It is queried during
// the comparison step to identify parameters that need updating.
//
// Keeping storage behind an interface allows unit tests to inject a mock
// repository and verify sync logic independently of the storage mechanism.

#pragma once

#include "ubx_config_types.h"
#include <vector>

namespace ubx
{
namespace config
{

class i_ubx_config_repository
{
public:
    virtual ~i_ubx_config_repository() = default;

    /// Persist a single key-value pair (overwrites any existing entry).
    virtual void store(const config_entry& entry) = 0;

    /// Retrieve the value for the given key.
    /// Returns true and writes to out_value if the key is present.
    virtual bool fetch(uint32_t key_id, config_value& out_value) const = 0;

    /// Returns true if the key is present in the repository.
    virtual bool contains(uint32_t key_id) const = 0;

    /// Returns all stored key IDs in implementation-defined order.
    virtual std::vector<uint32_t> all_keys() const = 0;

    /// Remove all stored entries (used before a fresh VALGET cycle).
    virtual void clear() = 0;
};

} // namespace config
} // namespace ubx
