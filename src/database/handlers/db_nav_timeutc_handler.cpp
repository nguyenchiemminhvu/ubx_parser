// src/database/handlers/db_nav_timeutc_handler.cpp
//
// Maps ubx_nav_timeutc fields to database data_field entries.

#include "database/handlers/db_nav_timeutc_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_timeutc_handler::update(const parser::ubx_nav_timeutc& msg)
{
    msg_ = msg;
}

void db_nav_timeutc_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_nav_timeutc& p = msg_;

    store.set(DATA_UBX_NAV_TIMEUTC_ITOW,  p.i_tow);
    store.set(DATA_UBX_NAV_TIMEUTC_T_ACC, p.t_acc);
    store.set(DATA_UBX_NAV_TIMEUTC_NANO,  p.nano);
    store.set(DATA_UBX_NAV_TIMEUTC_YEAR,  p.year);
    store.set(DATA_UBX_NAV_TIMEUTC_MONTH, p.month);
    store.set(DATA_UBX_NAV_TIMEUTC_DAY,   p.day);
    store.set(DATA_UBX_NAV_TIMEUTC_HOUR,  p.hour);
    store.set(DATA_UBX_NAV_TIMEUTC_MIN,   p.min);
    store.set(DATA_UBX_NAV_TIMEUTC_SEC,   p.sec);
    store.set(DATA_UBX_NAV_TIMEUTC_VALID, p.valid);

    mask.set(MSG_UBX_NAV_TIMEUTC);
}

} // namespace database
} // namespace ubx
