// src/database/handlers/db_mon_io_handler.cpp
//
// Maps ubx_mon_io fields to database data_field entries.
// Per-port arrays are stored using contiguous index ranges.

#include "database/handlers/db_mon_io_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_mon_io_handler::update(const parser::ubx_mon_io& msg)
{
    msg_ = msg;
}

void db_mon_io_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_mon_io& p = msg_;

    store.set(DATA_UBX_MON_IO_NUM_PORTS, static_cast<uint32_t>(p.num_ports));

    for (uint8_t i = 0u; i < p.num_ports; ++i)
    {
        const parser::mon_io_port& port = p.ports[i];

        store.set(db_index_with_offset(DATA_UBX_MON_IO_RX_BYTES,      i), port.rx_bytes);
        store.set(db_index_with_offset(DATA_UBX_MON_IO_TX_BYTES,      i), port.tx_bytes);
        store.set(db_index_with_offset(DATA_UBX_MON_IO_PARITY_ERRS,   i), port.parity_errs);
        store.set(db_index_with_offset(DATA_UBX_MON_IO_FRAMING_ERRS,  i), port.framing_errs);
        store.set(db_index_with_offset(DATA_UBX_MON_IO_OVERRUN_ERRS,  i), port.overrun_errs);
        store.set(db_index_with_offset(DATA_UBX_MON_IO_BREAK_COND,    i), port.break_cond);
        store.set(db_index_with_offset(DATA_UBX_MON_IO_RX_BUSY,       i), port.rx_busy);
        store.set(db_index_with_offset(DATA_UBX_MON_IO_TX_BUSY,       i), port.tx_busy);
    }

    mask.set(MSG_UBX_MON_IO);
}

} // namespace database
} // namespace ubx
