// handlers/db_nav_pvt_handler.h
//
// Message update handler for UBX-NAV-PVT.
//
// Holds the most-recently decoded ubx_nav_pvt and, when handle() is called,
// maps every field to the corresponding data_field entry in the store — exactly
// mirroring the old CProtocolUBX::ProcessNavPvt() field assignments.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav_pvt.h"

namespace ubx
{
namespace database
{

class db_nav_pvt_handler : public i_msg_update_handler
{
public:
    db_nav_pvt_handler() = default;

    // Called by the ubx_parser decoder stub to cache the latest message.
    void update(const parser::ubx_nav_pvt& msg);

    // i_msg_update_handler
    msg_type handled_msg() const override { return MSG_UBX_NAV_PVT; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav_pvt msg_{};
};

} // namespace database
} // namespace ubx
