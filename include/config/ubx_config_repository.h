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
// config/ubx_config_repository.h
//
// Concrete, in-memory implementation of i_ubx_config_repository backed by a
// sorted std::map<uint32_t, config_value>.
//
// Access complexity : O(log n) per lookup.
// Thread safety     : none — single-threaded use only.

#pragma once

#include "i_ubx_config_repository.h"
#include <map>

namespace ubx
{
namespace config
{

class ubx_config_repository : public i_ubx_config_repository
{
public:
    ubx_config_repository() = default;

    void store(const config_entry& entry)                      override;
    bool fetch(uint32_t key_id, config_value& out_value) const override;
    bool contains(uint32_t key_id)                       const override;
    std::vector<uint32_t> all_keys()                     const override;
    void clear()                                               override;

private:
    std::map<uint32_t, config_value> entries_;
};

} // namespace config
} // namespace ubx
