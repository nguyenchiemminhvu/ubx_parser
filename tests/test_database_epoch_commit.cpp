// tests/test_database_epoch_commit.cpp
//
// Integration tests: stream of message updates → epoch commit → snapshot.
//
// Tests:
//   1. HP epoch fires after NAV-PVT, LP epoch fires after NAV-SAT.
//   2. Mask is reset between epochs.
//   3. Snapshot holds fields from the epoch at commit time (not from later writes).
//   4. Custom commit policy can be injected.
//   5. db_nav_sat_handler writes satellite count and per-SV fields.

#include <cassert>
#include <cstdio>
#include <vector>

#include "database/ubx_database.h"
#include "database/epoch_commit_policy.h"
#include "database/handlers/db_nav_pvt_handler.h"
#include "database/handlers/db_nav_sat_handler.h"
#include "database/handlers/db_nav_dop_handler.h"
#include "database/ubx_data_fields.h"
#include "database/ubx_location_snapshot.h"

using namespace ubx::database;

// ─── helpers ─────────────────────────────────────────────────────────────────

static ubx::parser::ubx_nav_pvt make_pvt(double lat_deg, uint8_t numsv)
{
    ubx::parser::ubx_nav_pvt p{};
    p.lat    = static_cast<int32_t>(lat_deg * 1e7);
    p.year   = 2026u; p.month = 1u; p.day = 1u;
    p.num_sv = numsv;
    p.fix_type = ubx::parser::nav_pvt_fix_type::fix_3d;
    return p;
}

static ubx::parser::ubx_nav_sat make_sat(uint8_t count)
{
    ubx::parser::ubx_nav_sat s{};
    s.num_svs = count;
    for (uint8_t i = 0; i < count; ++i)
    {
        ubx::parser::nav_sat_sv sv{};
        sv.gnss_id = 0u;
        sv.sv_id   = static_cast<uint8_t>(i + 1u);
        sv.cno     = static_cast<uint8_t>(30u + i);
        sv.elev    = 45;
        sv.azim    = static_cast<int16_t>(i * 10);
        sv.flags   = (1u << 3) | (1u << 11) | (1u << 12) | (1u << 4); // used, eph, alm, healthy
        s.svs.push_back(sv);
    }
    return s;
}

// ─── test 1: HP fires on NAV-PVT, LP fires on NAV-SAT ────────────────────────

static void test_hp_lp_sequence()
{
    std::vector<commit_kind> kinds;

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    ubx_database db(std::move(policy));
    db.set_commit_callback(commit_kind::high_priority, [&](const ubx_location_snapshot& s) {
        kinds.push_back(s.kind());
    });
    db.set_commit_callback(commit_kind::low_priority, [&](const ubx_location_snapshot& s) {
        kinds.push_back(s.kind());
    });

    db_nav_pvt_handler pvt_h;
    db_nav_sat_handler sat_h;

    pvt_h.update(make_pvt(10.0, 8));
    db.apply_update(pvt_h);   // → HP commit

    sat_h.update(make_sat(8));
    db.apply_update(sat_h);   // → LP commit

    assert(kinds.size() == 2u);
    assert(kinds[0] == commit_kind::high_priority);
    assert(kinds[1] == commit_kind::low_priority);

    std::puts("  PASS: test_hp_lp_sequence");
}

// ─── test 2: mask is reset between epochs ────────────────────────────────────

static void test_mask_reset_between_epochs()
{
    std::vector<uint64_t> masks;

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    ubx_database db(std::move(policy));
    db.set_commit_callback(commit_kind::high_priority, [&](const ubx_location_snapshot& s) {
        masks.push_back(s.msg_mask());
    });

    db_nav_dop_handler dop_h;
    ubx::parser::ubx_nav_dop dop{};
    dop_h.update(dop);
    db.apply_update(dop_h);   // sets DOP bit, no commit

    db_nav_pvt_handler pvt_h;
    pvt_h.update(make_pvt(10.0, 4));
    db.apply_update(pvt_h);   // sets PVT bit, HP commit → mask cleared

    // mask at first commit should have both DOP and PVT bits
    assert(masks.size() == 1u);
    uint64_t dop_bit = (uint64_t(1u) << MSG_UBX_NAV_DOP);
    uint64_t pvt_bit = (uint64_t(1u) << MSG_UBX_NAV_PVT);
    assert((masks[0] & dop_bit) != 0u);
    assert((masks[0] & pvt_bit) != 0u);

    std::puts("  PASS: test_mask_reset_between_epochs");
}

// ─── test 3: snapshot is independent of subsequent writes ─────────────────────

static void test_snapshot_independence()
{
    ubx_location_snapshot first_snap;
    int commit_count = 0;

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    ubx_database db(std::move(policy));
    db.set_commit_callback(commit_kind::high_priority, [&](const ubx_location_snapshot& s) {
        ++commit_count;
        if (commit_count == 1)
        {
            first_snap = s;  // capture only the first commit (lat = 20.0)
        }
    });

    db_nav_pvt_handler pvt_h;
    pvt_h.update(make_pvt(20.0, 6));
    db.apply_update(pvt_h);  // HP commit #1, lat = 20.0

    // Second update must not retroactively change the first snapshot copy.
    pvt_h.update(make_pvt(99.0, 6));
    db.apply_update(pvt_h);  // HP commit #2 (lat = 99.0)
    assert(commit_count == 2);

    // The copy stored in first_snap must still reflect lat=20.0.
    double snapped_lat = 0.0;
    first_snap.get(DATA_UBX_NAV_PVT_LAT, snapped_lat);
    assert(snapped_lat > 19.999 && snapped_lat < 20.001);

    std::puts("  PASS: test_snapshot_independence");
}

// ─── test 4: nav_sat fields written correctly ─────────────────────────────────

static void test_nav_sat_fields()
{
    ubx_location_snapshot lp_snap;

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    ubx_database db(std::move(policy));
    db.set_commit_callback(commit_kind::low_priority, [&](const ubx_location_snapshot& s) {
        lp_snap = s;
    });

    db_nav_sat_handler sat_h;
    sat_h.update(make_sat(5u));
    db.apply_update(sat_h);   // LP commit

    uint32_t count = 0u;
    assert(lp_snap.get(DATA_SATELLITES_IN_VIEW, count));
    assert(count == 5u);

    // SV 0: sv_id = 1, cno = 30, gnss_id = 0
    uint8_t sv_id = 0u;
    lp_snap.get(db_index_with_offset(DATA_SATELLITES_IN_VIEW_SVID, 0u), sv_id);
    assert(sv_id == 1u);

    uint8_t cno = 0u;
    lp_snap.get(db_index_with_offset(DATA_SATELLITES_IN_VIEW_CN0_RATIO, 0u), cno);
    assert(cno == 30u);

    // SV 0 should be marked used (flags bit 3 set in make_sat)
    bool used = false;
    lp_snap.get(db_index_with_offset(DATA_SATELLITES_IN_VIEW_USED, 0u), used);
    assert(used);

    std::puts("  PASS: test_nav_sat_fields");
}

// ─── test 5: custom policy (commit on NAV-DOP) ────────────────────────────────

static void test_custom_policy()
{
    struct dop_only_policy : i_commit_policy
    {
        commit_kind should_commit(msg_type msg) const override
        {
            return (msg == MSG_UBX_NAV_DOP) ? commit_kind::high_priority
                                             : commit_kind::none;
        }
    };

    int count = 0;
    ubx_database db(std::unique_ptr<i_commit_policy>(new dop_only_policy()));
    // Wire both HP and LP to the same counter.
    db.set_commit_callback(commit_kind::high_priority, [&](const ubx_location_snapshot&) { ++count; });
    db.set_commit_callback(commit_kind::low_priority,  [&](const ubx_location_snapshot&) { ++count; });

    db_nav_pvt_handler pvt_h;
    pvt_h.update(make_pvt(1.0, 4));
    db.apply_update(pvt_h);   // no commit
    assert(count == 0);

    db_nav_dop_handler dop_h;
    ubx::parser::ubx_nav_dop dop{};
    dop_h.update(dop);
    db.apply_update(dop_h);   // commit
    assert(count == 1);

    std::puts("  PASS: test_custom_policy");
}

// ─── test 6: per-kind callbacks are isolated (HP cb never fires on LP commit) ──

static void test_per_kind_isolation()
{
    int hp_count = 0;
    int lp_count = 0;

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    ubx_database db(std::move(policy));
    db.set_commit_callback(commit_kind::high_priority, [&](const ubx_location_snapshot&) { ++hp_count; });
    db.set_commit_callback(commit_kind::low_priority,  [&](const ubx_location_snapshot&) { ++lp_count; });

    db_nav_pvt_handler pvt_h;
    pvt_h.update(make_pvt(5.0, 4));
    db.apply_update(pvt_h);   // → HP commit only
    assert(hp_count == 1 && lp_count == 0);

    db_nav_sat_handler sat_h;
    sat_h.update(make_sat(3u));
    db.apply_update(sat_h);   // → LP commit only
    assert(hp_count == 1 && lp_count == 1);

    // A second NAV-PVT must increment HP only.
    pvt_h.update(make_pvt(6.0, 4));
    db.apply_update(pvt_h);
    assert(hp_count == 2 && lp_count == 1);

    std::puts("  PASS: test_per_kind_isolation");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_database_epoch_commit ===");
    test_hp_lp_sequence();
    test_mask_reset_between_epochs();
    test_snapshot_independence();
    test_nav_sat_fields();
    test_custom_policy();
    test_per_kind_isolation();
    std::puts("ALL PASSED");
    return 0;
}
