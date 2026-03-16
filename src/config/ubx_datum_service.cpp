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

#include "config/ubx_datum_service.h"
#include "config/ubx_config_key.h"   // navspg_usrdat* constants
#include "ubx_types.h"               // UBX_VALGET_VERSION_RESP

#include <cstring>
#include <cmath>
#include <iostream>

namespace ubx
{
namespace config
{

// ─── Constructor ─────────────────────────────────────────────────────────────

ubx_datum_service::ubx_datum_service(
        std::shared_ptr<i_ubx_transport>        transport,
        std::shared_ptr<ubx_cfg_valget_builder> valget_builder,
        std::shared_ptr<ubx_cfg_valset_builder> valset_builder)
    : transport_     (std::move(transport))
    , valget_builder_(std::move(valget_builder))
    , valset_builder_(std::move(valset_builder))
{
    datum_data_.valid          = false;
    datum_data_.usrdat_enabled = false;
    datum_data_.id             = ubx::parser::datum_id::wgs84;
    datum_data_.name           = datum_name(ubx::parser::datum_id::wgs84);
}

// ─── start_valget_poll ───────────────────────────────────────────────────────

bool ubx_datum_service::start_valget_poll()
{
    // Reset received-key flags for a fresh cycle.
    received_usrdat_ = received_maja_ = received_flat_ = false;

    const std::vector<uint32_t> key_ids = {
        ubx::config::cfg_key::navspg_usrdat,
        ubx::config::cfg_key::navspg_usrdat_maja,
        ubx::config::cfg_key::navspg_usrdat_flat,
    };

    const std::vector<uint8_t> msg =
        valget_builder_->build(key_ids, ubx_cfg_valget_builder::LAYER_RAM);

    if (msg.empty())
    {
        std::cerr << "[ubx_datum_service] Failed to build datum VALGET message\n";
        return false;
    }

    std::cout << "[ubx_datum_service] Sending CFG-VALGET for " << key_ids.size() << " NAVSPG-USRDAT keys\n";
    return transport_->send_sync(msg);
}

// ─── on_valget_response ──────────────────────────────────────────────────────

void ubx_datum_service::on_valget_response(
    const ubx::parser::ubx_cfg_valget& decoded)
{
    if (decoded.version != ubx::parser::UBX_VALGET_VERSION_RESP)
    {
        return;
    }

    bool any_datum_key_found = false;

    for (const ubx::parser::cfg_valget_entry& e : decoded.entries)
    {
        if (e.key_id == ubx::config::cfg_key::navspg_usrdat)
        {
            datum_data_.usrdat_enabled = (e.value != 0u);
            received_usrdat_ = true;
            any_datum_key_found = true;
        }
        else if (e.key_id == ubx::config::cfg_key::navspg_usrdat_maja)
        {
            double v = 0.0;
            std::memcpy(&v, &e.value, sizeof(v));
            datum_data_.params.maja = v;
            received_maja_ = true;
            any_datum_key_found = true;
        }
        else if (e.key_id == ubx::config::cfg_key::navspg_usrdat_flat)
        {
            double v = 0.0;
            std::memcpy(&v, &e.value, sizeof(v));
            datum_data_.params.flat = v;
            received_flat_ = true;
            any_datum_key_found = true;
        }
        else
        {
            // Ignore other keys.
        }
    }

    if (!any_datum_key_found)
    {
        // This VALGET response does not contain any datum keys — skip resolution.
        return;
    }

    // Resolve datum once we have at least the usrdat flag + ellipsoid params.
    // Incomplete responses (e.g. chunked VALGET) are still processed with
    // whatever fields have arrived.
    if (!datum_data_.usrdat_enabled)
    {
        datum_data_.id   = ubx::parser::datum_id::wgs84;
        datum_data_.name = datum_name(ubx::parser::datum_id::wgs84);
    }
    else if (received_maja_ && received_flat_)
    {
        datum_data_.id   = resolve_datum_id(datum_data_.params.maja, datum_data_.params.flat);
        datum_data_.name = datum_name(datum_data_.id);
    }

    datum_data_.valid = true;

    std::cout << "[ubx_datum_service] Datum resolved: " << datum_data_.name
              << "  usrdat_enabled=" << datum_data_.usrdat_enabled
              << "  maja=" << datum_data_.params.maja
              << "  flat=" << datum_data_.params.flat
              << "\n";

    if (on_datum_resolved_)
    {
        on_datum_resolved_(datum_data_);
    }
}

// ─── get_datum_data ───────────────────────────────────────────────────────────

const ubx::parser::datum_data& ubx_datum_service::get_datum_data() const
{
    return datum_data_;
}

// ─── set_datum_resolved_callback ─────────────────────────────────────────────

void ubx_datum_service::set_datum_resolved_callback(datum_resolved_callback cb)
{
    on_datum_resolved_ = std::move(cb);
}

// ─── resolve_datum_id ─────────────────────────────────────────────────────────

ubx::parser::datum_id ubx_datum_service::resolve_datum_id(double maja, double flat)
{
    for (const ubx::parser::datum_signature& sig : ubx::parser::KNOWN_DATUMS)
    {
        if (std::fabs(maja - sig.maja) <= ubx::parser::MAJA_TOLERANCE &&
            std::fabs(flat - sig.flat) <= ubx::parser::FLAT_TOLERANCE)
        {
            return sig.id;
        }
    }
    return ubx::parser::datum_id::user_defined;
}

// ─── datum_name ───────────────────────────────────────────────────────────────

const char* ubx_datum_service::datum_name(ubx::parser::datum_id id)
{
    switch (id)
    {
    case ubx::parser::datum_id::wgs84:        return "WGS-84";
    case ubx::parser::datum_id::pz90:         return "PZ-90.11";
    case ubx::parser::datum_id::user_defined: return "User-Defined";
    default:                                  return "Unknown";
    }
}

// ─── start_valset_by_id ──────────────────────────────────────────────────────────────

bool ubx_datum_service::start_valset_by_id(
    ubx::parser::datum_id id,
    ubx::config::config_layer layer)
{
    if (id == ubx::parser::datum_id::unknown ||
        id == ubx::parser::datum_id::user_defined)
    {
        std::cerr << "[ubx_datum_service] start_valset_by_id: cannot set unknown or user_defined datum\n";
        return false;
    }

    for (const ubx::parser::datum_signature& sig : ubx::parser::KNOWN_DATUMS)
    {
        if (sig.id == id)
        {
            return start_valset_by_params(sig.maja, sig.flat, layer);
        }
    }

    std::cerr << "[ubx_datum_service] start_valset_by_id: no entry in KNOWN_DATUMS for datum id="
              << static_cast<int>(id) << "\n";
    return false;
}

// ─── reset_to_default ───────────────────────────────────────────────────────────────────

bool ubx_datum_service::reset_to_default(ubx::config::config_layer layer)
{
    const std::vector<ubx::config::config_entry> entries = {
        { ubx::config::cfg_key::navspg_usrdat, ubx::config::config_value(false) },
    };

    const std::vector<uint8_t> frame = valset_builder_->build(entries, layer);
    if (frame.empty())
    {
        std::cerr << "[ubx_datum_service] reset_to_default: failed to build VALSET message\n";
        return false;
    }

    std::cout << "[ubx_datum_service] Sending CFG-VALSET reset datum to WGS-84 (navspg_usrdat=false)\n";
    return transport_->send_async(frame);
}

// ─── start_valset_by_params ─────────────────────────────────────────────────────────────

bool ubx_datum_service::start_valset_by_params(
    double maja,
    double flat,
    ubx::config::config_layer layer)
{
    if (maja <= 0.0 || flat <= 0.0)
    {
        std::cerr << "[ubx_datum_service] start_valset_by_params: invalid parameters"
                  << " maja=" << maja << " flat=" << flat << "\n";
        return false;
    }

    uint64_t maja_raw = 0u;
    uint64_t flat_raw = 0u;
    std::memcpy(&maja_raw, &maja, sizeof(maja_raw));
    std::memcpy(&flat_raw, &flat, sizeof(flat_raw));

    const std::vector<ubx::config::config_entry> entries = {
        { ubx::config::cfg_key::navspg_usrdat,      ubx::config::config_value(true)     },
        { ubx::config::cfg_key::navspg_usrdat_maja, ubx::config::config_value(maja_raw) },
        { ubx::config::cfg_key::navspg_usrdat_flat, ubx::config::config_value(flat_raw) },
    };

    const std::vector<uint8_t> frame = valset_builder_->build(entries, layer);
    if (frame.empty())
    {
        std::cerr << "[ubx_datum_service] start_valset_by_params: failed to build VALSET message\n";
        return false;
    }

    const ubx::parser::datum_id resolved_id = resolve_datum_id(maja, flat);
    std::cout << "[ubx_datum_service] Sending CFG-VALSET datum: " << datum_name(resolved_id)
              << "  maja=" << maja << "  flat=" << flat << "\n";

    return transport_->send_async(frame);
}

} // namespace config
} // namespace ubx
