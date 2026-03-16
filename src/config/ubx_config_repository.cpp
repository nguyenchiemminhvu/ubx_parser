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

#include "config/ubx_config_repository.h"

namespace ubx
{
namespace config
{

void ubx_config_repository::store(const config_entry& entry)
{
    entries_[entry.key_id] = entry.value;
}

bool ubx_config_repository::fetch(uint32_t key_id, config_value& out_value) const
{
    const auto it = entries_.find(key_id);
    if (it == entries_.end())
    {
        return false;
    }
    out_value = it->second;
    return true;
}

bool ubx_config_repository::contains(uint32_t key_id) const
{
    return entries_.find(key_id) != entries_.end();
}

std::vector<uint32_t> ubx_config_repository::all_keys() const
{
    std::vector<uint32_t> keys;
    keys.reserve(entries_.size());
    for (const auto& pair : entries_)
    {
        keys.push_back(pair.first);
    }
    return keys;
}

void ubx_config_repository::clear()
{
    entries_.clear();
}

} // namespace config
} // namespace ubx
