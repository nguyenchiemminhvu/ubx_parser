// handlers/db_nav2_eell_handler.h
//
// Message update handler for UBX-NAV2-EELL.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav2_eell.h"

namespace ubx
{
namespace database
{

class db_nav2_eell_handler : public i_msg_update_handler
{
public:
    db_nav2_eell_handler() = default;

    void update(const parser::ubx_nav2_eell& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV2_EELL; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav2_eell msg_{};
};

} // namespace database
} // namespace ubx
