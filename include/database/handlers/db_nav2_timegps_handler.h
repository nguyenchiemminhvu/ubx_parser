// handlers/db_nav2_timegps_handler.h
//
// Message update handler for UBX-NAV2-TIMEGPS.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav2_timegps.h"

namespace ubx
{
namespace database
{

class db_nav2_timegps_handler : public i_msg_update_handler
{
public:
    db_nav2_timegps_handler() = default;

    void update(const parser::ubx_nav2_timegps& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV2_TIMEGPS; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav2_timegps msg_{};
};

} // namespace database
} // namespace ubx
