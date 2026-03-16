// src/database/handlers/db_tim_tp_handler.cpp

#include "database/handlers/db_tim_tp_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_tim_tp_handler::update(const parser::ubx_tim_tp& msg)
{
    msg_ = msg;
}

void db_tim_tp_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    // timeBase: bits 1:0 of flags  (0=GNSS,1=UTC,2=GLONASS,3=BeiDou)
    // utcAvail: bit 2 of flags
    // timeRefGnss: bits 7:4 of ref_info
    // utcStandard: bits 3:0 of ref_info
    uint8_t time_base    = msg_.flags & 0x03u;
    bool    utc_avail    = (msg_.flags & 0x04u) != 0u;
    bool    q_err_valid  = (msg_.flags & 0x08u) != 0u;  // raimActive reused for q_err_valid
    uint8_t time_ref     = (msg_.ref_info >> 4u) & 0x0Fu;
    uint8_t utc_standard = msg_.ref_info & 0x0Fu;

    store.set(DATA_TIME_PULSE_TIMESTAMP,          static_cast<int64_t>(msg_.tow_ms));
    store.set(DATA_TIME_PULSE_QUANTIZATION_ERROR, msg_.q_err);
    store.set(DATA_TIME_PULSE_UTC_TIME_BASE,      time_base);
    store.set(DATA_TIME_PULSE_UTC_AVAILABLE,      utc_avail);
    store.set(DATA_TIME_PULSE_RAIM_ACTIVE,        false);   // not in TIM-TP; legacy compat
    store.set(DATA_TIME_PULSE_Q_ERROR_VALID,      q_err_valid);
    store.set(DATA_TIME_PULSE_TP_LOCKED_GNSS,     false);   // not in TIM-TP; legacy compat
    store.set(DATA_TIME_PULSE_TIME_REF,           time_ref);
    store.set(DATA_TIME_PULSE_UTC_STANDARD,       utc_standard);

    mask.set(MSG_UBX_TIM_TP);
}

} // namespace database
} // namespace ubx
