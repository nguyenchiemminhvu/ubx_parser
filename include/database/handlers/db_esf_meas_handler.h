// handlers/db_esf_meas_handler.h
//
// Message update handler for UBX-ESF-MEAS.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_esf_meas.h"

namespace ubx
{
namespace database
{

class db_esf_meas_handler : public i_msg_update_handler
{
public:
    db_esf_meas_handler() = default;

    void update(const parser::ubx_esf_meas& msg);

    msg_type handled_msg() const override { return MSG_UBX_ESF_MEAS; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_esf_meas msg_{};
};

} // namespace database
} // namespace ubx
