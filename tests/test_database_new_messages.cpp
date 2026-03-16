// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_database_new_messages.cpp
//
// Unit tests for the 8 new database update handlers:
//   db_nav2_pvt_handler, db_nav2_eell_handler, db_nav2_dop_handler,
//   db_nav2_timegps_handler, db_nav_odo_handler,
//   db_esf_ins_handler, db_esf_status_handler, db_esf_meas_handler

#include <cassert>
#include <cstdio>
#include <cmath>

#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

#include "database/handlers/db_nav2_pvt_handler.h"
#include "database/handlers/db_nav2_eell_handler.h"
#include "database/handlers/db_nav2_dop_handler.h"
#include "database/handlers/db_nav2_timegps_handler.h"
#include "database/handlers/db_nav_odo_handler.h"
#include "database/handlers/db_esf_ins_handler.h"
#include "database/handlers/db_esf_status_handler.h"
#include "database/handlers/db_esf_meas_handler.h"

#include "messages/ubx_nav2_pvt.h"
#include "messages/ubx_nav2_eell.h"
#include "messages/ubx_nav2_dop.h"
#include "messages/ubx_nav2_timegps.h"
#include "messages/ubx_nav_odo.h"
#include "messages/ubx_esf_ins.h"
#include "messages/ubx_esf_status.h"
#include "messages/ubx_esf_meas.h"
#include "messages/ubx_esf_sensor_types.h"

using ubx::database::ubx_field_store;
using ubx::database::ubx_msg_mask;

static void pass(const char* name) { std::printf("[PASS] %s\n", name); }
static void fail(const char* name, const char* r) { std::printf("[FAIL] %s: %s\n", name, r); }

// ─── helper: floating-point equality with tolerance ──────────────────────────

static bool approx(double a, double b, double tol = 1e-9)
{
    return std::fabs(a - b) <= tol;
}

// ─────────────────────────────────────────────────────────────────────────────
// NAV2-PVT handler
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_nav2_pvt()
{
    const char* name = "db_nav2_pvt_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_nav2_pvt_handler handler;

    ubx::parser::ubx_nav2_pvt msg{};
    msg.i_tow    = 200000u;
    msg.fix_type = ubx::parser::nav2_pvt_fix_type::fix_3d;
    msg.flags    = 0x01u;
    msg.flags2   = 0x02u;
    msg.num_sv   = 10u;
    msg.lat      =  100000000;   // 10.0000000 deg (×1e-7)
    msg.lon      = 1060000000;   // 106.0000000 deg
    msg.height   = 30000;        // 30 000 mm
    msg.h_msl    = 28000;
    msg.h_acc    = 500u;
    msg.v_acc    = 700u;
    msg.g_speed  = 550;          // mm/s
    msg.head_veh = 4500000;      // 45.00000 deg (×1e-5)
    msg.head_mot = 4400000;
    msg.mag_dec  = 200;          // 2.00 deg (×1e-2)
    msg.vel_n    = 100;
    msg.vel_e    = 200;
    msg.vel_d    = -50;

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t itow = 0u; double lat = 0.0, lon = 0.0;
    float gspd = 0.f, head_veh = 0.f;

    if (!store.get(ubx::database::DATA_UBX_NAV2_PVT_ITOW, itow)
        || itow != 200000u)               { fail(name, "iTOW"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV2_PVT_LAT, lat)
        || !approx(lat, 10.0))            { fail(name, "lat"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV2_PVT_LON, lon)
        || !approx(lon, 106.0))           { fail(name, "lon"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV2_PVT_GSPEED, gspd)
        || gspd != 550.f)                 { fail(name, "gspeed"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV2_PVT_VEHICLE_HEADING_DEGREE, head_veh)
        || std::fabs(head_veh - 45.0f) > 1e-3f) { fail(name, "head_veh"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_NAV2_PVT)) { fail(name, "mask not set"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// NAV2-EELL handler
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_nav2_eell()
{
    const char* name = "db_nav2_eell_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_nav2_eell_handler handler;

    ubx::parser::ubx_nav2_eell msg{};
    msg.i_tow      = 300000u;
    msg.err_maj    = 1500u;
    msg.err_min    = 800u;
    msg.err_orient = 45u;

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t maj = 0u, mino = 0u;
    if (!store.get(ubx::database::DATA_UBX_NAV2_SEMI_MAJOR_AXIS_ACC, maj)
        || maj != 1500u)              { fail(name, "err_maj"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV2_SEMI_MINOR_AXIS_ACC, mino)
        || mino != 800u)              { fail(name, "err_min"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_NAV2_EELL)) { fail(name, "mask"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// NAV2-DOP handler
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_nav2_dop()
{
    const char* name = "db_nav2_dop_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_nav2_dop_handler handler;

    ubx::parser::ubx_nav2_dop msg{};
    msg.i_tow = 400000u;
    msg.g_dop = 120u;   // stored raw; consumer scales ×0.01 → 1.20
    msg.p_dop = 150u;
    msg.h_dop = 100u;
    msg.v_dop = 110u;

    handler.update(msg);
    handler.handle(store, mask);

    uint16_t gdop = 0u, pdop = 0u;
    if (!store.get(ubx::database::DATA_UBX_NAV2_GEOMETRIC_DILUTION_OF_PRECISION, gdop)
        || gdop != 120u)              { fail(name, "g_dop"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV2_POSITION_DILUTION_OF_PRECISION, pdop)
        || pdop != 150u)              { fail(name, "p_dop"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_NAV2_DOP)) { fail(name, "mask"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// NAV2-TIMEGPS handler
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_nav2_timegps()
{
    const char* name = "db_nav2_timegps_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_nav2_timegps_handler handler;

    ubx::parser::ubx_nav2_timegps msg{};
    msg.i_tow   = 500000u;
    msg.week    = 2300u;
    msg.leap_s  = 18;

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t itow = 0u;
    if (!store.get(ubx::database::DATA_UBX_NAV2_GPSTIME_TOW, itow)
        || itow != 500000u)                        { fail(name, "iTOW"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_NAV2_TIMEGPS)) { fail(name, "mask"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// NAV-ODO handler
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_nav_odo()
{
    const char* name = "db_nav_odo_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_nav_odo_handler handler;

    ubx::parser::ubx_nav_odo msg{};
    msg.i_tow          = 600000u;
    msg.distance       = 12345u;
    msg.total_distance = 67890u;
    msg.distance_std   = 5u;

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t dist = 0u, total = 0u, std_ = 0u;
    if (!store.get(ubx::database::DATA_UBX_NAV_ODO_DISTANCE, dist)
        || dist != 12345u)                     { fail(name, "distance"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV_ODO_TOTAL_DISTANCE, total)
        || total != 67890u)                    { fail(name, "total_distance"); return; }
    if (!store.get(ubx::database::DATA_UBX_NAV_ODO_DISTANCE_STD, std_)
        || std_ != 5u)                         { fail(name, "distance_std"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_NAV_ODO)) { fail(name, "mask"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// ESF-INS handler
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_esf_ins()
{
    const char* name = "db_esf_ins_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_esf_ins_handler handler;

    ubx::parser::ubx_esf_ins msg{};
    msg.i_tow      = 700000u;
    msg.bitfield0  = 0x00007u;
    msg.x_ang_rate = 100;
    msg.y_ang_rate = -200;
    msg.z_ang_rate = 300;
    msg.x_accel    = -9800;
    msg.y_accel    = 0;
    msg.z_accel    = 50;

    handler.update(msg);
    handler.handle(store, mask);

    int32_t x_ang = 0, y_ang = 0, x_acc = 0;
    uint32_t itow = 0u;
    if (!store.get(ubx::database::DATA_UBX_ESF_INS_ITOW, itow)
        || itow != 700000u)                    { fail(name, "iTOW"); return; }
    if (!store.get(ubx::database::DATA_UBX_ESF_INS_X_ANG_RATE, x_ang)
        || x_ang != 100)                       { fail(name, "x_ang_rate"); return; }
    if (!store.get(ubx::database::DATA_UBX_ESF_INS_Y_ANG_RATE, y_ang)
        || y_ang != -200)                      { fail(name, "y_ang_rate"); return; }
    if (!store.get(ubx::database::DATA_UBX_ESF_INS_X_ACCEL, x_acc)
        || x_acc != -9800)                     { fail(name, "x_accel"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_ESF_INS)) { fail(name, "mask"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// ESF-STATUS handler — header fields
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_esf_status()
{
    const char* name = "db_esf_status_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_esf_status_handler handler;

    ubx::parser::ubx_esf_status msg{};
    msg.i_tow       = 800000u;
    msg.fusion_mode = 3u;
    msg.num_sens    = 4u;

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t itow = 0u; uint8_t fmode = 0u, nsens = 0u;
    if (!store.get(ubx::database::DATA_UBX_ESF_STATUS_ITOW, itow)
        || itow != 800000u)                       { fail(name, "iTOW"); return; }
    if (!store.get(ubx::database::DATA_UBX_ESF_STATUS_FUSION_MODE, fmode)
        || fmode != 3u)                           { fail(name, "fusion_mode"); return; }
    if (!store.get(ubx::database::DATA_UBX_ESF_STATUS_NUM_SENS, nsens)
        || nsens != 4u)                           { fail(name, "num_sens"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_ESF_STATUS)) { fail(name, "mask"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// ESF-STATUS handler — per-sensor array fields
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_esf_status_per_sensor_arrays()
{
    const char* name = "db_esf_status_handler_per_sensor_arrays";
    using namespace ubx::database;
    using namespace ubx::parser;

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_esf_status_handler handler;

    // Sensor 0: x-axis accelerometer, used, ready, calibrated (calib=3), time_status=2
    //   sens_status1 = ESF_SENSOR_X_ACCEL(14) | USED(0x20) | READY(0x40) = 0x5E
    //   sens_status2 = calib(3) | (time_status(2) << 2) = 0x03 | 0x08 = 0x0B
    ubx_esf_status msg{};
    msg.i_tow       = 100u;
    msg.fusion_mode = 1u;
    msg.num_sens    = 2u;

    msg.sensors[0].sens_status1  =
        static_cast<uint8_t>(ESF_SENSOR_X_ACCEL)
        | ubx_esf_status_sensor::USED
        | ubx_esf_status_sensor::READY;    // 14 | 0x20 | 0x40 = 0x5E
    msg.sensors[0].sens_status2  = 3u | (2u << 2u);  // calib=3, time_status=2 → 0x0B
    msg.sensors[0].freq          = 50u;
    msg.sensors[0].faults        = 0u;

    // Sensor 1: z-axis gyro, not used, not ready, not calibrated, freq=100
    msg.sensors[1].sens_status1  = static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE);  // 5
    msg.sensors[1].sens_status2  = 0u;
    msg.sensors[1].freq          = 100u;
    msg.sensors[1].faults        = 0x05u;  // two fault bits

    handler.update(msg);
    handler.handle(store, mask);

    // ── sensor[0] ──
    uint8_t stype = 0u, sused = 0u, sready = 0u, scalib = 0u, stime = 0u, sfreq = 0u, sfaults = 0u;

    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_TYPE,        0u), stype)
        || stype != static_cast<uint8_t>(ESF_SENSOR_X_ACCEL))  { fail(name, "sens[0] type"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_USED,        0u), sused)
        || sused != 1u)                                        { fail(name, "sens[0] used"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_READY,       0u), sready)
        || sready != 1u)                                       { fail(name, "sens[0] ready"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_CALIB,       0u), scalib)
        || scalib != 3u)                                       { fail(name, "sens[0] calib"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_TIME_STATUS, 0u), stime)
        || stime != 2u)                                        { fail(name, "sens[0] time_status"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_FREQ,        0u), sfreq)
        || sfreq != 50u)                                       { fail(name, "sens[0] freq"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_FAULTS,      0u), sfaults)
        || sfaults != 0u)                                      { fail(name, "sens[0] faults"); return; }

    // ── sensor[1] ──
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_TYPE,  1u), stype)
        || stype != static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE))  { fail(name, "sens[1] type"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_USED,  1u), sused)
        || sused != 0u)                                                  { fail(name, "sens[1] used"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_READY, 1u), sready)
        || sready != 0u)                                                 { fail(name, "sens[1] ready"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_FREQ,  1u), sfreq)
        || sfreq != 100u)                                                { fail(name, "sens[1] freq"); return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_FAULTS,1u), sfaults)
        || sfaults != 0x05u)                                             { fail(name, "sens[1] faults"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// esf_sensor_type enum — value correctness (spec §3.2.7.5)
// ─────────────────────────────────────────────────────────────────────────────

static void test_esf_sensor_type_enum_values()
{
    const char* name = "esf_sensor_type_enum_values";
    using namespace ubx::parser;

    // Verify a representative subset against the ZED-F9R spec table.
    if (ESF_SENSOR_NONE                    != 0u)  { fail(name, "NONE");             return; }
    if (ESF_SENSOR_Z_GYRO_ANG_RATE         != 5u)  { fail(name, "Z_GYRO");           return; }
    if (ESF_SENSOR_FRONT_LEFT_WHEEL_TICKS  != 6u)  { fail(name, "FL_WHEEL");         return; }
    if (ESF_SENSOR_FRONT_RIGHT_WHEEL_TICKS != 7u)  { fail(name, "FR_WHEEL");         return; }
    if (ESF_SENSOR_REAR_LEFT_WHEEL_TICKS   != 8u)  { fail(name, "RL_WHEEL");         return; }
    if (ESF_SENSOR_REAR_RIGHT_WHEEL_TICKS  != 9u)  { fail(name, "RR_WHEEL");         return; }
    if (ESF_SENSOR_SPEED                   != 10u) { fail(name, "SPEED");            return; }
    if (ESF_SENSOR_GYRO_TEMP               != 11u) { fail(name, "GYRO_TEMP");        return; }
    if (ESF_SENSOR_X_GYRO_ANG_RATE         != 12u) { fail(name, "X_GYRO");           return; }
    if (ESF_SENSOR_Y_GYRO_ANG_RATE         != 13u) { fail(name, "Y_GYRO");           return; }
    if (ESF_SENSOR_X_ACCEL                 != 14u) { fail(name, "X_ACCEL");          return; }
    if (ESF_SENSOR_Y_ACCEL                 != 16u) { fail(name, "Y_ACCEL");          return; }
    if (ESF_SENSOR_Z_ACCEL                 != 17u) { fail(name, "Z_ACCEL");          return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// ESF-MEAS handler — header fields
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_esf_meas()
{
    const char* name = "db_esf_meas_handler_fields";

    ubx_field_store store;
    ubx_msg_mask    mask;
    ubx::database::db_esf_meas_handler handler;

    ubx::parser::ubx_esf_meas msg{};
    msg.time_tag = 900000u;
    msg.flags    = 0x0004u;
    msg.num_meas = 5u;

    handler.update(msg);
    handler.handle(store, mask);

    uint32_t ttag = 0u; uint16_t flags = 0u; uint8_t nmeas = 0u;
    if (!store.get(ubx::database::DATA_UBX_ESF_MEAS_TIME_TAG, ttag)
        || ttag != 900000u)                      { fail(name, "time_tag"); return; }
    if (!store.get(ubx::database::DATA_UBX_ESF_MEAS_FLAGS, flags)
        || flags != 0x0004u)                     { fail(name, "flags"); return; }
    if (!store.get(ubx::database::DATA_UBX_ESF_MEAS_NUM_MEAS, nmeas)
        || nmeas != 5u)                          { fail(name, "num_meas"); return; }
    if (!mask.is_set(ubx::database::MSG_UBX_ESF_MEAS)) { fail(name, "mask"); return; }

    pass(name);
}

// ─────────────────────────────────────────────────────────────────────────────
// ESF-MEAS handler — per-measurement array fields
// ─────────────────────────────────────────────────────────────────────────────

static void test_db_esf_meas_per_measurement_arrays()
{
    const char* name = "db_esf_meas_handler_per_measurement_arrays";
    using namespace ubx::database;
    using namespace ubx::parser;

    ubx_field_store store;
    ubx_msg_mask    mask;
    db_esf_meas_handler handler;

    ubx_esf_meas msg{};
    msg.time_tag = 1000u;
    msg.flags    = 0u;
    msg.num_meas = 3u;

    // meas[0]: x-axis accelerometer, positive value
    msg.data[0].data_type  = static_cast<uint8_t>(ESF_SENSOR_X_ACCEL);  // 14
    msg.data[0].data_value = 4096;

    // meas[1]: z-axis gyro angular rate, negative value (sign-extended from 24-bit)
    msg.data[1].data_type  = static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE);  // 5
    msg.data[1].data_value = -512;

    // meas[2]: speed, zero
    msg.data[2].data_type  = static_cast<uint8_t>(ESF_SENSOR_SPEED);  // 10
    msg.data[2].data_value = 0;

    handler.update(msg);
    handler.handle(store, mask);

    uint8_t dtype = 0u;
    int32_t dval  = 0;

    // ── meas[0] ──
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_TYPE,  0u), dtype)
        || dtype != static_cast<uint8_t>(ESF_SENSOR_X_ACCEL)) { fail(name, "meas[0] type");  return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_VALUE, 0u), dval)
        || dval != 4096)                                       { fail(name, "meas[0] value"); return; }

    // ── meas[1] ──
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_TYPE,  1u), dtype)
        || dtype != static_cast<uint8_t>(ESF_SENSOR_Z_GYRO_ANG_RATE)) { fail(name, "meas[1] type");  return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_VALUE, 1u), dval)
        || dval != -512)                                               { fail(name, "meas[1] value"); return; }

    // ── meas[2] ──
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_TYPE,  2u), dtype)
        || dtype != static_cast<uint8_t>(ESF_SENSOR_SPEED)) { fail(name, "meas[2] type");  return; }
    if (!store.get(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_VALUE, 2u), dval)
        || dval != 0)                                        { fail(name, "meas[2] value"); return; }

    if (!mask.is_set(MSG_UBX_ESF_MEAS)) { fail(name, "mask"); return; }

    pass(name);
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::puts("=== test_database_new_messages ===");
    test_db_nav2_pvt();
    test_db_nav2_eell();
    test_db_nav2_dop();
    test_db_nav2_timegps();
    test_db_nav_odo();
    test_db_esf_ins();
    test_db_esf_status();
    test_db_esf_status_per_sensor_arrays();
    test_esf_sensor_type_enum_values();
    test_db_esf_meas();
    test_db_esf_meas_per_measurement_arrays();
    return 0;
}
