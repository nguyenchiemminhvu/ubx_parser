// src/database/handlers/db_nav_clock_handler.cpp
//
// Maps ubx_nav_clock fields to database data_field entries.

#include "database/handlers/db_nav_clock_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_clock_handler::update(const parser::ubx_nav_clock& msg)
{
    msg_ = msg;
}

void db_nav_clock_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_nav_clock& p = msg_;

    store.set(DATA_UBX_NAV_CLOCK_ITOW,  p.i_tow);
    store.set(DATA_UBX_NAV_CLOCK_CLKB,  static_cast<float>(p.clk_b));  // ns
    store.set(DATA_UBX_NAV_CLOCK_CLKD,  static_cast<float>(p.clk_d));  // ns/s
    store.set(DATA_UBX_NAV_CLOCK_T_ACC, p.t_acc);
    store.set(DATA_UBX_NAV_CLOCK_F_ACC, p.f_acc);

    mask.set(MSG_UBX_NAV_CLOCK);
}

} // namespace database
} // namespace ubx
