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

#include "config/ubx_config_sync_service.h"
#include "config/ubx_config_protocol.h"  // ubx::config::protocol + shared helpers
#include "messages/ubx_cfg_valget.h"     // ubx::parser::ubx_cfg_valget
#include "ubx_types.h"                   // UBX_VALGET_VERSION_RESP

#include <iostream>

namespace ubx
{
namespace config
{

// VALGET response payload layout (UBX spec §3.4.6):
//   Byte  0     : version (0x01 for response)
//   Byte  1     : layer
//   Bytes 2-3   : position (uint16_t LE, entry offset for chunked responses)
//   Bytes 4+    : key-value records
//                   key   : 4 bytes LE uint32_t
//                   value : N bytes LE, N = value_byte_size(key_id)
static constexpr uint16_t VALGET_RESP_HEADER_SIZE = 4u;

ubx_config_sync_service::ubx_config_sync_service(
        std::shared_ptr<i_ubx_transport>         transport,
        std::shared_ptr<i_ini_config_provider>   config_provider,
        std::shared_ptr<i_ubx_config_repository> repository,
        std::shared_ptr<ubx_cfg_valget_builder>  valget_builder,
        std::shared_ptr<ubx_cfg_valset_builder>  valset_builder)
    : transport_       (std::move(transport))
    , config_provider_ (std::move(config_provider))
    , repository_      (std::move(repository))
    , valget_builder_  (std::move(valget_builder))
    , valset_builder_  (std::move(valset_builder))
{}

// ── Step 1 ────────────────────────────────────────────────────────────────────

bool ubx_config_sync_service::load_default_config(const std::string& ini_path)
{
    if (!config_provider_->load(ini_path))
    {
        std::cerr << "[ubx_config_sync_service] Failed to load default config from INI file: " << ini_path << std::endl;
        return false;
    }
    default_config_ = config_provider_->get_all_entries();
    std::cout << "[ubx_config_sync_service] Loaded default config with " << default_config_.size() << " entries\n";
    for (const config_entry& e : default_config_)
    {
        std::cout << "  - key_id: " << e.key_id << ", value: " << e.value.as_i64() << std::endl;
    }
    return !default_config_.empty();
}

// ── Step 2 ────────────────────────────────────────────────────────────────────

bool ubx_config_sync_service::start_valget_poll()
{
    std::cout << "[ubx_config_sync_service] Starting VALGET poll for " << default_config_.size() << " entries\n";
    if (default_config_.empty())
    {
        std::cerr << "[ubx_config_sync_service] No entries found in default config\n";
        return false;
    }

    // Collect key IDs for polling
    std::vector<uint32_t> key_ids;
    key_ids.reserve(default_config_.size());
    for (const config_entry& e : default_config_)
    {
        key_ids.push_back(e.key_id);
    }

    // Clear stale chip-state before issuing a fresh poll
    repository_->clear();

    const std::vector<uint8_t> msg =
        valget_builder_->build(key_ids, ubx_cfg_valget_builder::LAYER_RAM);

    if (msg.empty())
    {
        std::cerr << "[ubx_config_sync_service] Failed to build VALGET message\n";
        return false;
    }

    return transport_->send_sync(msg);
}

// ── Step 3 ────────────────────────────────────────────────────────────────────

void ubx_config_sync_service::on_valget_response(
    const ubx::parser::ubx_cfg_valget& decoded)
{
    if (decoded.version != ubx::parser::UBX_VALGET_VERSION_RESP)
    {
        std::cerr << "[ubx_config_sync_service] Received VALGET response with unexpected version: " << static_cast<int>(decoded.version) << std::endl;
        return;
    }

    for (const ubx::parser::cfg_valget_entry& e : decoded.entries)
    {
        repository_->store({e.key_id, config_value(e.value)});
        std::cout << "[ubx_config_sync_service] Stored VALGET response entry - key_id: " << e.key_id << ", value: " << e.value << std::endl;
    }
}

// ── Step 4 ────────────────────────────────────────────────────────────────────

bool ubx_config_sync_service::apply_configuration(config_layer layer)
{
    std::vector<config_entry> updates;

    for (const config_entry& desired : default_config_)
    {
        config_value current_val;
        const bool present = repository_->fetch(desired.key_id, current_val);

        if (!present || current_val != desired.value)
        {
            updates.push_back(desired);
        }
    }

    if (updates.empty())
    {
        std::cout << "[ubx_config_sync_service] No configuration updates needed; chip already matches desired configuration\n";
        return true;  // chip already matches desired configuration
    }

    const std::vector<uint8_t> msg = valset_builder_->build(updates, layer);
    if (msg.empty())
    {
        std::cerr << "[ubx_config_sync_service] Failed to build VALSET message\n";
        return false;
    }

    return transport_->send_sync(msg);
}

// ─────────────────────────────────────────────────────────────────────────────

std::size_t ubx_config_sync_service::default_config_size() const
{
    return default_config_.size();
}

} // namespace config
} // namespace ubx
