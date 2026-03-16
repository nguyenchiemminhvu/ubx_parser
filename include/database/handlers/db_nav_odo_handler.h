// handlers/db_nav_odo_handler.h
//
// Message update handler for UBX-NAV-ODO.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav_odo.h"

namespace ubx
{
namespace database
{

class db_nav_odo_handler : public i_msg_update_handler
{
public:
    db_nav_odo_handler() = default;

    void update(const parser::ubx_nav_odo& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV_ODO; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav_odo msg_{};
};

} // namespace database
} // namespace ubx
