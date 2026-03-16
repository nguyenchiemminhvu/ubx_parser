// handlers/db_esf_ins_handler.h
//
// Message update handler for UBX-ESF-INS.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_esf_ins.h"

namespace ubx
{
namespace database
{

class db_esf_ins_handler : public i_msg_update_handler
{
public:
    db_esf_ins_handler() = default;

    void update(const parser::ubx_esf_ins& msg);

    msg_type handled_msg() const override { return MSG_UBX_ESF_INS; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_esf_ins msg_{};
};

} // namespace database
} // namespace ubx
