// handlers/db_nav_sig_handler.h
//
// Message update handler for UBX-NAV-SIG.
//
// Maps per-signal records from ubx_nav_sig into the flat per-signal field
// arrays. Up to MAX_ALL_GNSS_SIGNALS records are written; extras are dropped.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_nav_sig.h"

namespace ubx
{
namespace database
{

class db_nav_sig_handler : public i_msg_update_handler
{
public:
    db_nav_sig_handler() = default;

    void update(const parser::ubx_nav_sig& msg);

    msg_type handled_msg() const override { return MSG_UBX_NAV_SIG; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_nav_sig msg_{};
};

} // namespace database
} // namespace ubx
