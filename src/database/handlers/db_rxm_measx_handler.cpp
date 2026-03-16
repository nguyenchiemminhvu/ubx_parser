// src/database/handlers/db_rxm_measx_handler.cpp
//
// Maps ubx_rxm_measx fields to database data_field entries.
// Per-SV arrays are stored using contiguous index ranges.

#include "database/handlers/db_rxm_measx_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_rxm_measx_handler::update(const parser::ubx_rxm_measx& msg)
{
    msg_ = msg;
}

void db_rxm_measx_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_rxm_measx& p = msg_;

    // ── Header fields ────────────────────────────────────────────────────────
    store.set(DATA_UBX_RXM_MEASX_GPS_TOW,      p.gps_tow);
    store.set(DATA_UBX_RXM_MEASX_GLO_TOW,      p.glo_tow);
    store.set(DATA_UBX_RXM_MEASX_BDS_TOW,      p.bds_tow);
    store.set(DATA_UBX_RXM_MEASX_QZS_TOW,      p.qzss_tow);
    store.set(DATA_UBX_RXM_MEASX_GPS_TOW_ACC,  p.gps_tow_acc);
    store.set(DATA_UBX_RXM_MEASX_GLO_TOW_ACC,  p.glo_tow_acc);
    store.set(DATA_UBX_RXM_MEASX_BDS_TOW_ACC,  p.bds_tow_acc);
    store.set(DATA_UBX_RXM_MEASX_QZS_TOW_ACC,  p.qzss_tow_acc);
    store.set(DATA_UBX_RXM_MEASX_NUM_SV,        static_cast<uint32_t>(p.num_svs));
    store.set(DATA_UBX_RXM_MEASX_FLAGS,         p.flags);

    // ── Per-SV arrays ────────────────────────────────────────────────────────
    for (uint8_t i = 0u; i < p.num_svs; ++i)
    {
        const parser::rxm_measx_sv& sv = p.svs[i];

        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_GNSS_ID,           i), sv.gnss_id);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_SV_ID,             i), sv.sv_id);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_CN0,               i), sv.cno);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_MPH,               i), sv.mpath_indic);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_DOPPLER,           i),
                  static_cast<float>(sv.doppler_ms) * 0.04f);   // m/s
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_WHOLE_CHIPS,       i), sv.whole_chips);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_FRAC_CHIPS,        i), sv.frac_chips);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_CODE_PHASE,        i), sv.code_phase);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_INT_CODE_PHASE,    i), sv.int_code_phase);
        store.set(db_index_with_offset(DATA_UBX_RXM_MEASX_PSEU_RANGE_RMS_ERR, i), sv.pseu_range_rms_err);
    }

    mask.set(MSG_UBX_RXM_MEASX);
}

} // namespace database
} // namespace ubx
