// src/database/handlers/db_nav_odo_handler.cpp

#include "database/handlers/db_nav_odo_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_odo_handler::update(const parser::ubx_nav_odo& msg)
{
    msg_ = msg;
}

void db_nav_odo_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    store.set(DATA_UBX_NAV_ODO_ITOW,           msg_.i_tow);
    store.set(DATA_UBX_NAV_ODO_DISTANCE,       msg_.distance);        // m
    store.set(DATA_UBX_NAV_ODO_TOTAL_DISTANCE, msg_.total_distance);  // m
    store.set(DATA_UBX_NAV_ODO_DISTANCE_STD,   msg_.distance_std);    // m

    mask.set(MSG_UBX_NAV_ODO);
}

} // namespace database
} // namespace ubx
