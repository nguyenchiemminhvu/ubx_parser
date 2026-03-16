// handlers/db_nav_att_handler.h
//
// Message update handler for UBX-NAV-ATT.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav_att.h"

namespace ubx
{
namespace database
{

class db_nav_att_handler : public i_msg_update_handler
{
public:
    db_nav_att_handler() = default;

    void update(const parser::ubx_nav_att& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV_ATT; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav_att msg_{};
};

} // namespace database
} // namespace ubx
