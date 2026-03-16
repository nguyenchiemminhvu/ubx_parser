// src/database/handlers/db_mon_txbuf_handler.cpp
//
// Maps ubx_mon_txbuf fields to database data_field entries.

#include "database/handlers/db_mon_txbuf_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_mon_txbuf_handler::update(const parser::ubx_mon_txbuf& msg)
{
    msg_ = msg;
}

void db_mon_txbuf_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_mon_txbuf& p = msg_;

    store.set(DATA_UBX_MON_TXBUF_ERRORS, p.errors);
    store.set(DATA_UBX_MON_TXBUF_TUSED,  p.t_used);
    store.set(DATA_UBX_MON_TXBUF_TPEAK,  p.t_peak);

    // ── Per-port arrays ──────────────────────────────────────────────────────
    store.set(DATA_UBX_MON_TXBUF_AVAIL_0,      p.pending[0]);
    store.set(DATA_UBX_MON_TXBUF_AVAIL_1,      p.pending[1]);
    store.set(DATA_UBX_MON_TXBUF_AVAIL_2,      p.pending[2]);
    store.set(DATA_UBX_MON_TXBUF_AVAIL_3,      p.pending[3]);
    store.set(DATA_UBX_MON_TXBUF_AVAIL_4,      p.pending[4]);
    store.set(DATA_UBX_MON_TXBUF_AVAIL_5,      p.pending[5]);

    store.set(DATA_UBX_MON_TXBUF_USAGE_0,      p.usage[0]);
    store.set(DATA_UBX_MON_TXBUF_USAGE_1,      p.usage[1]);
    store.set(DATA_UBX_MON_TXBUF_USAGE_2,      p.usage[2]);
    store.set(DATA_UBX_MON_TXBUF_USAGE_3,      p.usage[3]);
    store.set(DATA_UBX_MON_TXBUF_USAGE_4,      p.usage[4]);
    store.set(DATA_UBX_MON_TXBUF_USAGE_5,      p.usage[5]);

    store.set(DATA_UBX_MON_TXBUF_PEAK_USAGE_0, p.peak_usage[0]);
    store.set(DATA_UBX_MON_TXBUF_PEAK_USAGE_1, p.peak_usage[1]);
    store.set(DATA_UBX_MON_TXBUF_PEAK_USAGE_2, p.peak_usage[2]);
    store.set(DATA_UBX_MON_TXBUF_PEAK_USAGE_3, p.peak_usage[3]);
    store.set(DATA_UBX_MON_TXBUF_PEAK_USAGE_4, p.peak_usage[4]);
    store.set(DATA_UBX_MON_TXBUF_PEAK_USAGE_5, p.peak_usage[5]);

    // Store the pending field as "limit" proxy for compatibility with
    // consumers checking DATA_UBX_MON_TXBUF_LIMIT.
    store.set(DATA_UBX_MON_TXBUF_LIMIT,  static_cast<uint32_t>(p.pending[0]));

    mask.set(MSG_UBX_MON_TXBUF);
}

} // namespace database
} // namespace ubx
