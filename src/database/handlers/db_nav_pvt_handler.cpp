// src/database/handlers/db_nav_pvt_handler.cpp
//
// Maps ubx_nav_pvt fields to database data_field entries.
// Field names and scaling factors mirror CProtocolUBX::ProcessNavPvt() exactly.

#include "database/handlers/db_nav_pvt_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

#include <ctime>

namespace ubx
{
namespace database
{

void db_nav_pvt_handler::update(const parser::ubx_nav_pvt& msg)
{
    msg_ = msg;
}

void db_nav_pvt_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_nav_pvt& p = msg_;

    store.set(DATA_UBX_NAV_PVT_ITOW,                     p.i_tow);
    store.set(DATA_UBX_NAV_PVT_FIXTYPE,                  static_cast<uint8_t>(p.fix_type));
    store.set(DATA_UBX_NAV_PVT_FLAGS,                    p.flags);
    store.set(DATA_UBX_NAV_PVT_FLAGS2,                   p.flags2);
    store.set(DATA_UBX_NAV_PVT_GSPEED,                   static_cast<float>(p.g_speed));         // mm/s
    store.set(DATA_UBX_NAV_PVT_VEHICLE_HEADING_DEGREE,   static_cast<float>(p.head_veh) * 1e-5f); // deg
    store.set(DATA_UBX_NAV_PVT_MOTION_HEADING_DEGREE,    static_cast<float>(p.head_mot) * 1e-5f); // deg
    store.set(DATA_UBX_NAV_PVT_MAGNETIC_DECLINATION,     static_cast<float>(p.mag_dec)  * 1e-2f); // deg

    store.set(DATA_UBX_NAV_PVT_LAT,   static_cast<double>(p.lat)    * 1e-7); // deg
    store.set(DATA_UBX_NAV_PVT_LON,   static_cast<double>(p.lon)    * 1e-7); // deg
    store.set(DATA_UBX_NAV_PVT_ALT,   static_cast<double>(p.height));        // mm
    store.set(DATA_UBX_NAV_PVT_HMSL,  static_cast<double>(p.h_msl));         // mm

    store.set(DATA_UBX_NAV_PVT_T_ACC,    p.t_acc);
    store.set(DATA_UBX_NAV_PVT_H_ACC,    static_cast<float>(p.h_acc));  // mm
    store.set(DATA_UBX_NAV_PVT_V_ACC,    static_cast<float>(p.v_acc));  // mm
    store.set(DATA_UBX_NAV_PVT_S_ACC,    static_cast<float>(p.s_acc));  // mm/s
    store.set(DATA_UBX_NAV_PVT_HEAD_ACC, static_cast<float>(p.head_acc) * 1e-5f); // deg

    store.set(DATA_UBX_NAV_PVT_VEL_N, static_cast<float>(p.vel_n)); // mm/s
    store.set(DATA_UBX_NAV_PVT_VEL_E, static_cast<float>(p.vel_e)); // mm/s
    store.set(DATA_UBX_NAV_PVT_VEL_D, static_cast<float>(p.vel_d)); // mm/s

    store.set(DATA_UBX_NAV_PVT_NUMSV,       static_cast<uint32_t>(p.num_sv));
    store.set(DATA_UBX_NAV_PVT_YEAR,         p.year);
    store.set(DATA_UBX_NAV_PVT_MONTH,        p.month);
    store.set(DATA_UBX_NAV_PVT_DAY,          p.day);
    store.set(DATA_UBX_NAV_PVT_HOUR,         p.hour);
    store.set(DATA_UBX_NAV_PVT_MINUTE,       p.min);
    store.set(DATA_UBX_NAV_PVT_SECOND,       p.sec);
    store.set(DATA_UBX_NAV_PVT_MILI_SECOND,  p.i_tow % 1000u);
    store.set(DATA_UBX_NAV_PVT_MICRO_SECOND, p.nano / 1000); // ns → µs
    store.set(DATA_UBX_NAV_PVT_TIME_VALID,   p.valid);

    // Compute UTC Unix timestamp [ms]
    struct tm t{};
    t.tm_year   = (p.year >= 1900u) ? static_cast<int>(p.year - 1900u) : static_cast<int>(p.year);
    t.tm_mon    = (p.month >= 1u)   ? static_cast<int>(p.month - 1u)   : static_cast<int>(p.month);
    t.tm_mday   = static_cast<int>(p.day);
    t.tm_hour   = static_cast<int>(p.hour);
    t.tm_min    = static_cast<int>(p.min);
    t.tm_sec    = static_cast<int>(p.sec);
    t.tm_isdst  = -1;
    time_t epoch_sec  = timegm(&t);
    int64_t timestamp = static_cast<int64_t>(epoch_sec) * 1000
                        + static_cast<int64_t>(p.i_tow % 1000u);
    store.set(DATA_UBX_NAV_PVT_TIMESTAMP, timestamp);

    mask.set(MSG_UBX_NAV_PVT);
}

} // namespace database
} // namespace ubx
