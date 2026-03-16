// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// messages/ubx_esf_sensor_types.h
//
// Sensor data type enumeration shared by UBX-ESF-MEAS and UBX-ESF-STATUS.
//
// Reference: ZED-F9R Integration Manual (UBX-20039643), Section 3.2.7.5
//            "Sensor data types".
//
// These values appear in:
//   UBX-ESF-MEAS  – dataType field (bits [29:24] of each data word)
//   UBX-ESF-STATUS – sensStatus1 bits [4:0] of each per-sensor block
//
// Values not listed in the table are reserved.

#pragma once

#include <cstdint>

namespace ubx
{
namespace parser
{

/// Sensor data type values common to UBX-ESF-MEAS and UBX-ESF-STATUS.
/// ZED-F9R Integration Manual, Section 3.2.7.5.
enum esf_sensor_type : uint8_t
{
    ESF_SENSOR_NONE                   = 0u,  ///< No data / not configured
    ESF_SENSOR_Z_GYRO_ANG_RATE        = 5u,  ///< z-axis gyroscope angular rate [deg/s × 2^-12]
    ESF_SENSOR_FRONT_LEFT_WHEEL_TICKS = 6u,  ///< Front-left wheel ticks [ticks]
    ESF_SENSOR_FRONT_RIGHT_WHEEL_TICKS= 7u,  ///< Front-right wheel ticks [ticks]
    ESF_SENSOR_REAR_LEFT_WHEEL_TICKS  = 8u,  ///< Rear-left wheel ticks [ticks]
    ESF_SENSOR_REAR_RIGHT_WHEEL_TICKS = 9u,  ///< Rear-right wheel ticks [ticks]
    ESF_SENSOR_SPEED                  = 10u, ///< Single-tick speed (all wheels combined) [m/s × 1e-3]
    ESF_SENSOR_GYRO_TEMP              = 11u, ///< Gyroscope temperature [°C × 1e-2]
    ESF_SENSOR_X_GYRO_ANG_RATE        = 12u, ///< x-axis gyroscope angular rate [deg/s × 2^-12]
    ESF_SENSOR_Y_GYRO_ANG_RATE        = 13u, ///< y-axis gyroscope angular rate [deg/s × 2^-12]
    ESF_SENSOR_X_ACCEL                = 14u, ///< x-axis accelerometer specific force [m/s^2 × 2^-10]
    ESF_SENSOR_Y_ACCEL                = 16u, ///< y-axis accelerometer specific force [m/s^2 × 2^-10]
    ESF_SENSOR_Z_ACCEL                = 17u, ///< z-axis accelerometer specific force [m/s^2 × 2^-10]
};

} // namespace parser
} // namespace ubx
