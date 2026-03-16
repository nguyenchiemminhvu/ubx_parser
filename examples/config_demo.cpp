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
// examples/config_demo.cpp
//
// Demonstrates how to wire up and use the UBX configuration sub-feature that
// is integrated inside ubx_parser.
//
// All config types live in the ubx::config namespace.  Protocol framing
// constants (UBX_CLASS_CFG, UBX_ID_CFG_VALGET/VALSET, …) and the low-level
// LE helpers come from the shared ubx_types.h / ubx_protocol.h that is also
// used by the parser and database layers — nothing is duplicated.
//
// The demo shows the full 4-step synchronisation cycle:
//
//   Step 1 – load_default_config  (INI file → in-memory defaults)
//   Step 2 – start_valget_poll    (VALGET poll → UART)
//   Step 3 – on_valget_response   (decoded ubx_cfg_valget → repository)
//   Step 4 – apply_pending_sync   (diff → VALSET → UART)
//
// It also demonstrates ad-hoc set_config / poll_config calls.
//
// The UART transport and INI provider shown here are minimal stubs —
// replace them with real platform implementations in production.

// ── ubx_parser config sub-feature ────────────────────────────────────────────
#include "config/ubx_config_manager.h"
#include "config/ubx_config_repository.h"
#include "config/ubx_config_key.h"
#include "config/i_ubx_transport.h"
#include "config/i_ini_config_provider.h"
#include "config/ubx_cfg_key_registry.h"

#include <cstdio>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Stub: UART transport
//
// In production, replace send() with a real write() / writev() call against
// the GNSS receiver's UART file descriptor.
// ─────────────────────────────────────────────────────────────────────────────

class uart_transport : public ubx::config::i_ubx_transport
{
public:
    uart_transport() {}

    bool send_async(const std::vector<uint8_t>& message) override
    {
        std::printf("[UART] send %zu bytes  [", message.size());
        // Print first few bytes for visibility
        const std::size_t preview = (message.size() < 8u) ? message.size() : 8u;
        for (std::size_t i = 0u; i < preview; ++i)
            std::printf("%02X%s", message[i], (i + 1u < preview) ? " " : "");
        if (message.size() > preview) std::printf(" ...");
        std::printf("]\n");
        return true;   // stub: always succeeds
    }

    bool send_sync(const std::vector<uint8_t>& message) override
    {
        // For demo purposes, just call send_async.  In production, this would
        // implement a stronger delivery guarantee (e.g. wait for ACK/NACK).
        return send_async(message);
    }

    void handle_ack(uint8_t msg_class, uint8_t msg_id, bool is_ack) override
    {
        std::printf("[UART] received %s for class=0x%02X id=0x%02X\n",
                    is_ack ? "ACK" : "NACK", msg_class, msg_id);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Stub: INI config provider
//
// A real implementation would:
//   1. Parse the INI file at the given path.
//   2. For each key=value line call
//        ubx::config::ubx_cfg_key_registry::lookup_by_name(key, key_id)
//      to resolve the numeric ID.
//   3. Construct a config_value from the parsed value and push a config_entry.
//
// The hardcoded defaults below mirror the example in the README so the
// simulated VALGET response can deliberately differ on rate_meas to trigger
// a VALSET.
// ─────────────────────────────────────────────────────────────────────────────

class gnss_ini_config_provider : public ubx::config::i_ini_config_provider
{
public:
    bool load(const std::string& path) override
    {
        std::printf("[INI] loading %s\n", path.c_str());

        // Resolve key names → IDs via the registry (same path a real adapter uses).
        auto resolve = [](const char* name, uint32_t& id) -> bool {
            return ubx::config::ubx_cfg_key_registry::lookup_by_name(name, id);
        };

        entries_.clear();

        uint32_t id = 0u;
        if (resolve("rate_meas", id))
            entries_.push_back({ id, ubx::config::config_value(static_cast<uint16_t>(1000u)) });

        if (resolve("navspg_dynmodel", id))
            entries_.push_back({ id, ubx::config::config_value(static_cast<uint8_t>(6u)) });

        if (resolve("msgout_ubx_nav_pvt_uart1", id))
            entries_.push_back({ id, ubx::config::config_value(static_cast<uint8_t>(1u)) });

        if (resolve("signal_gps_ena", id))
            entries_.push_back({ id, ubx::config::config_value(true) });

        std::printf("[INI] loaded %zu entries\n", entries_.size());
        return !entries_.empty();
    }

    std::vector<ubx::config::config_entry> get_all_entries() const override
    {
        return entries_;
    }

private:
    std::vector<ubx::config::config_entry> entries_;
};

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== ubx_parser config sub-feature demo ===\n\n");

    // ── Wire up dependencies ──────────────────────────────────────────────────

    auto transport    = std::make_shared<uart_transport>();
    auto ini_provider = std::make_shared<gnss_ini_config_provider>();
    auto repository   = std::make_shared<ubx::config::ubx_config_repository>();

    ubx::config::ubx_config_manager manager(transport, ini_provider, repository);

    // ── Step 1 + 2: Load defaults, send VALGET poll ───────────────────────────

    std::printf("-- Steps 1 & 2: start_poll --\n");
    if (!manager.start_poll("/etc/gnss_default_config.ini"))
    {
        std::fprintf(stderr, "Failed to start GNSS config sync\n");
        return 1;
    }

    // ── Step 3: Simulate receiving a VALGET response from the chip ────────────
    //
    // The response below is hand-crafted to deliberately have rate_meas = 500 ms
    // while the default is 1000 ms.  All other keys match, so only one VALSET
    // entry should be emitted in step 4.
    //
    // Payload layout (UBX-CFG-VALGET response):
    //   [version=0x01][layer=0x00][position=0x0000]
    //   [key_id LE 4B][value LE N B]  ...
    //
    // Key IDs (from ubx::config::cfg_key):
    //   rate_meas              0x30210001  → size 3 → 2 bytes
    //   navspg_dynmodel        0x20110021  → size 2 → 1 byte
    //   msgout_ubx_nav_pvt_uart1 0x20910007 → size 2 → 1 byte
    //   signal_gps_ena         0x1031001F  → size 1 → 1 byte

    std::printf("\n-- Step 3: on_valget_response (simulated) --\n");

    // Simulate a decoded VALGET response:
    //   rate_meas = 500 ms (DIFFERS from default 1000) → will trigger VALSET in step 4.
    //   All other keys match the defaults.
    ubx::parser::ubx_cfg_valget decoded_valget;
    decoded_valget.version  = 0x01u;
    decoded_valget.layer    = 0x00u;
    decoded_valget.position = 0x0000u;
    decoded_valget.entries  = {
        { 0x30210001u, 500u  },   // rate_meas = 500 ms  (differs from default 1000)
        { 0x20110021u, 6u    },   // navspg_dynmodel = 6  (matches)
        { 0x20910007u, 1u    },   // msgout_ubx_nav_pvt_uart1 = 1  (matches)
        { 0x1031001Fu, 1u    },   // signal_gps_ena = true  (matches)
    };
    manager.on_valget_response(decoded_valget);

    // ── Step 4: Compare defaults vs repository, send VALSET for diffs ─────────
    //
    // Only rate_meas differs (500 vs 1000), so a single VALSET should be sent.

    std::printf("\n-- Step 4: apply_pending_sync --\n");
    if (!manager.apply_pending_sync(ubx::config::config_layer::ram))
    {
        std::fprintf(stderr, "Failed to apply GNSS config sync\n");
        return 1;
    }

    // ── Ad-hoc set: update two parameters directly ────────────────────────────

    std::printf("\n-- Ad-hoc set_config (rate_meas=200 ms, dynmodel=4) --\n");
    manager.set_config(
    {
        { ubx::config::cfg_key::rate_meas,
          ubx::config::config_value(static_cast<uint16_t>(200u)) },

        { ubx::config::cfg_key::navspg_dynmodel,
          ubx::config::config_value(static_cast<uint8_t>(4u)) },
    },
    ubx::config::config_layer::ram);

    // ── Ad-hoc poll: query three keys from the chip ───────────────────────────

    std::printf("\n-- Ad-hoc poll_config (rate_meas, dynmodel, signal_gps_ena) --\n");
    manager.poll_config(
    {
        ubx::config::cfg_key::rate_meas,
        ubx::config::cfg_key::navspg_dynmodel,
        ubx::config::cfg_key::signal_gps_ena,
    });

    // ── Key-registry look-up demo ─────────────────────────────────────────────

    std::printf("\n-- Key registry look-up demo --\n");
    {
        uint32_t key_id = 0u;
        if (ubx::config::ubx_cfg_key_registry::lookup_by_name("rate_meas", key_id))
            std::printf("  rate_meas → 0x%08X\n", key_id);

        const char* name = ubx::config::ubx_cfg_key_registry::lookup_by_id(0x30210001u);
        if (name)
            std::printf("  0x30210001 → %s\n", name);

        std::printf("  registry size: %zu entries\n",
                    ubx::config::ubx_cfg_key_registry::table_size());
    }

    std::printf("\n=== config_demo done ===\n");
    return 0;
}
