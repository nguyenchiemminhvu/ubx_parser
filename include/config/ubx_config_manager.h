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
// config/ubx_config_manager.h
//
// Public API facade for GNSS configuration management within ubx_parser.
//
// Responsibilities:
//   • Set configuration values (VALSET).
//   • Poll current configuration values (VALGET).
//   • Route incoming VALGET response payloads to the sync service.
//   • Orchestrate the full synchronisation cycle
//     (load INI → VALGET → compare → VALSET).
//
// All dependencies are injected at construction time. The manager owns the
// concrete builder instances while accepting transport, repository and INI
// provider through their abstract interfaces.
//
// Typical Location Service startup sequence:
//
//   class my_transport : public ubx::config::i_ubx_transport { ... };
//   class my_ini       : public ubx::config::i_ini_config_provider { ... };
//
//   my_transport transport;
//   my_ini       ini_prov;
//   ubx::config::ubx_config_repository repo;
//   ubx::config::ubx_config_manager    mgr(transport, ini_prov, repo);
//
//   mgr.start_poll("/etc/gnss_default_config.ini");
//   // ... (UART callback fires) ...
//   mgr.on_valget_response(payload, length);
//   mgr.apply_pending_sync(ubx::config::config_layer::ram);

#pragma once

#include "ubx_config_types.h"
#include "ubx_cfg_valset_builder.h"
#include "ubx_cfg_valget_builder.h"
#include "ubx_config_sync_service.h"
#include "ubx_datum_service.h"
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

class ubx_config_manager
{
public:
    ubx_config_manager(std::shared_ptr<i_ubx_transport>         transport,
                        std::shared_ptr<i_ini_config_provider>   config_provider,
                        std::shared_ptr<i_ubx_config_repository> repository);

    // Non-copyable — owns builder value objects.
    ubx_config_manager(const ubx_config_manager&)            = delete;
    ubx_config_manager& operator=(const ubx_config_manager&) = delete;

    // ── Direct VALSET ──────────────────────────────────────────────────────────
    /// Build and send a UBX-CFG-VALSET message for the provided key-value pairs.
    /// Returns true if the message was sent successfully.
    bool set_config(const std::vector<config_entry>& entries,
                    config_layer layer = config_layer::ram);

    // ── Direct VALGET ─────────────────────────────────────────────────────────
    /// Build and send a UBX-CFG-VALGET poll request for the provided key IDs.
    /// The response is delivered asynchronously via on_valget_response().
    /// Returns true if the request was sent successfully.
    bool poll_config(const std::vector<uint32_t>& key_ids,
                     uint8_t layer = ubx_cfg_valget_builder::LAYER_RAM);

    // ── VALGET response callback ───────────────────────────────────────────────
    /// Process a UBX-CFG-VALGET response payload by decoding it and forwarding the
    /// result to the sync service. This is called by the UBX parser layer when a
    /// VALGET response is received, and the decoded message is forwarded to the
    /// sync service for repository storage and pending-sync application.
    void on_valget_response(const ubx::parser::ubx_cfg_valget& decoded);

    // ── Configuration synchronisation ─────────────────────────────────────────
    /// Run steps 1 + 2 of the synchronisation cycle:
    ///   1. Load default configuration from 'ini_path'.
    ///   2. Send VALGET to poll the chip's current configuration.
    ///
    /// Step 3 (response decoding) is handled via on_valget_response().
    /// Step 4 (apply differences) must be called explicitly after the response
    /// arrives — see apply_pending_sync() below.
    ///
    /// Returns false if the INI file cannot be loaded or the VALGET send fails.
    bool start_poll(const std::string& ini_path);

    /// Apply the pending synchronisation after a VALGET response has been
    /// received and stored. Sends VALSET for keys that differ from defaults.
    /// Returns true if the chip was already correct (no VALSET needed) or if
    /// VALSET was sent successfully.
    bool apply_pending_sync(config_layer layer = config_layer::ram);

    // ── Datum configuration query ────────────────────────────────────────────────
    /// Callback type fired when datum resolution completes.
    using datum_resolved_callback = ubx_datum_service::datum_resolved_callback;

    /// Send a CFG-VALGET poll for all NAVSPG-USRDAT configuration keys.
    /// When the VALGET response arrives via on_valget_response() the datum
    /// service will identify the active reference datum and invoke 'cb'.
    /// Returns false if the transport fails.
    bool query_datum(datum_resolved_callback cb);

    /// Program the chip with the ellipsoid parameters for the given well-known datum.
    /// Sends CFG-VALSET for navspg_usrdat (=true), navspg_usrdat_maja, navspg_usrdat_flat.
    /// Returns false if datum_id has no known parameters or the transport fails.
    bool set_datum_by_id(ubx::parser::datum_id id,
                         config_layer layer = config_layer::ram);

    /// Program the chip with explicit ellipsoid parameters.
    /// Sends CFG-VALSET for navspg_usrdat (=true), navspg_usrdat_maja, navspg_usrdat_flat.
    /// Returns false on invalid parameters or transport failure.
    bool set_datum_by_params(double maja, double flat,
                              config_layer layer = config_layer::ram);

    /// Reset the chip's datum to its internal default (WGS-84) by writing
    /// navspg_usrdat = false via CFG-VALSET.  No ellipsoid parameters are altered.
    /// Returns false if the transport fails.
    bool reset_datum(config_layer layer = config_layer::ram);

    /// Return the most recently resolved datum (valid == false until the first
    /// successful on_valget_response() processing of usrdat keys).
    const ubx::parser::datum_data& get_datum_data() const;

private:
    std::shared_ptr<i_ubx_transport>        transport_;

    std::shared_ptr<ubx_cfg_valset_builder>  valset_builder_;
    std::shared_ptr<ubx_cfg_valget_builder>  valget_builder_;
    ubx_config_sync_service                  sync_service_;
    ubx_datum_service                        datum_service_;
};

} // namespace config
} // namespace ubx
