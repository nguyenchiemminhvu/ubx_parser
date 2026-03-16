// handlers/db_rxm_measx_handler.h
//
// Message update handler for UBX-RXM-MEASX.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_rxm_measx.h"

namespace ubx
{
namespace database
{

class db_rxm_measx_handler : public i_msg_update_handler
{
public:
    db_rxm_measx_handler() = default;

    void update(const parser::ubx_rxm_measx& msg);

    msg_type handled_msg() const override { return MSG_UBX_RXM_MEASX; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_rxm_measx msg_{};
};

} // namespace database
} // namespace ubx
