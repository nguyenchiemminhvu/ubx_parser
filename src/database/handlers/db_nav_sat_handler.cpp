// src/database/handlers/db_nav_sat_handler.cpp
//
// Maps ubx_nav_sat satellite records into the flat per-satellite field arrays.
// Up to MAX_SATELLITES_IN_VIEW records are written; extras are silently dropped.

#include "database/handlers/db_nav_sat_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

#include <algorithm>
#include <cstdint>

namespace ubx
{
namespace database
{

// NAV-SAT flags bitmask offsets (from u-blox spec).
static constexpr uint32_t FLAG_SV_USED      = (1u << 3);
static constexpr uint32_t FLAG_DIFF_CORR    = (1u << 6);
static constexpr uint32_t FLAG_SMOOTHED     = (1u << 7);
static constexpr uint32_t FLAG_ORB_SRC_MASK = (0x7u << 8);  // bits 10:8
static constexpr uint32_t FLAG_EPH_AVAIL    = (1u << 11);
static constexpr uint32_t FLAG_ALM_AVAIL    = (1u << 12);
static constexpr uint32_t FLAG_AN_AVAIL     = (1u << 13);
static constexpr uint32_t FLAG_SBAS_CORR    = (1u << 16);
static constexpr uint32_t FLAG_RTCM_CORR    = (1u << 17);
static constexpr uint32_t FLAG_HEALTH       = (0x3u << 4);  // bits 5:4; 1=healthy

void db_nav_sat_handler::update(const parser::ubx_nav_sat& msg)
{
    msg_ = msg;
}

void db_nav_sat_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const uint8_t count = static_cast<uint8_t>(
        std::min(static_cast<size_t>(msg_.num_svs),
                 static_cast<size_t>(MAX_SATELLITES_IN_VIEW)));

    store.set(DATA_SATELLITES_IN_VIEW, static_cast<uint32_t>(count));

    for (uint8_t i = 0u; i < count; ++i)
    {
        const parser::nav_sat_sv& sv = msg_.svs[i];

        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_GNSSID,     i), sv.gnss_id);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_SVID,       i), sv.sv_id);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_AZIMUTH,    i), sv.azim);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_ELEVATION,  i), sv.elev);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_CN0_RATIO,  i), sv.cno);

        bool alm_avail  = (sv.flags & FLAG_ALM_AVAIL) != 0u;
        bool eph_avail  = (sv.flags & FLAG_EPH_AVAIL) != 0u;
        bool health_ok  = ((sv.flags & FLAG_HEALTH) >> 4) == 1u;
        bool sv_used    = (sv.flags & FLAG_SV_USED)    != 0u;
        bool diff_corr  = (sv.flags & FLAG_DIFF_CORR)  != 0u;
        bool sbas_used  = (sv.flags & FLAG_SBAS_CORR)  != 0u;
        bool rtcm_used  = (sv.flags & FLAG_RTCM_CORR)  != 0u;

        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_ALMANAC,   i), alm_avail);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_EPHEMERIS, i), eph_avail);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_HEALTH,    i), health_ok);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_USED,      i), sv_used);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_DIFF_CORR, i), diff_corr);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_SBAS_USED, i), sbas_used);
        store.set(db_index_with_offset(DATA_SATELLITES_IN_VIEW_RTCM_USED, i), rtcm_used);
    }

    mask.set(MSG_UBX_NAV_SAT);
}

} // namespace database
} // namespace ubx
