// handlers/db_mon_io_handler.h
//
// Message update handler for UBX-MON-IO.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_mon_io.h"

namespace ubx
{
namespace database
{

class db_mon_io_handler : public i_msg_update_handler
{
public:
    db_mon_io_handler() = default;

    void update(const parser::ubx_mon_io& msg);

    msg_type handled_msg() const override { return MSG_UBX_MON_IO; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_mon_io msg_{};
};

} // namespace database
} // namespace ubx
