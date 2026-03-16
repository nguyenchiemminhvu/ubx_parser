// handlers/db_nav_clock_handler.h
//
// Message update handler for UBX-NAV-CLOCK.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav_clock.h"

namespace ubx
{
namespace database
{

class db_nav_clock_handler : public i_msg_update_handler
{
public:
    db_nav_clock_handler() = default;

    void update(const parser::ubx_nav_clock& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV_CLOCK; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav_clock msg_{};
};

} // namespace database
} // namespace ubx
