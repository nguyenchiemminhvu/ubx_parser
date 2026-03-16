// src/database/handlers/db_esf_meas_handler.cpp
//
// Maps ubx_esf_meas fields to database data_field entries.
// Header fields and per-measurement arrays (data type + value) are all stored
// in the flat field store, mirroring the NAV-SAT array pattern.

#include "database/handlers/db_esf_meas_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

#include <algorithm>

namespace ubx
{
namespace database
{

void db_esf_meas_handler::update(const parser::ubx_esf_meas& msg)
{
    msg_ = msg;
}

void db_esf_meas_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    store.set(DATA_UBX_ESF_MEAS_TIME_TAG, msg_.time_tag);
    store.set(DATA_UBX_ESF_MEAS_FLAGS,    msg_.flags);
    store.set(DATA_UBX_ESF_MEAS_NUM_MEAS, msg_.num_meas);

    const uint8_t count = static_cast<uint8_t>(
        std::min(static_cast<unsigned>(msg_.num_meas),
                 static_cast<unsigned>(MAX_ESF_MEAS_DATA)));

    for (uint8_t i = 0u; i < count; ++i)
    {
        double scale_factor = 1.0;
        switch (msg_.data[i].data_type)
        {
            case ubx::parser::esf_sensor_type::ESF_SENSOR_X_ACCEL:
            case ubx::parser::esf_sensor_type::ESF_SENSOR_Y_ACCEL:
            case ubx::parser::esf_sensor_type::ESF_SENSOR_Z_ACCEL:
                scale_factor = 2e-10;
                break;
            case ubx::parser::esf_sensor_type::ESF_SENSOR_X_GYRO_ANG_RATE:
            case ubx::parser::esf_sensor_type::ESF_SENSOR_Y_GYRO_ANG_RATE:
            case ubx::parser::esf_sensor_type::ESF_SENSOR_Z_GYRO_ANG_RATE:
                scale_factor = 2e-12;
                break;
            case ubx::parser::esf_sensor_type::ESF_SENSOR_SPEED:
                scale_factor = 1e-3;
                break;
            case ubx::parser::esf_sensor_type::ESF_SENSOR_GYRO_TEMP:
                scale_factor = 1e-2;
                break;
            default:
                break;
        }

        store.set(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_TYPE,  i), msg_.data[i].data_type);
        store.set(db_index_with_offset(DATA_UBX_ESF_MEAS_DATA_VALUE, i), static_cast<double>(msg_.data[i].data_value) * scale_factor);
    }

    mask.set(MSG_UBX_ESF_MEAS);
}

} // namespace database
} // namespace ubx
