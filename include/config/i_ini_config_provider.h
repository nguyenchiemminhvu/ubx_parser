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
// config/i_ini_config_provider.h
//
// Interface for reading default GNSS configuration from an INI file.
//
// Implementations parse the persistent configuration file and expose each
// entry as a typed config_entry. This allows the synchronisation service to
// compare the desired defaults with the values currently programmed into the
// GNSS chip without depending on any specific INI parser library.

#pragma once

#include "ubx_config_types.h"
#include <string>
#include <vector>

namespace ubx
{
namespace config
{

class i_ini_config_provider
{
public:
    virtual ~i_ini_config_provider() = default;

    /// Load configuration from the given filesystem path.
    /// Returns true on success; false if the file cannot be opened or parsed.
    virtual bool load(const std::string& path) = 0;

    /// Return all configuration entries successfully loaded from the file.
    /// Returns an empty vector if load() has not been called or failed.
    virtual std::vector<config_entry> get_all_entries() const = 0;
};

} // namespace config
} // namespace ubx
