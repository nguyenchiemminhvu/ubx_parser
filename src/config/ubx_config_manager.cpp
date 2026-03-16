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

#include "config/ubx_config_manager.h"

#include <iostream>

namespace ubx
{
namespace config
{

ubx_config_manager::ubx_config_manager(std::shared_ptr<i_ubx_transport>         transport,
                                         std::shared_ptr<i_ini_config_provider>   config_provider,
                                         std::shared_ptr<i_ubx_config_repository> repository)
    : transport_     (transport)
    , valset_builder_(std::make_shared<ubx_cfg_valset_builder>())
    , valget_builder_(std::make_shared<ubx_cfg_valget_builder>())
    , sync_service_  (transport, config_provider, repository,
                       valget_builder_, valset_builder_)
    , datum_service_ (transport, valget_builder_, valset_builder_)
{}

// ── Direct VALSET ─────────────────────────────────────────────────────────────

bool ubx_config_manager::set_config(const std::vector<config_entry>& entries,
                                      config_layer layer)
{
    if (entries.empty())
    {
        return false;
    }
    const std::vector<uint8_t> msg = valset_builder_->build(entries, layer);
    if (msg.empty())
    {
        return false;
    }
    return transport_->send_async(msg);
}

// ── Direct VALGET ─────────────────────────────────────────────────────────────

bool ubx_config_manager::poll_config(const std::vector<uint32_t>& key_ids,
                                      uint8_t layer)
{
    if (key_ids.empty())
    {
        return false;
    }
    const std::vector<uint8_t> msg = valget_builder_->build(key_ids, layer);
    if (msg.empty())
    {
        return false;
    }
    return transport_->send_async(msg);
}

// ── VALGET response callback ──────────────────────────────────────────────────

void ubx_config_manager::on_valget_response(
    const ubx::parser::ubx_cfg_valget& decoded)
{
    sync_service_.on_valget_response(decoded);
    datum_service_.on_valget_response(decoded);
}
// ── Configuration synchronisation ────────────────────────────────────────────

bool ubx_config_manager::start_poll(const std::string& ini_path)
{
    if (!sync_service_.load_default_config(ini_path))
    {
        std::cerr << "[ubx_config_manager] Failed to load default config: " << ini_path << std::endl;
        return false;
    }
    return sync_service_.start_valget_poll();
}

bool ubx_config_manager::apply_pending_sync(config_layer layer)
{
    std::cout << "[ubx_config_manager] Applying pending configuration sync for layer: " << static_cast<int>(layer) << std::endl;
    return sync_service_.apply_configuration(layer);
}

// ── Datum configuration query ───────────────────────────────────────────────────────────

bool ubx_config_manager::query_datum(datum_resolved_callback cb)
{
    datum_service_.set_datum_resolved_callback(std::move(cb));
    return datum_service_.start_valget_poll();
}

const ubx::parser::datum_data& ubx_config_manager::get_datum_data() const
{
    return datum_service_.get_datum_data();
}

bool ubx_config_manager::reset_datum(config_layer layer)
{
    return datum_service_.reset_to_default(layer);
}

bool ubx_config_manager::set_datum_by_id(ubx::parser::datum_id id,
                                          config_layer layer)
{
    return datum_service_.start_valset_by_id(id, layer);
}

bool ubx_config_manager::set_datum_by_params(double maja, double flat,
                                              config_layer layer)
{
    return datum_service_.start_valset_by_params(maja, flat, layer);
}

} // namespace config
} // namespace ubx
