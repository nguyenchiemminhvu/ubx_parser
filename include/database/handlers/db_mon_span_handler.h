// handlers/db_mon_span_handler.h
//
// Message update handler for UBX-MON-SPAN.
//
// Stores the PGA value for each RF block into the database field store using
// the contiguous DATA_UBX_MON_SPAN_PGA array field range.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_mon_span.h"

namespace ubx
{
namespace database
{

class db_mon_span_handler : public i_msg_update_handler
{
public:
    db_mon_span_handler() = default;

    void update(const parser::ubx_mon_span& msg);

    msg_type handled_msg() const override { return MSG_UBX_MON_SPAN; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_mon_span msg_{};
};

} // namespace database
} // namespace ubx
