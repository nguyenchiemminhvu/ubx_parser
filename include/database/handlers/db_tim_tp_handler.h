// handlers/db_tim_tp_handler.h
//
// Message update handler for UBX-TIM-TP.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_tim_tp.h"

namespace ubx
{
namespace database
{

class db_tim_tp_handler : public i_msg_update_handler
{
public:
    db_tim_tp_handler() = default;

    void update(const parser::ubx_tim_tp& msg);

    msg_type handled_msg() const override { return MSG_UBX_TIM_TP; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_tim_tp msg_{};
};

} // namespace database
} // namespace ubx
