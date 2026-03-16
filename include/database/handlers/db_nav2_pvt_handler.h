// handlers/db_nav2_pvt_handler.h
//
// Message update handler for UBX-NAV2-PVT.
//
// Holds the most-recently decoded ubx_nav2_pvt and, when handle() is called,
// maps every field to the corresponding data_field entry in the store.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav2_pvt.h"

namespace ubx
{
namespace database
{

class db_nav2_pvt_handler : public i_msg_update_handler
{
public:
    db_nav2_pvt_handler() = default;

    void update(const parser::ubx_nav2_pvt& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV2_PVT; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav2_pvt msg_{};
};

} // namespace database
} // namespace ubx
