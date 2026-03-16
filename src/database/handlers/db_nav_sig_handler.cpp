// src/database/handlers/db_nav_sig_handler.cpp
//
// Maps ubx_nav_sig signal records into the flat per-signal field arrays.
// Up to MAX_ALL_GNSS_SIGNALS records are written; extras are silently dropped.

#include "database/handlers/db_nav_sig_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

#include <algorithm>
#include <cstdint>

namespace ubx
{
namespace database
{

// NAV-SIG sigFlags bitmask offsets (u-blox Interface Description).
static constexpr uint16_t SIG_FLAG_HEALTH_MASK = (0x3u << 0);  // bits 1:0; 1=healthy
static constexpr uint16_t SIG_FLAG_PR_USED     = (1u   << 3);  // bit 3
static constexpr uint16_t SIG_FLAG_CR_USED     = (1u   << 4);  // bit 4
static constexpr uint16_t SIG_FLAG_DO_USED     = (1u   << 5);  // bit 5

void db_nav_sig_handler::update(const parser::ubx_nav_sig& msg)
{
    msg_ = msg;
}

void db_nav_sig_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const uint8_t count = static_cast<uint8_t>(
        std::min(static_cast<size_t>(msg_.num_sigs),
                 static_cast<size_t>(MAX_ALL_GNSS_SIGNALS)));

    store.set(DATA_UBX_NAV_SIG_NUM_SIGS, static_cast<uint32_t>(count));

    for (uint8_t i = 0u; i < count; ++i)
    {
        const parser::nav_sig_info& sig = msg_.signals[i];

        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_GNSSID,      i), sig.gnss_id);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_SVID,        i), sig.sv_id);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_SIGID,       i), sig.sig_id);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_FREQID,      i), sig.freq_id);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_PR_RES,      i), sig.pr_res);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_CNO,         i), sig.cno);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_QUAL_IND,    i), sig.qual_ind);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_CORR_SOURCE, i), sig.corr_source);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_IONO_MODEL,  i), sig.iono_model);

        bool health_ok = (sig.sig_flags & SIG_FLAG_HEALTH_MASK) == 1u;
        bool pr_used   = (sig.sig_flags & SIG_FLAG_PR_USED) != 0u;
        bool cr_used   = (sig.sig_flags & SIG_FLAG_CR_USED) != 0u;
        bool do_used   = (sig.sig_flags & SIG_FLAG_DO_USED) != 0u;

        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_HEALTH,      i), health_ok);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_PR_USED,     i), pr_used);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_CR_USED,     i), cr_used);
        store.set(db_index_with_offset(DATA_UBX_NAV_SIG_DO_USED,     i), do_used);
    }

    mask.set(MSG_UBX_NAV_SIG);
}

} // namespace database
} // namespace ubx
