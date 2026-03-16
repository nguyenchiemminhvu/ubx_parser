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
// config/ubx_config_sync_service.h
//
// Implements the four-step GNSS configuration synchronisation protocol.
//
// Step 1 – Load default configuration from an INI file via i_ini_config_provider.
// Step 2 – Send UBX-CFG-VALGET for all default-config keys to poll the chip's
//           current values.
// Step 3 – Receive the VALGET response (called back by the UBX parser layer)
//           and store decoded key-value pairs in the repository.
// Step 4 – Compare repository vs. defaults; send UBX-CFG-VALSET for every key
//           whose current value differs from the desired default.
//
// Dependencies are injected at construction time (Dependency Inversion Principle).

#pragma once

#include "ubx_config_types.h"
#include "ubx_cfg_valget_builder.h"
#include "ubx_cfg_valset_builder.h"
#include "i_ubx_transport.h"
#include "i_ini_config_provider.h"
#include "i_ubx_config_repository.h"
#include "../messages/ubx_cfg_valget.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ubx
{
namespace config
{

class ubx_config_sync_service
{
public:
    ubx_config_sync_service(std::shared_ptr<i_ubx_transport>         transport,
                             std::shared_ptr<i_ini_config_provider>   config_provider,
                             std::shared_ptr<i_ubx_config_repository> repository,
                             std::shared_ptr<ubx_cfg_valget_builder>  valget_builder,
                             std::shared_ptr<ubx_cfg_valset_builder>  valset_builder);

    // ── Step 1 ────────────────────────────────────────────────────────────────
    /// Load the default configuration from the file at 'ini_path'.
    /// Returns false if the provider fails to load the file.
    bool load_default_config(const std::string& ini_path);

    // ── Step 2 ────────────────────────────────────────────────────────────────
    /// Build and send a UBX-CFG-VALGET message polling the chip for every key
    /// that was loaded in step 1. The repository is cleared before the poll.
    /// Returns false if load_default_config() has not been called, the default
    /// config is empty, or the transport send fails.
    bool start_valget_poll();

    // ── Step 3 ────────────────────────────────────────────────────────────────
    /// Accept a pre-decoded UBX-CFG-VALGET response and store the key-value
    /// pairs in the repository.  Prefer this overload when the data has already
    /// been decoded by cfg_valget_decoder to avoid repeated parsing.
    ///
    /// Safe to call multiple times for paged responses.
    void on_valget_response(const ubx::parser::ubx_cfg_valget& decoded);

    // ── Step 4 ────────────────────────────────────────────────────────────────
    /// Compare the repository against the default config loaded in step 1.
    /// For every key where the current value differs from the desired default,
    /// build and send a single UBX-CFG-VALSET message targeting 'layer'.
    ///
    /// Returns true if no differences were detected, or if VALSET was sent
    /// successfully. Returns false if VALSET build produced no message or the
    /// transport send failed.
    bool apply_configuration(config_layer layer = config_layer::ram);

    /// Returns the number of default config entries loaded in step 1.
    std::size_t default_config_size() const;

private:
    std::shared_ptr<i_ubx_transport>         transport_;
    std::shared_ptr<i_ini_config_provider>   config_provider_;
    std::shared_ptr<i_ubx_config_repository> repository_;
    std::shared_ptr<ubx_cfg_valget_builder>  valget_builder_;
    std::shared_ptr<ubx_cfg_valset_builder>  valset_builder_;

    std::vector<config_entry> default_config_;
};

} // namespace config
} // namespace ubx
