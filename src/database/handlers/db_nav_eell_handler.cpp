// src/database/handlers/db_nav_eell_handler.cpp

#include "database/handlers/db_nav_eell_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_eell_handler::update(const parser::ubx_nav_eell& msg)
{
    msg_ = msg;
}

void db_nav_eell_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    // err_orient: raw [0.01 deg];
    store.set(DATA_UBX_SEMI_MAJOR_AXIS_ACC,         static_cast<double>(msg_.err_maj));
    store.set(DATA_UBX_SEMI_MINOR_AXIS_ACC,         static_cast<double>(msg_.err_min));
    store.set(DATA_UBX_SEMI_MAJOR_AXIS_ORIENTATION, static_cast<double>(msg_.err_orient) * 1e-2);

    mask.set(MSG_UBX_NAV_EELL);
}

} // namespace database
} // namespace ubx
