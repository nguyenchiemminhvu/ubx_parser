// src/database/handlers/db_esf_status_handler.cpp
//
// Maps ubx_esf_status fields to database data_field entries.
// Header fields (fusion_mode, num_sens) and per-sensor arrays are all stored
// in the flat field store, mirroring the NAV-SAT array pattern.

#include "database/handlers/db_esf_status_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

#include <algorithm>

namespace ubx
{
namespace database
{

void db_esf_status_handler::update(const parser::ubx_esf_status& msg)
{
    msg_ = msg;
}

void db_esf_status_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    store.set(DATA_UBX_ESF_STATUS_ITOW,        msg_.i_tow);
    store.set(DATA_UBX_ESF_STATUS_FUSION_MODE,  msg_.fusion_mode);
    store.set(DATA_UBX_ESF_STATUS_NUM_SENS,     msg_.num_sens);

    const uint8_t count = static_cast<uint8_t>(
        std::min(static_cast<unsigned>(msg_.num_sens),
                 static_cast<unsigned>(MAX_ESF_SENSORS)));

    for (uint8_t i = 0u; i < count; ++i)
    {
        const parser::ubx_esf_status_sensor& s = msg_.sensors[i];

        const uint8_t sensor_type   = s.sens_status1 & parser::ubx_esf_status_sensor::TYPE_MASK;
        const bool    sensor_used   = (s.sens_status1 & parser::ubx_esf_status_sensor::USED)  != 0u;
        const bool    sensor_ready  = (s.sens_status1 & parser::ubx_esf_status_sensor::READY) != 0u;
        const uint8_t calib_status  = s.sens_status2 & parser::ubx_esf_status_sensor::CALIB_STATUS_MASK;
        const uint8_t time_status   = (s.sens_status2 & parser::ubx_esf_status_sensor::TIME_STATUS_MASK) >> 2u;

        store.set(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_TYPE,        i), sensor_type);
        store.set(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_USED,        i), sensor_used);
        store.set(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_READY,       i), sensor_ready);
        store.set(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_CALIB,       i), calib_status);
        store.set(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_TIME_STATUS, i), time_status);
        store.set(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_FREQ,        i), s.freq);
        store.set(db_index_with_offset(DATA_UBX_ESF_STATUS_SENS_FAULTS,      i), s.faults);
    }

    mask.set(MSG_UBX_ESF_STATUS);
}

} // namespace database
} // namespace ubx
