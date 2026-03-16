// handlers/db_nav_sat_handler.h
//
// Message update handler for UBX-NAV-SAT.
//
// Mirrors the old CProtocolUBX::ProcessNavSat() field assignments, writing up
// to MAX_SATELLITES_IN_VIEW satellite records into the flat field store.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav_sat.h"

namespace ubx
{
namespace database
{

class db_nav_sat_handler : public i_msg_update_handler
{
public:
    db_nav_sat_handler() = default;

    void update(const parser::ubx_nav_sat& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV_SAT; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav_sat msg_{};
};

} // namespace database
} // namespace ubx
