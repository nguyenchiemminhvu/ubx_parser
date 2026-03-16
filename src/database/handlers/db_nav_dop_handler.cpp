// src/database/handlers/db_nav_dop_handler.cpp

#include "database/handlers/db_nav_dop_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_dop_handler::update(const parser::ubx_nav_dop& msg)
{
    msg_ = msg;
}

void db_nav_dop_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    // Raw values scaled ×0.01 per spec;
    store.set(DATA_UBX_NAV_GEOMETRIC_DILUTION_OF_PRECISION, static_cast<double>(msg_.g_dop) * 0.01);
    store.set(DATA_UBX_NAV_POSITION_DILUTION_OF_PRECISION,  static_cast<double>(msg_.p_dop) * 0.01);
    store.set(DATA_UBX_NAV_TIME_DILUTION_OF_PRECISION,      static_cast<double>(msg_.t_dop) * 0.01);
    store.set(DATA_UBX_NAV_VERTICAL_DILUTION_OF_PRECISION,  static_cast<double>(msg_.v_dop) * 0.01);
    store.set(DATA_UBX_NAV_HORIZONAL_DILUTION_OF_PRECISION, static_cast<double>(msg_.h_dop) * 0.01);
    mask.set(MSG_UBX_NAV_DOP);
}

} // namespace database
} // namespace ubx
