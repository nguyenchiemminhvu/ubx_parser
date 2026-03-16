// handlers/db_esf_status_handler.h
//
// Message update handler for UBX-ESF-STATUS.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_esf_status.h"

namespace ubx
{
namespace database
{

class db_esf_status_handler : public i_msg_update_handler
{
public:
    db_esf_status_handler() = default;

    void update(const parser::ubx_esf_status& msg);

    msg_type handled_msg() const override { return MSG_UBX_ESF_STATUS; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_esf_status msg_{};
};

} // namespace database
} // namespace ubx
