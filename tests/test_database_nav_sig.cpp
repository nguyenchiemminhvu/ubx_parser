// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_database_nav_sig.cpp
//
// Unit tests for db_nav_sig_handler:
//   1. Field store written correctly for all per-signal fields.
//   2. Signal flags decomposed correctly (health, pr_used, cr_used, do_used).
//   3. MSG_UBX_NAV_SIG mask bit is set after handle().
//   4. Count capped at MAX_ALL_GNSS_SIGNALS; extra signals are dropped.
//   5. Empty message (num_sigs = 0) writes zero count and sets mask.
//   6. Integration via ubx_database::apply_update() + snapshot.

#include <cassert>
#include <cstdio>
#include <cmath>

#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"
#include "database/handlers/db_nav_sig_handler.h"
#include "database/ubx_database.h"
#include "database/epoch_commit_policy.h"
#include "messages/ubx_nav_sig.h"

using ubx::database::ubx_field_store;
using ubx::database::ubx_msg_mask;
using ubx::database::db_nav_sig_handler;
using ubx::database::DATA_UBX_NAV_SIG_NUM_SIGS;
using ubx::database::DATA_UBX_NAV_SIG_GNSSID;
using ubx::database::DATA_UBX_NAV_SIG_SVID;
using ubx::database::DATA_UBX_NAV_SIG_SIGID;
using ubx::database::DATA_UBX_NAV_SIG_FREQID;
using ubx::database::DATA_UBX_NAV_SIG_PR_RES;
using ubx::database::DATA_UBX_NAV_SIG_CNO;
using ubx::database::DATA_UBX_NAV_SIG_QUAL_IND;
using ubx::database::DATA_UBX_NAV_SIG_CORR_SOURCE;
using ubx::database::DATA_UBX_NAV_SIG_IONO_MODEL;
using ubx::database::DATA_UBX_NAV_SIG_HEALTH;
using ubx::database::DATA_UBX_NAV_SIG_PR_USED;
using ubx::database::DATA_UBX_NAV_SIG_CR_USED;
using ubx::database::DATA_UBX_NAV_SIG_DO_USED;
using ubx::database::MSG_UBX_NAV_SIG;
using ubx::database::db_index_with_offset;

static void pass(const char* name) { std::printf("[PASS] %s\n", name); }
static void fail(const char* name, const char* r) { std::printf("[FAIL] %s: %s\n", name, r); }

// ─── helper ──────────────────────────────────────────────────────────────────

static ubx::parser::ubx_nav_sig make_test_msg(uint8_t num_sigs = 2u)
{
    ubx::parser::ubx_nav_sig msg{};
    msg.i_tow    = 390000u;
    msg.version  = 0u;
    msg.num_sigs = num_sigs;
    msg.signals.resize(num_sigs);

    for (uint8_t i = 0u; i < num_sigs; ++i)
    {
        ubx::parser::nav_sig_info& s = msg.signals[i];
        s.gnss_id     = static_cast<uint8_t>(i);
        s.sv_id       = static_cast<uint8_t>(i + 1u);
        s.sig_id      = static_cast<uint8_t>(i);
        s.freq_id     = 255u;   // unknown (non-GLONASS)
        s.pr_res      = static_cast<int16_t>(i * 10);
        s.cno         = static_cast<uint8_t>(30u + i);
        s.qual_ind    = 4u;
        s.corr_source = 0u;     // none
        s.iono_model  = 0u;     // none
        // sig_flags: bits 1:0 = 1 (healthy), bits 3,4,5 = 1,1,0
        s.sig_flags   = 0x0019u; // 0b0000_0000_0001_1001 → health=1, prUsed=1, crUsed=1
    }
    return msg;
}

// ─── test 1: basic field write ────────────────────────────────────────────────

static void test_db_nav_sig_fields()
{
    const char* name = "db_nav_sig_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_nav_sig_handler handler;

    auto msg = make_test_msg(2u);
    handler.update(msg);
    handler.handle(store, mask);

    uint32_t cnt = 0u;
    if (!store.get(DATA_UBX_NAV_SIG_NUM_SIGS, cnt) || cnt != 2u)
    { fail(name, "num_sigs"); return; }

    uint8_t gnss = 0u, sv = 0u, sig_id = 0u, cno = 0u, qual = 0u;
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_GNSSID,   0u), gnss) || gnss != 0u)
    { fail(name, "gnss_id[0]"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_SVID,     0u), sv)   || sv   != 1u)
    { fail(name, "sv_id[0]"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_SIGID,    0u), sig_id) || sig_id != 0u)
    { fail(name, "sig_id[0]"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_CNO,      0u), cno)  || cno  != 30u)
    { fail(name, "cno[0]"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_QUAL_IND, 0u), qual) || qual != 4u)
    { fail(name, "qual_ind[0]"); return; }

    // second signal
    uint8_t gnss1 = 0u, sv1 = 0u, cno1 = 0u;
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_GNSSID, 1u), gnss1) || gnss1 != 1u)
    { fail(name, "gnss_id[1]"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_SVID,   1u), sv1)   || sv1   != 2u)
    { fail(name, "sv_id[1]"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_CNO,    1u), cno1)  || cno1  != 31u)
    { fail(name, "cno[1]"); return; }

    pass(name);
}

// ─── test 2: signal flags decomposition ──────────────────────────────────────

static void test_db_nav_sig_flags()
{
    const char* name = "db_nav_sig_flags_decompose";

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_nav_sig_handler handler;

    ubx::parser::ubx_nav_sig msg{};
    msg.num_sigs = 1u;
    msg.signals.resize(1u);
    ubx::parser::nav_sig_info& s = msg.signals[0];
    s.gnss_id     = 0u;
    s.sv_id       = 1u;
    s.sig_id      = 0u;
    s.freq_id     = 255u;
    s.pr_res      = 0;
    s.cno         = 35u;
    s.qual_ind    = 7u;
    s.corr_source = 1u;  // SBAS
    s.iono_model  = 2u;  // SBAS
    // sig_flags: health=1 (bits 1:0), prUsed=1 (bit 3), crUsed=1 (bit 4), doUsed=1 (bit 5)
    // 0b0000_0000_0011_1001 = 0x0039
    s.sig_flags   = 0x0039u;

    handler.update(msg);
    handler.handle(store, mask);

    bool health = false, pr = false, cr = false, dou = false;
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_HEALTH,      0u), health) || !health)
    { fail(name, "health"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_PR_USED,     0u), pr)     || !pr)
    { fail(name, "pr_used"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_CR_USED,     0u), cr)     || !cr)
    { fail(name, "cr_used"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_DO_USED,     0u), dou)    || !dou)
    { fail(name, "do_used"); return; }

    // corr_source and iono_model
    uint8_t corr = 0u, iono = 0u;
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_CORR_SOURCE, 0u), corr) || corr != 1u)
    { fail(name, "corr_source"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_IONO_MODEL,  0u), iono) || iono != 2u)
    { fail(name, "iono_model"); return; }

    pass(name);
}

// ─── test 3: unhealthy / flags-all-zero signal ────────────────────────────────

static void test_db_nav_sig_flags_unhealthy()
{
    const char* name = "db_nav_sig_flags_unhealthy";

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_nav_sig_handler handler;

    ubx::parser::ubx_nav_sig msg{};
    msg.num_sigs = 1u;
    msg.signals.resize(1u);
    msg.signals[0].sig_flags = 0x0002u; // health=2 (unhealthy), all usage bits clear

    handler.update(msg);
    handler.handle(store, mask);

    bool health = true; // preset true, expect it to be cleared
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_HEALTH, 0u), health) || health)
    { fail(name, "unhealthy should set health=false"); return; }

    bool pr = true;
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_PR_USED, 0u), pr) || pr)
    { fail(name, "pr_used should be false"); return; }

    pass(name);
}

// ─── test 4: mask bit set ─────────────────────────────────────────────────────

static void test_db_nav_sig_mask()
{
    const char* name = "db_nav_sig_mask_set";

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_nav_sig_handler handler;

    auto msg = make_test_msg(1u);
    handler.update(msg);
    handler.handle(store, mask);

    if (!mask.is_set(MSG_UBX_NAV_SIG))
    { fail(name, "MSG_UBX_NAV_SIG mask bit not set"); return; }

    pass(name);
}

// ─── test 5: empty message ────────────────────────────────────────────────────

static void test_db_nav_sig_empty()
{
    const char* name = "db_nav_sig_empty_message";

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_nav_sig_handler handler;

    ubx::parser::ubx_nav_sig msg{};
    msg.num_sigs = 0u;

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t cnt = 99u;
    if (!store.get(DATA_UBX_NAV_SIG_NUM_SIGS, cnt) || cnt != 0u)
    { fail(name, "count should be 0 for empty message"); return; }
    if (!mask.is_set(MSG_UBX_NAV_SIG))
    { fail(name, "mask not set for empty message"); return; }

    pass(name);
}

// ─── test 6: count capped at MAX_ALL_GNSS_SIGNALS ────────────────────────────

static void test_db_nav_sig_count_capped()
{
    const char* name = "db_nav_sig_count_capped";

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_nav_sig_handler handler;

    // Build a message claiming more signals than the store can hold.
    const uint8_t over_limit = static_cast<uint8_t>(ubx::database::MAX_ALL_GNSS_SIGNALS);
    // Provide exactly MAX + 5 entries in the vector (num_sigs > MAX).
    ubx::parser::ubx_nav_sig msg{};
    msg.num_sigs = static_cast<uint8_t>(over_limit); // exactly MAX is fine
    msg.signals.resize(over_limit);
    for (uint8_t i = 0u; i < over_limit; ++i)
    {
        msg.signals[i].gnss_id  = 0u;
        msg.signals[i].sv_id    = i;
        msg.signals[i].cno      = 20u;
        msg.signals[i].sig_flags = 0x0001u; // healthy
    }

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t cnt = 0u;
    if (!store.get(DATA_UBX_NAV_SIG_NUM_SIGS, cnt)
        || cnt != static_cast<uint32_t>(over_limit))
    { fail(name, "count mismatch at MAX_ALL_GNSS_SIGNALS"); return; }

    pass(name);
}

// ─── test 7: pr_res signed value stored correctly ────────────────────────────

static void test_db_nav_sig_pr_res_signed()
{
    const char* name = "db_nav_sig_pr_res_signed";

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_nav_sig_handler handler;

    ubx::parser::ubx_nav_sig msg{};
    msg.num_sigs = 1u;
    msg.signals.resize(1u);
    msg.signals[0].pr_res = -150; // negative residual

    handler.update(msg);
    handler.handle(store, mask);

    int16_t pr = 0;
    if (!store.get(db_index_with_offset(DATA_UBX_NAV_SIG_PR_RES, 0u), pr) || pr != -150)
    { fail(name, "pr_res signed value mismatch"); return; }

    pass(name);
}

// ─── test 8: integration via ubx_database::apply_update + snapshot ────────────

static void test_db_nav_sig_database_integration()
{
    const char* name = "db_nav_sig_database_integration";

    bool committed = false;
    uint32_t snap_count = 0u;

    // Configure LP epoch trigger to MSG_UBX_NAV_SIG so apply_update fires the callback.
    auto policy = std::unique_ptr<ubx::database::epoch_commit_policy>(
        new ubx::database::epoch_commit_policy(
            ubx::database::MSG_UBX_NAV_PVT,
            ubx::database::MSG_UBX_NAV_SIG));

    auto db = std::make_shared<ubx::database::ubx_database>(std::move(policy));
    db->set_commit_callback(ubx::database::commit_kind::low_priority,
        [&](const ubx::database::ubx_location_snapshot& snap) {
            committed = true;
            snap.get(DATA_UBX_NAV_SIG_NUM_SIGS, snap_count);
        });

    db_nav_sig_handler handler;
    auto msg = make_test_msg(3u);
    handler.update(msg);
    db->apply_update(handler);

    if (!committed)
    { fail(name, "LP commit not triggered"); return; }
    if (snap_count != 3u)
    { fail(name, "snapshot num_sigs wrong"); return; }

    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_database_nav_sig ===\n");
    test_db_nav_sig_fields();
    test_db_nav_sig_flags();
    test_db_nav_sig_flags_unhealthy();
    test_db_nav_sig_mask();
    test_db_nav_sig_empty();
    test_db_nav_sig_count_capped();
    test_db_nav_sig_pr_res_signed();
    test_db_nav_sig_database_integration();
    std::printf("Done.\n");
    return 0;
}
