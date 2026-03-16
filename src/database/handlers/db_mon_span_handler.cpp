// src/database/handlers/db_mon_span_handler.cpp
//
// Maps ubx_mon_span fields to database data_field entries.
// Only the PGA value per RF block is stored, as requested.

#include "database/handlers/db_mon_span_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_mon_span_handler::update(const parser::ubx_mon_span& msg)
{
    msg_ = msg;
}

void db_mon_span_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_mon_span& p = msg_;

    store.set(DATA_UBX_MON_SPAN_NUM_RF_BLOCKS, static_cast<uint32_t>(p.num_rf_blocks));

    for (uint8_t i = 0u; i < p.num_rf_blocks && i < MAX_MON_SPAN_RF_BLOCKS; ++i)
    {
        store.set(db_index_with_offset(DATA_UBX_MON_SPAN_PGA, i), p.rf_blocks[i].pga);
    }

    mask.set(MSG_UBX_MON_SPAN);
}

} // namespace database
} // namespace ubx
