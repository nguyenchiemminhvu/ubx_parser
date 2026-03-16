// src/database/handlers/db_nav_att_handler.cpp
//
// Maps ubx_nav_att fields to database data_field entries.
// All angle fields are stored in degrees (scaled by 1e-5 per spec).

#include "database/handlers/db_nav_att_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_att_handler::update(const parser::ubx_nav_att& msg)
{
    msg_ = msg;
}

void db_nav_att_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_nav_att& p = msg_;

    store.set(DATA_UBX_NAV_ATT_ITOW,         p.i_tow);
    store.set(DATA_UBX_NAV_ATT_VERSION,      p.version);
    store.set(DATA_UBX_NAV_ATT_ROLL,         static_cast<float>(p.roll)        * 1e-5f);  // deg
    store.set(DATA_UBX_NAV_ATT_PITCH,        static_cast<float>(p.pitch)       * 1e-5f);  // deg
    store.set(DATA_UBX_NAV_ATT_HEADING,      static_cast<float>(p.heading)     * 1e-5f);  // deg
    store.set(DATA_UBX_NAV_ATT_ACC_ROLL,     static_cast<float>(p.acc_roll)    * 1e-5f);  // deg
    store.set(DATA_UBX_NAV_ATT_ACC_PITCH,    static_cast<float>(p.acc_pitch)   * 1e-5f);  // deg
    store.set(DATA_UBX_NAV_ATT_ACC_HEADING,  static_cast<float>(p.acc_heading) * 1e-5f);  // deg

    mask.set(MSG_UBX_NAV_ATT);
}

} // namespace database
} // namespace ubx
