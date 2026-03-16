// src/database/handlers/db_nav_timegps_handler.cpp

#include "database/handlers/db_nav_timegps_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_timegps_handler::update(const parser::ubx_nav_timegps& msg)
{
    msg_ = msg;
}

void db_nav_timegps_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    store.set(DATA_UBX_NAV_GPSTIME_TOW,          msg_.i_tow);
    store.set(DATA_UBX_NAV_GPSTIME_WEEK,         msg_.week);
    store.set(DATA_UBX_NAV_GPSTIME_LEAP_SECONDS, msg_.leap_s);

    mask.set(MSG_UBX_NAV_TIMEGPS);
}

} // namespace database
} // namespace ubx
