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
// config/ubx_datum_service.h
//
// Polls the chip's NAVSPG-USRDAT configuration keys via CFG-VALGET, interprets
// the returned ellipsoid parameters, and resolves the active coordinate datum
// (WGS-84, PZ-90, GRS-80, etc.).
//
// Workflow:
//   1. call start_valget_poll() → sends CFG-VALGET for all usrdat keys.
//   2. When the CFG-VALGET response arrives, call on_valget_response().
//   3. Retrieve the resolved datum via get_datum_data() or subscribe to the
//      on_datum_resolved callback set via set_datum_resolved_callback().
//
// Datum identification algorithm:
//   • If navspg_usrdat == false (or 0)  →  the chip uses WGS-84 internally.
//   • Otherwise the chip is using a custom ellipsoid.  The service compares
//     the semi-major axis (maja) and inverse-flattening (flat) values against
//     a built-in table of well-known datums with tolerances of ±1.0 m and
//     ±0.001 respectively.  A match is flagged as the corresponding datum_id;
//     an unmatchable combination is flagged as datum_id::user_defined.

#pragma once

#include "i_ubx_transport.h"
#include "ubx_cfg_valget_builder.h"
#include "ubx_cfg_valset_builder.h"
#include "ubx_config_types.h"
#include "../messages/ubx_cfg_valget.h"

#include "../ubx_datum.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace ubx
{
namespace config
{

// ─── ubx_datum_service ────────────────────────────────────────────────────────

class ubx_datum_service
{
public:
    using datum_resolved_callback = std::function<void(const ubx::parser::datum_data&)>;

    explicit ubx_datum_service(
        std::shared_ptr<i_ubx_transport>        transport,
        std::shared_ptr<ubx_cfg_valget_builder> valget_builder,
        std::shared_ptr<ubx_cfg_valset_builder> valset_builder);

    // Non-copyable
    ubx_datum_service(const ubx_datum_service&)            = delete;
    ubx_datum_service& operator=(const ubx_datum_service&) = delete;

    // ── Step 1 ───────────────────────────────────────────────────────────────
    /// Build and send a CFG-VALGET poll request for all NAVSPG-USRDAT keys.
    /// Returns false if the transport fails or the message cannot be built.
    bool start_valget_poll();

    // ── Write path ───────────────────────────────────────────────────────────
    /// Set the active datum by its well-known datum_id.
    /// Looks up maja and flat in the KNOWN_DATUMS table.
    /// Also writes navspg_usrdat = true.
    /// Returns false if datum_id is unknown, user_defined, or the transport fails.
    bool start_valset_by_id(ubx::parser::datum_id id,
                            ubx::config::config_layer layer = ubx::config::config_layer::ram);

    /// Set the active datum using explicit ellipsoid parameters.
    /// Also writes navspg_usrdat = true.
    /// maja must be > 0.0; flat must be > 0.0 (validated before sending).
    /// Returns false on invalid parameters or transport failure.
    bool start_valset_by_params(double maja, double flat,
                                ubx::config::config_layer layer = ubx::config::config_layer::ram);

    /// Reset the active datum to the chip's internal default (WGS-84) by
    /// writing navspg_usrdat = false.  No ellipsoid parameters are changed.
    /// Returns false if the transport fails.
    bool reset_to_default(ubx::config::config_layer layer = ubx::config::config_layer::ram);

    // ── Step 2 ───────────────────────────────────────────────────────────────
    /// Called by the upper layer (gnss_controller / ubx_config_manager) when a
    /// CFG-VALGET response frame is decoded.  Inspects every entry: only usrdat
    /// key IDs are processed; others are silently ignored.
    /// When all ten usrdat keys have been collected the datum is resolved and
    /// — if a callback is registered — the callback is invoked.
    void on_valget_response(const ubx::parser::ubx_cfg_valget& decoded);

    // ── Query ────────────────────────────────────────────────────────────────
    /// Returns the most recently resolved datum data.
    /// valid == false until at least one successful VALGET response cycle.
    const ubx::parser::datum_data& get_datum_data() const;

    // ── Callback ─────────────────────────────────────────────────────────────
    /// Register a callback that fires synchronously inside on_valget_response()
    /// when a complete datum resolution cycle finishes (all required keys seen).
    void set_datum_resolved_callback(datum_resolved_callback cb);

private:
    /// Match maja + flat against the built-in datum table.
    static ubx::parser::datum_id   resolve_datum_id(double maja, double flat);
    static const char*      datum_name(ubx::parser::datum_id id);

    std::shared_ptr<i_ubx_transport>        transport_;
    std::shared_ptr<ubx_cfg_valget_builder> valget_builder_;
    std::shared_ptr<ubx_cfg_valset_builder> valset_builder_;
    datum_resolved_callback                 on_datum_resolved_;
    ubx::parser::datum_data                 datum_data_{};

    // Tracks which usrdat keys have been received in the current cycle.
    bool received_usrdat_      = false;
    bool received_maja_        = false;
    bool received_flat_        = false;
};

} // namespace config
} // namespace ubx
