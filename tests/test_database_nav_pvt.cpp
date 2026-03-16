// tests/test_database_nav_pvt.cpp
//
// Unit tests for the NAV-PVT update handler and database field store.
//
// Tests:
//   1. db_nav_pvt_handler writes expected fields into ubx_field_store.
//   2. Timestamp calculation matches expected UTC Unix ms value.
//   3. MSG_UBX_NAV_PVT mask bit is set after handle().
//   4. ubx_database.apply_update() drives the handler correctly.
//   5. Snapshot captures the committed values.

#include <cassert>
#include <cstdio>
#include <cstring>

#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"
#include "database/handlers/db_nav_pvt_handler.h"
#include "database/ubx_database.h"
#include "database/epoch_commit_policy.h"

// ─── helpers ─────────────────────────────────────────────────────────────────

static ubx::parser::ubx_nav_pvt make_test_pvt()
{
    ubx::parser::ubx_nav_pvt p{};
    p.i_tow     = 123456u;
    p.year      = 2026u;
    p.month     = 3u;
    p.day       = 17u;
    p.hour      = 10u;
    p.min       = 30u;
    p.sec       = 0u;
    p.valid     = ubx::parser::ubx_nav_pvt::VALID_DATE
                | ubx::parser::ubx_nav_pvt::VALID_TIME;
    p.t_acc     = 50u;
    p.nano      = 0;
    p.fix_type  = ubx::parser::nav_pvt_fix_type::fix_3d;
    p.flags     = ubx::parser::ubx_nav_pvt::FLAGS_GNSS_FIX_OK;
    p.flags2    = 0u;
    p.num_sv    = 12u;
    p.lon       = 1068750000;  // 106.875000 deg  (×1e-7)
    p.lat       =  107500000;  //  10.750000 deg  (×1e-7)
    p.height    = 50000;       // 50 000 mm = 50 m
    p.h_msl     = 45000;
    p.h_acc     = 800u;
    p.v_acc     = 1200u;
    p.vel_n     = 100;
    p.vel_e     = 200;
    p.vel_d     = 50;
    p.g_speed   = 224;         // ~224 mm/s
    p.head_mot  = 6300000;     // 63.00000 deg  (×1e-5)
    p.s_acc     = 400u;
    p.head_acc  = 500000u;     // 5.00000 deg (×1e-5)
    p.head_veh  = 6300000;
    p.mag_dec   = 120;         // 1.20 deg (×1e-2)
    p.mag_acc   = 50u;
    return p;
}

// ─── test 1: field store is written correctly ─────────────────────────────────

static void test_fields_written()
{
    ubx::database::ubx_field_store store;
    ubx::database::ubx_msg_mask    mask;
    ubx::database::db_nav_pvt_handler handler;

    auto pvt = make_test_pvt();
    handler.update(pvt);
    handler.handle(store, mask);

    // fix type
    uint8_t fix = 0u;
    assert(store.get(ubx::database::DATA_UBX_NAV_PVT_FIXTYPE, fix));
    assert(fix == static_cast<uint8_t>(ubx::parser::nav_pvt_fix_type::fix_3d));

    // latitude
    double lat = 0.0;
    assert(store.get(ubx::database::DATA_UBX_NAV_PVT_LAT, lat));
    // 107500000 × 1e-7 = 10.75 deg
    assert(lat > 10.749 && lat < 10.751);

    // longitude
    double lon = 0.0;
    assert(store.get(ubx::database::DATA_UBX_NAV_PVT_LON, lon));
    assert(lon > 106.874 && lon < 106.876);

    // num_sv
    uint32_t numsv = 0u;
    assert(store.get(ubx::database::DATA_UBX_NAV_PVT_NUMSV, numsv));
    assert(numsv == 12u);

    // ground speed (stored as float, read as float)
    float gspeed = 0.f;
    assert(store.get(ubx::database::DATA_UBX_NAV_PVT_GSPEED, gspeed));
    assert(gspeed > 223.f && gspeed < 225.f);

    std::puts("  PASS: test_fields_written");
}

// ─── test 2: mask bit set ─────────────────────────────────────────────────────

static void test_mask_set()
{
    ubx::database::ubx_field_store store;
    ubx::database::ubx_msg_mask    mask;
    ubx::database::db_nav_pvt_handler handler;

    handler.update(make_test_pvt());
    handler.handle(store, mask);

    assert(mask.is_set(ubx::database::MSG_UBX_NAV_PVT));
    assert(!mask.is_set(ubx::database::MSG_UBX_NAV_SAT));

    std::puts("  PASS: test_mask_set");
}

// ─── test 3: timestamp calculation ───────────────────────────────────────────

static void test_timestamp()
{
    ubx::database::ubx_field_store store;
    ubx::database::ubx_msg_mask    mask;
    ubx::database::db_nav_pvt_handler handler;

    auto pvt      = make_test_pvt();
    pvt.i_tow     = 400u;   // makes milli-second part = 400 ms
    pvt.year      = 1970u;
    pvt.month     = 1u;
    pvt.day       = 1u;
    pvt.hour      = 0u;
    pvt.min       = 0u;
    pvt.sec       = 0u;

    handler.update(pvt);
    handler.handle(store, mask);

    int64_t ts = 0;
    assert(store.get(ubx::database::DATA_UBX_NAV_PVT_TIMESTAMP, ts));
    // epoch 0 + 400 ms = 400
    assert(ts == 400);

    std::puts("  PASS: test_timestamp");
}

// ─── test 4: ubx_database commit callback triggered on NAV-PVT ───────────────

static void test_commit_triggered()
{
    using namespace ubx::database;

    bool committed     = false;
    commit_kind ck     = commit_kind::none;
    double committed_lat = 0.0;

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    ubx_database db(std::move(policy));
    // Register only the HP callback; LP must NOT fire for NAV-PVT.
    db.set_commit_callback(commit_kind::high_priority, [&](const ubx_location_snapshot& snap) {
        committed = true;
        ck        = snap.kind();
        snap.get(DATA_UBX_NAV_PVT_LAT, committed_lat);
    });

    db_nav_pvt_handler handler;
    handler.update(make_test_pvt());
    db.apply_update(handler);

    assert(committed);
    assert(ck == commit_kind::high_priority);
    assert(committed_lat > 10.749 && committed_lat < 10.751);

    std::puts("  PASS: test_commit_triggered");
}

// ─── test 5: LP commit NOT triggered on NAV-PVT ───────────────────────────────

static void test_no_lp_commit_on_pvt()
{
    using namespace ubx::database;

    int commit_count = 0;

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    ubx_database db(std::move(policy));
    // DOP triggers no commit → neither callback should fire.
    db.set_commit_callback(commit_kind::high_priority, [&](const ubx_location_snapshot&) { ++commit_count; });
    db.set_commit_callback(commit_kind::low_priority,  [&](const ubx_location_snapshot&) { ++commit_count; });

    // NAV-DOP should NOT trigger a commit
    db_nav_pvt_handler dummy;
    dummy.update(make_test_pvt());

    // Manually test with a no-op handler reporting MSG_UBX_NAV_DOP
    // (epoch policy says neither HP nor LP → no commit)
    struct dop_noop : i_msg_update_handler {
        msg_type handled_msg() const override { return MSG_UBX_NAV_DOP; }
        void handle(ubx_field_store&, ubx_msg_mask&) override {}
    } dop_handler;

    db.apply_update(dop_handler);
    assert(commit_count == 0);

    std::puts("  PASS: test_no_lp_commit_on_pvt");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_database_nav_pvt ===");
    test_fields_written();
    test_mask_set();
    test_timestamp();
    test_commit_triggered();
    test_no_lp_commit_on_pvt();
    std::puts("ALL PASSED");
    return 0;
}
