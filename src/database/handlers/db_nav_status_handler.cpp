// src/database/handlers/db_nav_status_handler.cpp

#include "database/handlers/db_nav_status_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_nav_status_handler::update(const parser::ubx_nav_status& msg)
{
    msg_ = msg;
}

void db_nav_status_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    // ttff is in ms; store as-is.
    store.set(DATA_UBX_TTFF, msg_.ttff);

    mask.set(MSG_UBX_NAV_STATUS);
}

} // namespace database
} // namespace ubx
