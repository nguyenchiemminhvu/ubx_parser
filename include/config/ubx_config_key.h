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
// UBX configuration key IDs — imported from official u-blox ubxlib source.
// Source: https://github.com/u-blox/ubxlib/blob/master/gnss/api/u_gnss_cfg_val_key.h
// Section: auto-generated key IDs (DO NOT MODIFY — regenerated from official source)
//
// Names are remapped to snake_case without the U_GNSS_CFG_VAL_KEY_ID_ prefix
// and without the trailing type suffix, to match the project naming convention.
// The uint32_t value for each constant is exactly as found in the source file.
#pragma once

#include <cstdint>

namespace ubx
{
namespace config
{
namespace cfg_key
{

// ANA
constexpr uint32_t ana_use_ana                          = 0x10230001u;
constexpr uint32_t ana_orbmaxerr                        = 0x30230002u;

// BATCH
constexpr uint32_t batch_enable                         = 0x10260013u;
constexpr uint32_t batch_pioenable                      = 0x10260014u;
constexpr uint32_t batch_maxentries                     = 0x30260015u;
constexpr uint32_t batch_warnthrs                       = 0x30260016u;
constexpr uint32_t batch_pioactivelow                   = 0x10260018u;
constexpr uint32_t batch_pioid                          = 0x20260019u;
constexpr uint32_t batch_extrapvt                       = 0x1026001au;
constexpr uint32_t batch_extraodo                       = 0x1026001bu;

// BDS
constexpr uint32_t bds_use_geo_prn                      = 0x10340014u;

// GEOFENCE
constexpr uint32_t geofence_conflvl                     = 0x20240011u;
constexpr uint32_t geofence_use_pio                     = 0x10240012u;
constexpr uint32_t geofence_pinpol                      = 0x20240013u;
constexpr uint32_t geofence_pin                         = 0x20240014u;
constexpr uint32_t geofence_use_fence1                  = 0x10240020u;
constexpr uint32_t geofence_fence1_lat                  = 0x40240021u;
constexpr uint32_t geofence_fence1_lon                  = 0x40240022u;
constexpr uint32_t geofence_fence1_rad                  = 0x40240023u;
constexpr uint32_t geofence_use_fence2                  = 0x10240030u;
constexpr uint32_t geofence_fence2_lat                  = 0x40240031u;
constexpr uint32_t geofence_fence2_lon                  = 0x40240032u;
constexpr uint32_t geofence_fence2_rad                  = 0x40240033u;
constexpr uint32_t geofence_use_fence3                  = 0x10240040u;
constexpr uint32_t geofence_fence3_lat                  = 0x40240041u;
constexpr uint32_t geofence_fence3_lon                  = 0x40240042u;
constexpr uint32_t geofence_fence3_rad                  = 0x40240043u;
constexpr uint32_t geofence_use_fence4                  = 0x10240050u;
constexpr uint32_t geofence_fence4_lat                  = 0x40240051u;
constexpr uint32_t geofence_fence4_lon                  = 0x40240052u;
constexpr uint32_t geofence_fence4_rad                  = 0x40240053u;

// HW
constexpr uint32_t hw_ant_cfg_voltctrl                  = 0x10a3002eu;
constexpr uint32_t hw_ant_cfg_shortdet                  = 0x10a3002fu;
constexpr uint32_t hw_ant_cfg_shortdet_pol              = 0x10a30030u;
constexpr uint32_t hw_ant_cfg_opendet                   = 0x10a30031u;
constexpr uint32_t hw_ant_cfg_opendet_pol               = 0x10a30032u;
constexpr uint32_t hw_ant_cfg_pwrdown                   = 0x10a30033u;
constexpr uint32_t hw_ant_cfg_pwrdown_pol               = 0x10a30034u;
constexpr uint32_t hw_ant_cfg_recover                   = 0x10a30035u;
constexpr uint32_t hw_ant_sup_switch_pin                = 0x20a30036u;
constexpr uint32_t hw_ant_sup_short_pin                 = 0x20a30037u;
constexpr uint32_t hw_ant_sup_open_pin                  = 0x20a30038u;
constexpr uint32_t hw_ant_sup_engine                    = 0x20a30054u;
constexpr uint32_t hw_ant_sup_short_thr                 = 0x20a30055u;
constexpr uint32_t hw_ant_sup_open_thr                  = 0x20a30056u;
constexpr uint32_t hw_rf_lna_mode                       = 0x20a30057u;

// I2C
constexpr uint32_t i2c_address                          = 0x20510001u;
constexpr uint32_t i2c_extendedtimeout                  = 0x10510002u;
constexpr uint32_t i2c_enabled                          = 0x10510003u;

// I2CINPROT
constexpr uint32_t i2cinprot_ubx                        = 0x10710001u;
constexpr uint32_t i2cinprot_nmea                       = 0x10710002u;
constexpr uint32_t i2cinprot_rtcm3x                     = 0x10710004u;
constexpr uint32_t i2cinprot_spartn                     = 0x10710005u;

// I2COUTPROT
constexpr uint32_t i2coutprot_ubx                       = 0x10720001u;
constexpr uint32_t i2coutprot_nmea                      = 0x10720002u;
constexpr uint32_t i2coutprot_rtcm3x                    = 0x10720004u;

// INFMSG
constexpr uint32_t infmsg_ubx_i2c                       = 0x20920001u;
constexpr uint32_t infmsg_ubx_uart1                     = 0x20920002u;
constexpr uint32_t infmsg_ubx_uart2                     = 0x20920003u;
constexpr uint32_t infmsg_ubx_usb                       = 0x20920004u;
constexpr uint32_t infmsg_ubx_spi                       = 0x20920005u;
constexpr uint32_t infmsg_nmea_i2c                      = 0x20920006u;
constexpr uint32_t infmsg_nmea_uart1                    = 0x20920007u;
constexpr uint32_t infmsg_nmea_uart2                    = 0x20920008u;
constexpr uint32_t infmsg_nmea_usb                      = 0x20920009u;
constexpr uint32_t infmsg_nmea_spi                      = 0x2092000au;

// ITFM
constexpr uint32_t itfm_bbthreshold                     = 0x20410001u;
constexpr uint32_t itfm_cwthreshold                     = 0x20410002u;
constexpr uint32_t itfm_enable                          = 0x1041000du;
constexpr uint32_t itfm_antsetting                      = 0x20410010u;
constexpr uint32_t itfm_enable_aux                      = 0x10410013u;

// LOGFILTER
constexpr uint32_t logfilter_record_ena                 = 0x10de0002u;
constexpr uint32_t logfilter_once_per_wake_up_ena       = 0x10de0003u;
constexpr uint32_t logfilter_apply_all_filters          = 0x10de0004u;
constexpr uint32_t logfilter_min_interval               = 0x30de0005u;
constexpr uint32_t logfilter_time_thrs                  = 0x30de0006u;
constexpr uint32_t logfilter_speed_thrs                 = 0x30de0007u;
constexpr uint32_t logfilter_position_thrs              = 0x40de0008u;

// MOT
constexpr uint32_t mot_gnssspeed_thrs                   = 0x20250038u;
constexpr uint32_t mot_gnssdist_thrs                    = 0x3025003bu;

// MSGOUT — NMEA
constexpr uint32_t msgout_nmea_id_dtm_i2c               = 0x209100a6u;
constexpr uint32_t msgout_nmea_id_dtm_spi               = 0x209100aau;
constexpr uint32_t msgout_nmea_id_dtm_uart1             = 0x209100a7u;
constexpr uint32_t msgout_nmea_id_dtm_uart2             = 0x209100a8u;
constexpr uint32_t msgout_nmea_id_dtm_usb               = 0x209100a9u;
constexpr uint32_t msgout_nmea_id_gbs_i2c               = 0x209100ddu;
constexpr uint32_t msgout_nmea_id_gbs_spi               = 0x209100e1u;
constexpr uint32_t msgout_nmea_id_gbs_uart1             = 0x209100deu;
constexpr uint32_t msgout_nmea_id_gbs_uart2             = 0x209100dfu;
constexpr uint32_t msgout_nmea_id_gbs_usb               = 0x209100e0u;
constexpr uint32_t msgout_nmea_id_gga_i2c               = 0x209100bau;
constexpr uint32_t msgout_nmea_id_gga_spi               = 0x209100beu;
constexpr uint32_t msgout_nmea_id_gga_uart1             = 0x209100bbu;
constexpr uint32_t msgout_nmea_id_gga_uart2             = 0x209100bcu;
constexpr uint32_t msgout_nmea_id_gga_usb               = 0x209100bdu;
constexpr uint32_t msgout_nmea_id_gll_i2c               = 0x209100c9u;
constexpr uint32_t msgout_nmea_id_gll_spi               = 0x209100cdu;
constexpr uint32_t msgout_nmea_id_gll_uart1             = 0x209100cau;
constexpr uint32_t msgout_nmea_id_gll_uart2             = 0x209100cbu;
constexpr uint32_t msgout_nmea_id_gll_usb               = 0x209100ccu;
constexpr uint32_t msgout_nmea_id_gns_i2c               = 0x209100b5u;
constexpr uint32_t msgout_nmea_id_gns_spi               = 0x209100b9u;
constexpr uint32_t msgout_nmea_id_gns_uart1             = 0x209100b6u;
constexpr uint32_t msgout_nmea_id_gns_uart2             = 0x209100b7u;
constexpr uint32_t msgout_nmea_id_gns_usb               = 0x209100b8u;
constexpr uint32_t msgout_nmea_id_grs_i2c               = 0x209100ceu;
constexpr uint32_t msgout_nmea_id_grs_spi               = 0x209100d2u;
constexpr uint32_t msgout_nmea_id_grs_uart1             = 0x209100cfu;
constexpr uint32_t msgout_nmea_id_grs_uart2             = 0x209100d0u;
constexpr uint32_t msgout_nmea_id_grs_usb               = 0x209100d1u;
constexpr uint32_t msgout_nmea_id_gsa_i2c               = 0x209100bfu;
constexpr uint32_t msgout_nmea_id_gsa_spi               = 0x209100c3u;
constexpr uint32_t msgout_nmea_id_gsa_uart1             = 0x209100c0u;
constexpr uint32_t msgout_nmea_id_gsa_uart2             = 0x209100c1u;
constexpr uint32_t msgout_nmea_id_gsa_usb               = 0x209100c2u;
constexpr uint32_t msgout_nmea_id_gst_i2c               = 0x209100d3u;
constexpr uint32_t msgout_nmea_id_gst_spi               = 0x209100d7u;
constexpr uint32_t msgout_nmea_id_gst_uart1             = 0x209100d4u;
constexpr uint32_t msgout_nmea_id_gst_uart2             = 0x209100d5u;
constexpr uint32_t msgout_nmea_id_gst_usb               = 0x209100d6u;
constexpr uint32_t msgout_nmea_id_gsv_i2c               = 0x209100c4u;
constexpr uint32_t msgout_nmea_id_gsv_spi               = 0x209100c8u;
constexpr uint32_t msgout_nmea_id_gsv_uart1             = 0x209100c5u;
constexpr uint32_t msgout_nmea_id_gsv_uart2             = 0x209100c6u;
constexpr uint32_t msgout_nmea_id_gsv_usb               = 0x209100c7u;
constexpr uint32_t msgout_nmea_id_rlm_i2c               = 0x20910400u;
constexpr uint32_t msgout_nmea_id_rlm_spi               = 0x20910404u;
constexpr uint32_t msgout_nmea_id_rlm_uart1             = 0x20910401u;
constexpr uint32_t msgout_nmea_id_rlm_uart2             = 0x20910402u;
constexpr uint32_t msgout_nmea_id_rlm_usb               = 0x20910403u;
constexpr uint32_t msgout_nmea_id_rmc_i2c               = 0x209100abu;
constexpr uint32_t msgout_nmea_id_rmc_spi               = 0x209100afu;
constexpr uint32_t msgout_nmea_id_rmc_uart1             = 0x209100acu;
constexpr uint32_t msgout_nmea_id_rmc_uart2             = 0x209100adu;
constexpr uint32_t msgout_nmea_id_rmc_usb               = 0x209100aeu;
constexpr uint32_t msgout_nmea_id_vlw_i2c               = 0x209100e7u;
constexpr uint32_t msgout_nmea_id_vlw_spi               = 0x209100ebu;
constexpr uint32_t msgout_nmea_id_vlw_uart1             = 0x209100e8u;
constexpr uint32_t msgout_nmea_id_vlw_uart2             = 0x209100e9u;
constexpr uint32_t msgout_nmea_id_vlw_usb               = 0x209100eau;
constexpr uint32_t msgout_nmea_id_vtg_i2c               = 0x209100b0u;
constexpr uint32_t msgout_nmea_id_vtg_spi               = 0x209100b4u;
constexpr uint32_t msgout_nmea_id_vtg_uart1             = 0x209100b1u;
constexpr uint32_t msgout_nmea_id_vtg_uart2             = 0x209100b2u;
constexpr uint32_t msgout_nmea_id_vtg_usb               = 0x209100b3u;
constexpr uint32_t msgout_nmea_id_zda_i2c               = 0x209100d8u;
constexpr uint32_t msgout_nmea_id_zda_spi               = 0x209100dcu;
constexpr uint32_t msgout_nmea_id_zda_uart1             = 0x209100d9u;
constexpr uint32_t msgout_nmea_id_zda_uart2             = 0x209100dau;
constexpr uint32_t msgout_nmea_id_zda_usb               = 0x209100dbu;

// MSGOUT — NMEA NAV2
constexpr uint32_t msgout_nmea_nav2_id_gga_i2c          = 0x20910661u;
constexpr uint32_t msgout_nmea_nav2_id_gga_spi          = 0x20910665u;
constexpr uint32_t msgout_nmea_nav2_id_gga_uart1        = 0x20910662u;
constexpr uint32_t msgout_nmea_nav2_id_gga_uart2        = 0x20910663u;
constexpr uint32_t msgout_nmea_nav2_id_gga_usb          = 0x20910664u;
constexpr uint32_t msgout_nmea_nav2_id_gll_i2c          = 0x20910670u;
constexpr uint32_t msgout_nmea_nav2_id_gll_spi          = 0x20910674u;
constexpr uint32_t msgout_nmea_nav2_id_gll_uart1        = 0x20910671u;
constexpr uint32_t msgout_nmea_nav2_id_gll_uart2        = 0x20910672u;
constexpr uint32_t msgout_nmea_nav2_id_gll_usb          = 0x20910673u;
constexpr uint32_t msgout_nmea_nav2_id_gns_i2c          = 0x2091065cu;
constexpr uint32_t msgout_nmea_nav2_id_gns_spi          = 0x20910660u;
constexpr uint32_t msgout_nmea_nav2_id_gns_uart1        = 0x2091065du;
constexpr uint32_t msgout_nmea_nav2_id_gns_uart2        = 0x2091065eu;
constexpr uint32_t msgout_nmea_nav2_id_gns_usb          = 0x2091065fu;
constexpr uint32_t msgout_nmea_nav2_id_gsa_i2c          = 0x20910666u;
constexpr uint32_t msgout_nmea_nav2_id_gsa_spi          = 0x2091066au;
constexpr uint32_t msgout_nmea_nav2_id_gsa_uart1        = 0x20910667u;
constexpr uint32_t msgout_nmea_nav2_id_gsa_uart2        = 0x20910668u;
constexpr uint32_t msgout_nmea_nav2_id_gsa_usb          = 0x20910669u;
constexpr uint32_t msgout_nmea_nav2_id_rmc_i2c          = 0x20910652u;
constexpr uint32_t msgout_nmea_nav2_id_rmc_spi          = 0x20910656u;
constexpr uint32_t msgout_nmea_nav2_id_rmc_uart1        = 0x20910653u;
constexpr uint32_t msgout_nmea_nav2_id_rmc_uart2        = 0x20910654u;
constexpr uint32_t msgout_nmea_nav2_id_rmc_usb          = 0x20910655u;
constexpr uint32_t msgout_nmea_nav2_id_vtg_i2c          = 0x20910657u;
constexpr uint32_t msgout_nmea_nav2_id_vtg_spi          = 0x2091065bu;
constexpr uint32_t msgout_nmea_nav2_id_vtg_uart1        = 0x20910658u;
constexpr uint32_t msgout_nmea_nav2_id_vtg_uart2        = 0x20910649u;
constexpr uint32_t msgout_nmea_nav2_id_vtg_usb          = 0x2091065au;
constexpr uint32_t msgout_nmea_nav2_id_zda_i2c          = 0x2091067fu;
constexpr uint32_t msgout_nmea_nav2_id_zda_spi          = 0x20910683u;
constexpr uint32_t msgout_nmea_nav2_id_zda_uart1        = 0x20910680u;
constexpr uint32_t msgout_nmea_nav2_id_zda_uart2        = 0x20910681u;
constexpr uint32_t msgout_nmea_nav2_id_zda_usb          = 0x20910682u;

// MSGOUT — PUBX
constexpr uint32_t msgout_pubx_id_polyp_i2c             = 0x209100ecu;
constexpr uint32_t msgout_pubx_id_polyp_spi             = 0x209100f0u;
constexpr uint32_t msgout_pubx_id_polyp_uart1           = 0x209100edu;
constexpr uint32_t msgout_pubx_id_polyp_uart2           = 0x209100eeu;
constexpr uint32_t msgout_pubx_id_polyp_usb             = 0x209100efu;
constexpr uint32_t msgout_pubx_id_polys_i2c             = 0x209100f1u;
constexpr uint32_t msgout_pubx_id_polys_spi             = 0x209100f5u;
constexpr uint32_t msgout_pubx_id_polys_uart1           = 0x209100f2u;
constexpr uint32_t msgout_pubx_id_polys_uart2           = 0x209100f3u;
constexpr uint32_t msgout_pubx_id_polys_usb             = 0x209100f4u;
constexpr uint32_t msgout_pubx_id_polyt_i2c             = 0x209100f6u;
constexpr uint32_t msgout_pubx_id_polyt_spi             = 0x209100fau;
constexpr uint32_t msgout_pubx_id_polyt_uart1           = 0x209100f7u;
constexpr uint32_t msgout_pubx_id_polyt_uart2           = 0x209100f8u;
constexpr uint32_t msgout_pubx_id_polyt_usb             = 0x209100f9u;

// MSGOUT — RTCM
constexpr uint32_t msgout_rtcm_3x_type1005_i2c          = 0x209102bdu;
constexpr uint32_t msgout_rtcm_3x_type1005_spi          = 0x209102c1u;
constexpr uint32_t msgout_rtcm_3x_type1005_uart1        = 0x209102beu;
constexpr uint32_t msgout_rtcm_3x_type1005_uart2        = 0x209102bfu;
constexpr uint32_t msgout_rtcm_3x_type1005_usb          = 0x209102c0u;
constexpr uint32_t msgout_rtcm_3x_type1074_i2c          = 0x2091035eu;
constexpr uint32_t msgout_rtcm_3x_type1074_spi          = 0x20910362u;
constexpr uint32_t msgout_rtcm_3x_type1074_uart1        = 0x2091035fu;
constexpr uint32_t msgout_rtcm_3x_type1074_uart2        = 0x20910360u;
constexpr uint32_t msgout_rtcm_3x_type1074_usb          = 0x20910361u;
constexpr uint32_t msgout_rtcm_3x_type1077_i2c          = 0x209102ccu;
constexpr uint32_t msgout_rtcm_3x_type1077_spi          = 0x209102d0u;
constexpr uint32_t msgout_rtcm_3x_type1077_uart1        = 0x209102cdu;
constexpr uint32_t msgout_rtcm_3x_type1077_uart2        = 0x209102ceu;
constexpr uint32_t msgout_rtcm_3x_type1077_usb          = 0x209102cfu;
constexpr uint32_t msgout_rtcm_3x_type1084_i2c          = 0x20910363u;
constexpr uint32_t msgout_rtcm_3x_type1084_spi          = 0x20910367u;
constexpr uint32_t msgout_rtcm_3x_type1084_uart1        = 0x20910364u;
constexpr uint32_t msgout_rtcm_3x_type1084_uart2        = 0x20910365u;
constexpr uint32_t msgout_rtcm_3x_type1084_usb          = 0x20910366u;
constexpr uint32_t msgout_rtcm_3x_type1087_i2c          = 0x209102d1u;
constexpr uint32_t msgout_rtcm_3x_type1087_spi          = 0x209102d5u;
constexpr uint32_t msgout_rtcm_3x_type1087_uart1        = 0x209102d2u;
constexpr uint32_t msgout_rtcm_3x_type1087_uart2        = 0x209102d3u;
constexpr uint32_t msgout_rtcm_3x_type1087_usb          = 0x209102d4u;
constexpr uint32_t msgout_rtcm_3x_type1094_i2c          = 0x20910368u;
constexpr uint32_t msgout_rtcm_3x_type1094_spi          = 0x2091036cu;
constexpr uint32_t msgout_rtcm_3x_type1094_uart1        = 0x20910369u;
constexpr uint32_t msgout_rtcm_3x_type1094_uart2        = 0x2091036au;
constexpr uint32_t msgout_rtcm_3x_type1094_usb          = 0x2091036bu;
constexpr uint32_t msgout_rtcm_3x_type1097_i2c          = 0x20910318u;
constexpr uint32_t msgout_rtcm_3x_type1097_spi          = 0x2091031cu;
constexpr uint32_t msgout_rtcm_3x_type1097_uart1        = 0x20910319u;
constexpr uint32_t msgout_rtcm_3x_type1097_uart2        = 0x2091031au;
constexpr uint32_t msgout_rtcm_3x_type1097_usb          = 0x2091031bu;
constexpr uint32_t msgout_rtcm_3x_type1124_i2c          = 0x2091036du;
constexpr uint32_t msgout_rtcm_3x_type1124_spi          = 0x20910371u;
constexpr uint32_t msgout_rtcm_3x_type1124_uart1        = 0x2091036eu;
constexpr uint32_t msgout_rtcm_3x_type1124_uart2        = 0x2091036fu;
constexpr uint32_t msgout_rtcm_3x_type1124_usb          = 0x20910370u;
constexpr uint32_t msgout_rtcm_3x_type1127_i2c          = 0x209102d6u;
constexpr uint32_t msgout_rtcm_3x_type1127_spi          = 0x209102dau;
constexpr uint32_t msgout_rtcm_3x_type1127_uart1        = 0x209102d7u;
constexpr uint32_t msgout_rtcm_3x_type1127_uart2        = 0x209102d8u;
constexpr uint32_t msgout_rtcm_3x_type1127_usb          = 0x209102d9u;
constexpr uint32_t msgout_rtcm_3x_type1230_i2c          = 0x20910303u;
constexpr uint32_t msgout_rtcm_3x_type1230_spi          = 0x20910307u;
constexpr uint32_t msgout_rtcm_3x_type1230_uart1        = 0x20910304u;
constexpr uint32_t msgout_rtcm_3x_type1230_uart2        = 0x20910305u;
constexpr uint32_t msgout_rtcm_3x_type1230_usb          = 0x20910306u;
constexpr uint32_t msgout_rtcm_3x_type4072_0_i2c        = 0x209102feu;
constexpr uint32_t msgout_rtcm_3x_type4072_0_spi        = 0x20910302u;
constexpr uint32_t msgout_rtcm_3x_type4072_0_uart1      = 0x209102ffu;
constexpr uint32_t msgout_rtcm_3x_type4072_0_uart2      = 0x20910300u;

// MSGOUT — UBX ESF
constexpr uint32_t msgout_ubx_esf_alg_i2c               = 0x2091010fu;
constexpr uint32_t msgout_ubx_esf_alg_spi               = 0x20910113u;
constexpr uint32_t msgout_ubx_esf_alg_uart1             = 0x20910110u;
constexpr uint32_t msgout_ubx_esf_alg_uart2             = 0x20910111u;
constexpr uint32_t msgout_ubx_esf_alg_usb               = 0x20910112u;
constexpr uint32_t msgout_ubx_esf_ins_i2c               = 0x20910114u;
constexpr uint32_t msgout_ubx_esf_ins_spi               = 0x20910118u;
constexpr uint32_t msgout_ubx_esf_ins_uart1             = 0x20910115u;
constexpr uint32_t msgout_ubx_esf_ins_uart2             = 0x20910116u;
constexpr uint32_t msgout_ubx_esf_ins_usb               = 0x20910117u;
constexpr uint32_t msgout_ubx_esf_meas_i2c              = 0x20910277u;
constexpr uint32_t msgout_ubx_esf_meas_spi              = 0x2091027bu;
constexpr uint32_t msgout_ubx_esf_meas_uart1            = 0x20910278u;
constexpr uint32_t msgout_ubx_esf_meas_uart2            = 0x20910279u;
constexpr uint32_t msgout_ubx_esf_meas_usb              = 0x2091027au;
constexpr uint32_t msgout_ubx_esf_raw_i2c               = 0x2091029fu;
constexpr uint32_t msgout_ubx_esf_raw_spi               = 0x209102a3u;
constexpr uint32_t msgout_ubx_esf_raw_uart1             = 0x209102a0u;
constexpr uint32_t msgout_ubx_esf_raw_uart2             = 0x209102a1u;
constexpr uint32_t msgout_ubx_esf_raw_usb               = 0x209102a2u;
constexpr uint32_t msgout_ubx_esf_status_i2c            = 0x20910105u;
constexpr uint32_t msgout_ubx_esf_status_spi            = 0x20910109u;
constexpr uint32_t msgout_ubx_esf_status_uart1          = 0x20910106u;
constexpr uint32_t msgout_ubx_esf_status_uart2          = 0x20910107u;
constexpr uint32_t msgout_ubx_esf_status_usb            = 0x20910108u;

// MSGOUT — UBX LOG
constexpr uint32_t msgout_ubx_log_info_i2c              = 0x20910259u;
constexpr uint32_t msgout_ubx_log_info_spi              = 0x2091025du;
constexpr uint32_t msgout_ubx_log_info_uart1            = 0x2091025au;
constexpr uint32_t msgout_ubx_log_info_uart2            = 0x2091025bu;
constexpr uint32_t msgout_ubx_log_info_usb              = 0x2091025cu;

// MSGOUT — UBX MON
constexpr uint32_t msgout_ubx_mon_comms_i2c             = 0x2091034fu;
constexpr uint32_t msgout_ubx_mon_comms_spi             = 0x20910353u;
constexpr uint32_t msgout_ubx_mon_comms_uart1           = 0x20910350u;
constexpr uint32_t msgout_ubx_mon_comms_uart2           = 0x20910351u;
constexpr uint32_t msgout_ubx_mon_comms_usb             = 0x20910352u;
constexpr uint32_t msgout_ubx_mon_hw2_i2c               = 0x209101b9u;
constexpr uint32_t msgout_ubx_mon_hw2_spi               = 0x209101bdu;
constexpr uint32_t msgout_ubx_mon_hw2_uart1             = 0x209101bau;
constexpr uint32_t msgout_ubx_mon_hw2_uart2             = 0x209101bbu;
constexpr uint32_t msgout_ubx_mon_hw2_usb               = 0x209101bcu;
constexpr uint32_t msgout_ubx_mon_hw3_i2c               = 0x20910354u;
constexpr uint32_t msgout_ubx_mon_hw3_spi               = 0x20910358u;
constexpr uint32_t msgout_ubx_mon_hw3_uart1             = 0x20910355u;
constexpr uint32_t msgout_ubx_mon_hw3_uart2             = 0x20910356u;
constexpr uint32_t msgout_ubx_mon_hw3_usb               = 0x20910357u;
constexpr uint32_t msgout_ubx_mon_hw_i2c                = 0x209101b4u;
constexpr uint32_t msgout_ubx_mon_hw_spi                = 0x209101b8u;
constexpr uint32_t msgout_ubx_mon_hw_uart1              = 0x209101b5u;
constexpr uint32_t msgout_ubx_mon_hw_uart2              = 0x209101b6u;
constexpr uint32_t msgout_ubx_mon_hw_usb                = 0x209101b7u;
constexpr uint32_t msgout_ubx_mon_io_i2c                = 0x209101a5u;
constexpr uint32_t msgout_ubx_mon_io_spi                = 0x209101a9u;
constexpr uint32_t msgout_ubx_mon_io_uart1              = 0x209101a6u;
constexpr uint32_t msgout_ubx_mon_io_uart2              = 0x209101a7u;
constexpr uint32_t msgout_ubx_mon_io_usb                = 0x209101a8u;
constexpr uint32_t msgout_ubx_mon_msgpp_i2c             = 0x20910196u;
constexpr uint32_t msgout_ubx_mon_msgpp_spi             = 0x2091019au;
constexpr uint32_t msgout_ubx_mon_msgpp_uart1           = 0x20910197u;
constexpr uint32_t msgout_ubx_mon_msgpp_uart2           = 0x20910198u;
constexpr uint32_t msgout_ubx_mon_msgpp_usb             = 0x20910199u;
constexpr uint32_t msgout_ubx_mon_rf_i2c                = 0x20910359u;
constexpr uint32_t msgout_ubx_mon_rf_spi                = 0x2091035du;
constexpr uint32_t msgout_ubx_mon_rf_uart1              = 0x2091035au;
constexpr uint32_t msgout_ubx_mon_rf_uart2              = 0x2091035bu;
constexpr uint32_t msgout_ubx_mon_rf_usb                = 0x2091035cu;
constexpr uint32_t msgout_ubx_mon_rxbuf_i2c             = 0x209101a0u;
constexpr uint32_t msgout_ubx_mon_rxbuf_spi             = 0x209101a4u;
constexpr uint32_t msgout_ubx_mon_rxbuf_uart1           = 0x209101a1u;
constexpr uint32_t msgout_ubx_mon_rxbuf_uart2           = 0x209101a2u;
constexpr uint32_t msgout_ubx_mon_rxbuf_usb             = 0x209101a3u;
constexpr uint32_t msgout_ubx_mon_rxr_i2c               = 0x20910187u;
constexpr uint32_t msgout_ubx_mon_rxr_spi               = 0x2091018bu;
constexpr uint32_t msgout_ubx_mon_rxr_uart1             = 0x20910188u;
constexpr uint32_t msgout_ubx_mon_rxr_uart2             = 0x20910189u;
constexpr uint32_t msgout_ubx_mon_rxr_usb               = 0x2091018au;
constexpr uint32_t msgout_ubx_mon_span_i2c              = 0x2091038bu;
constexpr uint32_t msgout_ubx_mon_span_spi              = 0x2091038fu;
constexpr uint32_t msgout_ubx_mon_span_uart1            = 0x2091038cu;
constexpr uint32_t msgout_ubx_mon_span_uart2            = 0x2091038du;
constexpr uint32_t msgout_ubx_mon_span_usb              = 0x2091038eu;
constexpr uint32_t msgout_ubx_mon_sys_i2c               = 0x2091069du;
constexpr uint32_t msgout_ubx_mon_sys_spi               = 0x209106a1u;
constexpr uint32_t msgout_ubx_mon_sys_uart1             = 0x2091069eu;
constexpr uint32_t msgout_ubx_mon_sys_uart2             = 0x2091069fu;
constexpr uint32_t msgout_ubx_mon_sys_usb               = 0x209106a0u;
constexpr uint32_t msgout_ubx_mon_txbuf_i2c             = 0x2091019bu;
constexpr uint32_t msgout_ubx_mon_txbuf_spi             = 0x2091019fu;
constexpr uint32_t msgout_ubx_mon_txbuf_uart1           = 0x2091019cu;
constexpr uint32_t msgout_ubx_mon_txbuf_uart2           = 0x2091019du;
constexpr uint32_t msgout_ubx_mon_txbuf_usb             = 0x2091019eu;

// MSGOUT — UBX NAV2
constexpr uint32_t msgout_ubx_nav2_clock_i2c            = 0x20910430u;
constexpr uint32_t msgout_ubx_nav2_clock_spi            = 0x20910434u;
constexpr uint32_t msgout_ubx_nav2_clock_uart1          = 0x20910431u;
constexpr uint32_t msgout_ubx_nav2_clock_uart2          = 0x20910432u;
constexpr uint32_t msgout_ubx_nav2_clock_usb            = 0x20910433u;
constexpr uint32_t msgout_ubx_nav2_cov_i2c              = 0x20910435u;
constexpr uint32_t msgout_ubx_nav2_cov_spi              = 0x20910439u;
constexpr uint32_t msgout_ubx_nav2_cov_uart1            = 0x20910436u;
constexpr uint32_t msgout_ubx_nav2_cov_uart2            = 0x20910437u;
constexpr uint32_t msgout_ubx_nav2_cov_usb              = 0x20910438u;
constexpr uint32_t msgout_ubx_nav2_dop_i2c              = 0x20910465u;
constexpr uint32_t msgout_ubx_nav2_dop_spi              = 0x20910469u;
constexpr uint32_t msgout_ubx_nav2_dop_uart1            = 0x20910466u;
constexpr uint32_t msgout_ubx_nav2_dop_uart2            = 0x20910467u;
constexpr uint32_t msgout_ubx_nav2_dop_usb              = 0x20910468u;
constexpr uint32_t msgout_ubx_nav2_eoe_i2c              = 0x20910565u;
constexpr uint32_t msgout_ubx_nav2_eoe_spi              = 0x20910569u;
constexpr uint32_t msgout_ubx_nav2_eoe_uart1            = 0x20910566u;
constexpr uint32_t msgout_ubx_nav2_eoe_uart2            = 0x20910567u;
constexpr uint32_t msgout_ubx_nav2_eoe_usb              = 0x20910568u;
constexpr uint32_t msgout_ubx_nav2_odo_i2c              = 0x20910475u;
constexpr uint32_t msgout_ubx_nav2_odo_spi              = 0x20910479u;
constexpr uint32_t msgout_ubx_nav2_odo_uart1            = 0x20910476u;
constexpr uint32_t msgout_ubx_nav2_odo_uart2            = 0x20910477u;
constexpr uint32_t msgout_ubx_nav2_odo_usb              = 0x20910478u;
constexpr uint32_t msgout_ubx_nav2_posecef_i2c          = 0x20910480u;
constexpr uint32_t msgout_ubx_nav2_posecef_spi          = 0x20910484u;
constexpr uint32_t msgout_ubx_nav2_posecef_uart1        = 0x20910481u;
constexpr uint32_t msgout_ubx_nav2_posecef_uart2        = 0x20910482u;
constexpr uint32_t msgout_ubx_nav2_posecef_usb          = 0x20910483u;
constexpr uint32_t msgout_ubx_nav2_posllh_i2c           = 0x20910485u;
constexpr uint32_t msgout_ubx_nav2_posllh_spi           = 0x20910489u;
constexpr uint32_t msgout_ubx_nav2_posllh_uart1         = 0x20910486u;
constexpr uint32_t msgout_ubx_nav2_posllh_uart2         = 0x20910487u;
constexpr uint32_t msgout_ubx_nav2_posllh_usb           = 0x20910488u;
constexpr uint32_t msgout_ubx_nav2_pvt_i2c              = 0x20910490u;
constexpr uint32_t msgout_ubx_nav2_pvt_spi              = 0x20910494u;
constexpr uint32_t msgout_ubx_nav2_pvt_uart1            = 0x20910491u;
constexpr uint32_t msgout_ubx_nav2_pvt_uart2            = 0x20910492u;
constexpr uint32_t msgout_ubx_nav2_pvt_usb              = 0x20910493u;
constexpr uint32_t msgout_ubx_nav2_sat_i2c              = 0x20910495u;
constexpr uint32_t msgout_ubx_nav2_sat_spi              = 0x20910499u;
constexpr uint32_t msgout_ubx_nav2_sat_uart1            = 0x20910496u;
constexpr uint32_t msgout_ubx_nav2_sat_uart2            = 0x20910497u;
constexpr uint32_t msgout_ubx_nav2_sat_usb              = 0x20910498u;
constexpr uint32_t msgout_ubx_nav2_sbas_i2c             = 0x20910500u;
constexpr uint32_t msgout_ubx_nav2_sbas_spi             = 0x20910504u;
constexpr uint32_t msgout_ubx_nav2_sbas_uart1           = 0x20910501u;
constexpr uint32_t msgout_ubx_nav2_sbas_uart2           = 0x20910502u;
constexpr uint32_t msgout_ubx_nav2_sbas_usb             = 0x20910503u;
constexpr uint32_t msgout_ubx_nav2_sig_i2c              = 0x20910505u;
constexpr uint32_t msgout_ubx_nav2_sig_spi              = 0x20910509u;
constexpr uint32_t msgout_ubx_nav2_sig_uart1            = 0x20910506u;
constexpr uint32_t msgout_ubx_nav2_sig_uart2            = 0x20910507u;
constexpr uint32_t msgout_ubx_nav2_sig_usb              = 0x20910508u;
constexpr uint32_t msgout_ubx_nav2_slas_i2c             = 0x20910510u;
constexpr uint32_t msgout_ubx_nav2_slas_spi             = 0x20910514u;
constexpr uint32_t msgout_ubx_nav2_slas_uart1           = 0x20910511u;
constexpr uint32_t msgout_ubx_nav2_slas_uart2           = 0x20910512u;
constexpr uint32_t msgout_ubx_nav2_slas_usb             = 0x20910513u;
constexpr uint32_t msgout_ubx_nav2_status_i2c           = 0x20910515u;
constexpr uint32_t msgout_ubx_nav2_status_spi           = 0x20910519u;
constexpr uint32_t msgout_ubx_nav2_status_uart1         = 0x20910516u;
constexpr uint32_t msgout_ubx_nav2_status_uart2         = 0x20910517u;
constexpr uint32_t msgout_ubx_nav2_status_usb           = 0x20910518u;
constexpr uint32_t msgout_ubx_nav2_svin_i2c             = 0x20910520u;
constexpr uint32_t msgout_ubx_nav2_svin_spi             = 0x20910524u;
constexpr uint32_t msgout_ubx_nav2_svin_uart1           = 0x20910521u;
constexpr uint32_t msgout_ubx_nav2_svin_uart2           = 0x20910522u;
constexpr uint32_t msgout_ubx_nav2_svin_usb             = 0x20910523u;
constexpr uint32_t msgout_ubx_nav2_timebds_i2c          = 0x20910525u;
constexpr uint32_t msgout_ubx_nav2_timebds_spi          = 0x20910529u;
constexpr uint32_t msgout_ubx_nav2_timebds_uart1        = 0x20910526u;
constexpr uint32_t msgout_ubx_nav2_timebds_uart2        = 0x20910527u;
constexpr uint32_t msgout_ubx_nav2_timebds_usb          = 0x20910528u;
constexpr uint32_t msgout_ubx_nav2_timegal_i2c          = 0x20910530u;
constexpr uint32_t msgout_ubx_nav2_timegal_spi          = 0x20910534u;
constexpr uint32_t msgout_ubx_nav2_timegal_uart1        = 0x20910531u;
constexpr uint32_t msgout_ubx_nav2_timegal_uart2        = 0x20910532u;
constexpr uint32_t msgout_ubx_nav2_timegal_usb          = 0x20910533u;
constexpr uint32_t msgout_ubx_nav2_timeglo_i2c          = 0x20910535u;
constexpr uint32_t msgout_ubx_nav2_timeglo_spi          = 0x20910539u;
constexpr uint32_t msgout_ubx_nav2_timeglo_uart1        = 0x20910536u;
constexpr uint32_t msgout_ubx_nav2_timeglo_uart2        = 0x20910537u;
constexpr uint32_t msgout_ubx_nav2_timeglo_usb          = 0x20910538u;
constexpr uint32_t msgout_ubx_nav2_timegps_i2c          = 0x20910540u;
constexpr uint32_t msgout_ubx_nav2_timegps_spi          = 0x20910544u;
constexpr uint32_t msgout_ubx_nav2_timegps_uart1        = 0x20910541u;
constexpr uint32_t msgout_ubx_nav2_timegps_uart2        = 0x20910542u;
constexpr uint32_t msgout_ubx_nav2_timegps_usb          = 0x20910543u;
constexpr uint32_t msgout_ubx_nav2_timels_i2c           = 0x20910545u;
constexpr uint32_t msgout_ubx_nav2_timels_spi           = 0x20910549u;
constexpr uint32_t msgout_ubx_nav2_timels_uart1         = 0x20910546u;
constexpr uint32_t msgout_ubx_nav2_timels_uart2         = 0x20910547u;
constexpr uint32_t msgout_ubx_nav2_timels_usb           = 0x20910548u;
constexpr uint32_t msgout_ubx_nav2_timeqzss_i2c         = 0x20910575u;
constexpr uint32_t msgout_ubx_nav2_timeqzss_spi         = 0x20910579u;
constexpr uint32_t msgout_ubx_nav2_timeqzss_uart1       = 0x20910576u;
constexpr uint32_t msgout_ubx_nav2_timeqzss_uart2       = 0x20910577u;
constexpr uint32_t msgout_ubx_nav2_timeqzss_usb         = 0x20910578u;
constexpr uint32_t msgout_ubx_nav2_timeutc_i2c          = 0x20910550u;
constexpr uint32_t msgout_ubx_nav2_timeutc_spi          = 0x20910554u;
constexpr uint32_t msgout_ubx_nav2_timeutc_uart1        = 0x20910551u;
constexpr uint32_t msgout_ubx_nav2_timeutc_uart2        = 0x20910552u;
constexpr uint32_t msgout_ubx_nav2_timeutc_usb          = 0x20910553u;
constexpr uint32_t msgout_ubx_nav2_velecef_i2c          = 0x20910555u;
constexpr uint32_t msgout_ubx_nav2_velecef_spi          = 0x20910559u;
constexpr uint32_t msgout_ubx_nav2_velecef_uart1        = 0x20910556u;
constexpr uint32_t msgout_ubx_nav2_velecef_uart2        = 0x20910557u;
constexpr uint32_t msgout_ubx_nav2_velecef_usb          = 0x20910558u;
constexpr uint32_t msgout_ubx_nav2_velned_i2c           = 0x20910560u;
constexpr uint32_t msgout_ubx_nav2_velned_spi           = 0x20910564u;
constexpr uint32_t msgout_ubx_nav2_velned_uart1         = 0x20910561u;
constexpr uint32_t msgout_ubx_nav2_velned_uart2         = 0x20910562u;
constexpr uint32_t msgout_ubx_nav2_velned_usb           = 0x20910563u;

// MSGOUT — UBX NAV
constexpr uint32_t msgout_ubx_nav_att_i2c               = 0x2091001fu;
constexpr uint32_t msgout_ubx_nav_att_spi               = 0x20910023u;
constexpr uint32_t msgout_ubx_nav_att_uart1             = 0x20910020u;
constexpr uint32_t msgout_ubx_nav_att_uart2             = 0x20910021u;
constexpr uint32_t msgout_ubx_nav_att_usb               = 0x20910022u;
constexpr uint32_t msgout_ubx_nav_aopstatus_i2c         = 0x20910079u;
constexpr uint32_t msgout_ubx_nav_aopstatus_spi         = 0x2091007du;
constexpr uint32_t msgout_ubx_nav_aopstatus_uart1       = 0x2091007au;
constexpr uint32_t msgout_ubx_nav_aopstatus_uart2       = 0x2091007bu;
constexpr uint32_t msgout_ubx_nav_aopstatus_usb         = 0x2091007cu;
constexpr uint32_t msgout_ubx_nav_clock_i2c             = 0x20910065u;
constexpr uint32_t msgout_ubx_nav_clock_spi             = 0x20910069u;
constexpr uint32_t msgout_ubx_nav_clock_uart1           = 0x20910066u;
constexpr uint32_t msgout_ubx_nav_clock_uart2           = 0x20910067u;
constexpr uint32_t msgout_ubx_nav_clock_usb             = 0x20910068u;
constexpr uint32_t msgout_ubx_nav_cov_i2c               = 0x20910083u;
constexpr uint32_t msgout_ubx_nav_cov_spi               = 0x20910087u;
constexpr uint32_t msgout_ubx_nav_cov_uart1             = 0x20910084u;
constexpr uint32_t msgout_ubx_nav_cov_uart2             = 0x20910085u;
constexpr uint32_t msgout_ubx_nav_cov_usb               = 0x20910086u;
constexpr uint32_t msgout_ubx_nav_dop_i2c               = 0x20910038u;
constexpr uint32_t msgout_ubx_nav_dop_spi               = 0x2091003cu;
constexpr uint32_t msgout_ubx_nav_dop_uart1             = 0x20910039u;
constexpr uint32_t msgout_ubx_nav_dop_uart2             = 0x2091003au;
constexpr uint32_t msgout_ubx_nav_dop_usb               = 0x2091003bu;
constexpr uint32_t msgout_ubx_nav_eell_i2c              = 0x20910313u;
constexpr uint32_t msgout_ubx_nav_eell_spi              = 0x20910317u;
constexpr uint32_t msgout_ubx_nav_eell_uart1            = 0x20910314u;
constexpr uint32_t msgout_ubx_nav_eell_uart2            = 0x20910315u;
constexpr uint32_t msgout_ubx_nav_eell_usb              = 0x20910316u;
constexpr uint32_t msgout_ubx_nav_eoe_i2c               = 0x2091015fu;
constexpr uint32_t msgout_ubx_nav_eoe_spi               = 0x20910163u;
constexpr uint32_t msgout_ubx_nav_eoe_uart1             = 0x20910160u;
constexpr uint32_t msgout_ubx_nav_eoe_uart2             = 0x20910161u;
constexpr uint32_t msgout_ubx_nav_eoe_usb               = 0x20910162u;
constexpr uint32_t msgout_ubx_nav_geofence_i2c          = 0x209100a1u;
constexpr uint32_t msgout_ubx_nav_geofence_spi          = 0x209100a5u;
constexpr uint32_t msgout_ubx_nav_geofence_uart1        = 0x209100a2u;
constexpr uint32_t msgout_ubx_nav_geofence_uart2        = 0x209100a3u;
constexpr uint32_t msgout_ubx_nav_geofence_usb          = 0x209100a4u;
constexpr uint32_t msgout_ubx_nav_hpposecef_i2c         = 0x2091002eu;
constexpr uint32_t msgout_ubx_nav_hpposecef_spi         = 0x20910032u;
constexpr uint32_t msgout_ubx_nav_hpposecef_uart1       = 0x2091002fu;
constexpr uint32_t msgout_ubx_nav_hpposecef_uart2       = 0x20910030u;
constexpr uint32_t msgout_ubx_nav_hpposecef_usb         = 0x20910031u;
constexpr uint32_t msgout_ubx_nav_hpposllh_i2c          = 0x20910033u;
constexpr uint32_t msgout_ubx_nav_hpposllh_spi          = 0x20910037u;
constexpr uint32_t msgout_ubx_nav_hpposllh_uart1        = 0x20910034u;
constexpr uint32_t msgout_ubx_nav_hpposllh_uart2        = 0x20910035u;
constexpr uint32_t msgout_ubx_nav_hpposllh_usb          = 0x20910036u;
constexpr uint32_t msgout_ubx_nav_odo_i2c               = 0x2091007eu;
constexpr uint32_t msgout_ubx_nav_odo_spi               = 0x20910082u;
constexpr uint32_t msgout_ubx_nav_odo_uart1             = 0x2091007fu;
constexpr uint32_t msgout_ubx_nav_odo_uart2             = 0x20910080u;
constexpr uint32_t msgout_ubx_nav_odo_usb               = 0x20910081u;
constexpr uint32_t msgout_ubx_nav_orb_i2c               = 0x20910010u;
constexpr uint32_t msgout_ubx_nav_orb_spi               = 0x20910014u;
constexpr uint32_t msgout_ubx_nav_orb_uart1             = 0x20910011u;
constexpr uint32_t msgout_ubx_nav_orb_uart2             = 0x20910012u;
constexpr uint32_t msgout_ubx_nav_orb_usb               = 0x20910013u;
constexpr uint32_t msgout_ubx_nav_pl_i2c                = 0x20910415u;
constexpr uint32_t msgout_ubx_nav_pl_spi                = 0x20910419u;
constexpr uint32_t msgout_ubx_nav_pl_uart1              = 0x20910416u;
constexpr uint32_t msgout_ubx_nav_pl_uart2              = 0x20910417u;
constexpr uint32_t msgout_ubx_nav_pl_usb                = 0x20910418u;
constexpr uint32_t msgout_ubx_nav_posecef_i2c           = 0x20910024u;
constexpr uint32_t msgout_ubx_nav_posecef_spi           = 0x20910028u;
constexpr uint32_t msgout_ubx_nav_posecef_uart1         = 0x20910025u;
constexpr uint32_t msgout_ubx_nav_posecef_uart2         = 0x20910026u;
constexpr uint32_t msgout_ubx_nav_posecef_usb           = 0x20910027u;
constexpr uint32_t msgout_ubx_nav_posllh_i2c            = 0x20910029u;
constexpr uint32_t msgout_ubx_nav_posllh_spi            = 0x2091002du;
constexpr uint32_t msgout_ubx_nav_posllh_uart1          = 0x2091002au;
constexpr uint32_t msgout_ubx_nav_posllh_uart2          = 0x2091002bu;
constexpr uint32_t msgout_ubx_nav_posllh_usb            = 0x2091002cu;
constexpr uint32_t msgout_ubx_nav_pvt_i2c               = 0x20910006u;
constexpr uint32_t msgout_ubx_nav_pvt_spi               = 0x2091000au;
constexpr uint32_t msgout_ubx_nav_pvt_uart1             = 0x20910007u;
constexpr uint32_t msgout_ubx_nav_pvt_uart2             = 0x20910008u;
constexpr uint32_t msgout_ubx_nav_pvt_usb               = 0x20910009u;
constexpr uint32_t msgout_ubx_nav_relposned_i2c         = 0x2091008du;
constexpr uint32_t msgout_ubx_nav_relposned_spi         = 0x20910091u;
constexpr uint32_t msgout_ubx_nav_relposned_uart1       = 0x2091008eu;
constexpr uint32_t msgout_ubx_nav_relposned_uart2       = 0x2091008fu;
constexpr uint32_t msgout_ubx_nav_relposned_usb         = 0x20910090u;
constexpr uint32_t msgout_ubx_nav_sat_i2c               = 0x20910015u;
constexpr uint32_t msgout_ubx_nav_sat_spi               = 0x20910019u;
constexpr uint32_t msgout_ubx_nav_sat_uart1             = 0x20910016u;
constexpr uint32_t msgout_ubx_nav_sat_uart2             = 0x20910017u;
constexpr uint32_t msgout_ubx_nav_sat_usb               = 0x20910018u;
constexpr uint32_t msgout_ubx_nav_sbas_i2c              = 0x2091006au;
constexpr uint32_t msgout_ubx_nav_sbas_spi              = 0x2091006eu;
constexpr uint32_t msgout_ubx_nav_sbas_uart1            = 0x2091006bu;
constexpr uint32_t msgout_ubx_nav_sbas_uart2            = 0x2091006cu;
constexpr uint32_t msgout_ubx_nav_sbas_usb              = 0x2091006du;
constexpr uint32_t msgout_ubx_nav_sig_i2c               = 0x20910345u;
constexpr uint32_t msgout_ubx_nav_sig_spi               = 0x20910349u;
constexpr uint32_t msgout_ubx_nav_sig_uart1             = 0x20910346u;
constexpr uint32_t msgout_ubx_nav_sig_uart2             = 0x20910347u;
constexpr uint32_t msgout_ubx_nav_sig_usb               = 0x20910348u;
constexpr uint32_t msgout_ubx_nav_slas_i2c              = 0x20910336u;
constexpr uint32_t msgout_ubx_nav_slas_spi              = 0x2091033au;
constexpr uint32_t msgout_ubx_nav_slas_uart1            = 0x20910337u;
constexpr uint32_t msgout_ubx_nav_slas_uart2            = 0x20910338u;
constexpr uint32_t msgout_ubx_nav_slas_usb              = 0x20910339u;
constexpr uint32_t msgout_ubx_nav_status_i2c            = 0x2091001au;
constexpr uint32_t msgout_ubx_nav_status_spi            = 0x2091001eu;
constexpr uint32_t msgout_ubx_nav_status_uart1          = 0x2091001bu;
constexpr uint32_t msgout_ubx_nav_status_uart2          = 0x2091001cu;
constexpr uint32_t msgout_ubx_nav_status_usb            = 0x2091001du;
constexpr uint32_t msgout_ubx_nav_svin_i2c              = 0x20910088u;
constexpr uint32_t msgout_ubx_nav_svin_spi              = 0x2091008cu;
constexpr uint32_t msgout_ubx_nav_svin_uart1            = 0x20910089u;
constexpr uint32_t msgout_ubx_nav_svin_uart2            = 0x2091008au;
constexpr uint32_t msgout_ubx_nav_svin_usb              = 0x2091008bu;
constexpr uint32_t msgout_ubx_nav_timebds_i2c           = 0x20910051u;
constexpr uint32_t msgout_ubx_nav_timebds_spi           = 0x20910055u;
constexpr uint32_t msgout_ubx_nav_timebds_uart1         = 0x20910052u;
constexpr uint32_t msgout_ubx_nav_timebds_uart2         = 0x20910053u;
constexpr uint32_t msgout_ubx_nav_timebds_usb           = 0x20910054u;
constexpr uint32_t msgout_ubx_nav_timegal_i2c           = 0x20910056u;
constexpr uint32_t msgout_ubx_nav_timegal_spi           = 0x2091005au;
constexpr uint32_t msgout_ubx_nav_timegal_uart1         = 0x20910057u;
constexpr uint32_t msgout_ubx_nav_timegal_uart2         = 0x20910058u;
constexpr uint32_t msgout_ubx_nav_timegal_usb           = 0x20910059u;
constexpr uint32_t msgout_ubx_nav_timeglo_i2c           = 0x2091004cu;
constexpr uint32_t msgout_ubx_nav_timeglo_spi           = 0x20910050u;
constexpr uint32_t msgout_ubx_nav_timeglo_uart1         = 0x2091004du;
constexpr uint32_t msgout_ubx_nav_timeglo_uart2         = 0x2091004eu;
constexpr uint32_t msgout_ubx_nav_timeglo_usb           = 0x2091004fu;
constexpr uint32_t msgout_ubx_nav_timegps_i2c           = 0x20910047u;
constexpr uint32_t msgout_ubx_nav_timegps_spi           = 0x2091004bu;
constexpr uint32_t msgout_ubx_nav_timegps_uart1         = 0x20910048u;
constexpr uint32_t msgout_ubx_nav_timegps_uart2         = 0x20910049u;
constexpr uint32_t msgout_ubx_nav_timegps_usb           = 0x2091004au;
constexpr uint32_t msgout_ubx_nav_timels_i2c            = 0x20910060u;
constexpr uint32_t msgout_ubx_nav_timels_spi            = 0x20910064u;
constexpr uint32_t msgout_ubx_nav_timels_uart1          = 0x20910061u;
constexpr uint32_t msgout_ubx_nav_timels_uart2          = 0x20910062u;
constexpr uint32_t msgout_ubx_nav_timels_usb            = 0x20910063u;
constexpr uint32_t msgout_ubx_nav_timeqzss_i2c          = 0x20910386u;
constexpr uint32_t msgout_ubx_nav_timeqzss_spi          = 0x2091038au;
constexpr uint32_t msgout_ubx_nav_timeqzss_uart1        = 0x20910387u;
constexpr uint32_t msgout_ubx_nav_timeqzss_uart2        = 0x20910388u;
constexpr uint32_t msgout_ubx_nav_timeqzss_usb          = 0x20910389u;
constexpr uint32_t msgout_ubx_nav_timeutc_i2c           = 0x2091005bu;
constexpr uint32_t msgout_ubx_nav_timeutc_spi           = 0x2091005fu;
constexpr uint32_t msgout_ubx_nav_timeutc_uart1         = 0x2091005cu;
constexpr uint32_t msgout_ubx_nav_timeutc_uart2         = 0x2091005du;
constexpr uint32_t msgout_ubx_nav_timeutc_usb           = 0x2091005eu;
constexpr uint32_t msgout_ubx_nav_velecef_i2c           = 0x2091003du;
constexpr uint32_t msgout_ubx_nav_velecef_spi           = 0x20910041u;
constexpr uint32_t msgout_ubx_nav_velecef_uart1         = 0x2091003eu;
constexpr uint32_t msgout_ubx_nav_velecef_uart2         = 0x2091003fu;
constexpr uint32_t msgout_ubx_nav_velecef_usb           = 0x20910040u;
constexpr uint32_t msgout_ubx_nav_velned_i2c            = 0x20910042u;
constexpr uint32_t msgout_ubx_nav_velned_spi            = 0x20910046u;
constexpr uint32_t msgout_ubx_nav_velned_uart1          = 0x20910043u;
constexpr uint32_t msgout_ubx_nav_velned_uart2          = 0x20910044u;
constexpr uint32_t msgout_ubx_nav_velned_usb            = 0x20910045u;

// MSGOUT — UBX RXM
constexpr uint32_t msgout_ubx_rxm_cor_i2c               = 0x209106b6u;
constexpr uint32_t msgout_ubx_rxm_cor_spi               = 0x209106bau;
constexpr uint32_t msgout_ubx_rxm_cor_uart1             = 0x209106b7u;
constexpr uint32_t msgout_ubx_rxm_cor_uart2             = 0x209106b8u;
constexpr uint32_t msgout_ubx_rxm_cor_usb               = 0x209106b9u;
constexpr uint32_t msgout_ubx_rxm_measx_i2c             = 0x20910204u;
constexpr uint32_t msgout_ubx_rxm_measx_spi             = 0x20910208u;
constexpr uint32_t msgout_ubx_rxm_measx_uart1           = 0x20910205u;
constexpr uint32_t msgout_ubx_rxm_measx_uart2           = 0x20910206u;
constexpr uint32_t msgout_ubx_rxm_measx_usb             = 0x20910207u;
constexpr uint32_t msgout_ubx_rxm_pmp_i2c               = 0x2091031du;
constexpr uint32_t msgout_ubx_rxm_pmp_spi               = 0x20910321u;
constexpr uint32_t msgout_ubx_rxm_pmp_uart1             = 0x2091031eu;
constexpr uint32_t msgout_ubx_rxm_pmp_uart2             = 0x2091031fu;
constexpr uint32_t msgout_ubx_rxm_pmp_usb               = 0x20910320u;
constexpr uint32_t msgout_ubx_rxm_qzssl6_uart1          = 0x2091033bu;
constexpr uint32_t msgout_ubx_rxm_qzssl6_uart2          = 0x2091033cu;
constexpr uint32_t msgout_ubx_rxm_qzssl6_usb            = 0x2091033du;
constexpr uint32_t msgout_ubx_rxm_qzssl6_i2c            = 0x2091033fu;
constexpr uint32_t msgout_ubx_rxm_qzssl6_spi            = 0x2091033eu;
constexpr uint32_t msgout_ubx_rxm_rawx_i2c              = 0x209102a4u;
constexpr uint32_t msgout_ubx_rxm_rawx_spi              = 0x209102a8u;
constexpr uint32_t msgout_ubx_rxm_rawx_uart1            = 0x209102a5u;
constexpr uint32_t msgout_ubx_rxm_rawx_uart2            = 0x209102a6u;
constexpr uint32_t msgout_ubx_rxm_rawx_usb              = 0x209102a7u;
constexpr uint32_t msgout_ubx_rxm_rlm_i2c               = 0x2091025eu;
constexpr uint32_t msgout_ubx_rxm_rlm_spi               = 0x20910262u;
constexpr uint32_t msgout_ubx_rxm_rlm_uart1             = 0x2091025fu;
constexpr uint32_t msgout_ubx_rxm_rlm_uart2             = 0x20910260u;
constexpr uint32_t msgout_ubx_rxm_rlm_usb               = 0x20910261u;
constexpr uint32_t msgout_ubx_rxm_rtc5_i2c              = 0x20910155u;
constexpr uint32_t msgout_ubx_rxm_rtc5_spi              = 0x20910159u;
constexpr uint32_t msgout_ubx_rxm_rtc5_uart1            = 0x20910156u;
constexpr uint32_t msgout_ubx_rxm_rtc5_uart2            = 0x20910157u;
constexpr uint32_t msgout_ubx_rxm_rtc5_usb              = 0x20910158u;
constexpr uint32_t msgout_ubx_rxm_rtcm_i2c              = 0x20910268u;
constexpr uint32_t msgout_ubx_rxm_rtcm_spi              = 0x2091026cu;
constexpr uint32_t msgout_ubx_rxm_rtcm_uart1            = 0x20910269u;
constexpr uint32_t msgout_ubx_rxm_rtcm_uart2            = 0x2091026au;
constexpr uint32_t msgout_ubx_rxm_rtcm_usb              = 0x2091026bu;
constexpr uint32_t msgout_ubx_rxm_sfrbx_i2c             = 0x20910231u;
constexpr uint32_t msgout_ubx_rxm_sfrbx_spi             = 0x20910235u;
constexpr uint32_t msgout_ubx_rxm_sfrbx_uart1           = 0x20910232u;
constexpr uint32_t msgout_ubx_rxm_sfrbx_uart2           = 0x20910233u;
constexpr uint32_t msgout_ubx_rxm_sfrbx_usb             = 0x20910234u;
constexpr uint32_t msgout_ubx_rxm_spartn_i2c            = 0x20910605u;
constexpr uint32_t msgout_ubx_rxm_spartn_spi            = 0x20910609u;
constexpr uint32_t msgout_ubx_rxm_spartn_uart1          = 0x20910606u;
constexpr uint32_t msgout_ubx_rxm_spartn_uart2          = 0x20910607u;
constexpr uint32_t msgout_ubx_rxm_spartn_usb            = 0x20910608u;
constexpr uint32_t msgout_ubx_rxm_tm_i2c                = 0x20910610u;
constexpr uint32_t msgout_ubx_rxm_tm_spi                = 0x20910614u;
constexpr uint32_t msgout_ubx_rxm_tm_uart1              = 0x20910611u;
constexpr uint32_t msgout_ubx_rxm_tm_uart2              = 0x20910612u;
constexpr uint32_t msgout_ubx_rxm_tm_usb                = 0x20910613u;

// MSGOUT — UBX SEC
constexpr uint32_t msgout_ubx_sec_crc_i2c               = 0x209103b2u;
constexpr uint32_t msgout_ubx_sec_crc_spi               = 0x209103b6u;
constexpr uint32_t msgout_ubx_sec_crc_uart1             = 0x209103b3u;
constexpr uint32_t msgout_ubx_sec_crc_uart2             = 0x209103b4u;
constexpr uint32_t msgout_ubx_sec_crc_usb               = 0x209103b5u;
constexpr uint32_t msgout_ubx_sec_sig_i2c               = 0x20910634u;
constexpr uint32_t msgout_ubx_sec_sig_spi               = 0x20910638u;
constexpr uint32_t msgout_ubx_sec_sig_uart1             = 0x20910635u;
constexpr uint32_t msgout_ubx_sec_sig_uart2             = 0x20910636u;
constexpr uint32_t msgout_ubx_sec_sig_usb               = 0x20910637u;
constexpr uint32_t msgout_ubx_sec_siglog_i2c            = 0x20910689u;
constexpr uint32_t msgout_ubx_sec_siglog_spi            = 0x2091068du;
constexpr uint32_t msgout_ubx_sec_siglog_uart1          = 0x2091068au;
constexpr uint32_t msgout_ubx_sec_siglog_uart2          = 0x2091068bu;
constexpr uint32_t msgout_ubx_sec_siglog_usb            = 0x2091068cu;

// MSGOUT — UBX TIM
constexpr uint32_t msgout_ubx_tim_tm2_i2c               = 0x20910178u;
constexpr uint32_t msgout_ubx_tim_tm2_spi               = 0x2091017cu;
constexpr uint32_t msgout_ubx_tim_tm2_uart1             = 0x20910179u;
constexpr uint32_t msgout_ubx_tim_tm2_uart2             = 0x2091017au;
constexpr uint32_t msgout_ubx_tim_tm2_usb               = 0x2091017bu;
constexpr uint32_t msgout_ubx_tim_tp_i2c                = 0x2091017du;
constexpr uint32_t msgout_ubx_tim_tp_spi                = 0x20910181u;
constexpr uint32_t msgout_ubx_tim_tp_uart1              = 0x2091017eu;
constexpr uint32_t msgout_ubx_tim_tp_uart2              = 0x2091017fu;
constexpr uint32_t msgout_ubx_tim_tp_usb                = 0x20910180u;
constexpr uint32_t msgout_ubx_tim_vrfy_i2c              = 0x20910092u;
constexpr uint32_t msgout_ubx_tim_vrfy_spi              = 0x20910096u;
constexpr uint32_t msgout_ubx_tim_vrfy_uart1            = 0x20910093u;
constexpr uint32_t msgout_ubx_tim_vrfy_uart2            = 0x20910094u;
constexpr uint32_t msgout_ubx_tim_vrfy_usb              = 0x20910095u;

// NAV2
constexpr uint32_t nav2_out_enabled                     = 0x10170001u;
constexpr uint32_t nav2_sbas_use_integrity              = 0x10170002u;

// NAVHPG
constexpr uint32_t navhpg_dgnssmode                     = 0x20140011u;

// NAVSPG
constexpr uint32_t navspg_fixmode                       = 0x20110011u;
constexpr uint32_t navspg_inifix3d                      = 0x10110013u;
constexpr uint32_t navspg_wknrollover                   = 0x30110017u;
constexpr uint32_t navspg_use_ppp                       = 0x10110019u;
constexpr uint32_t navspg_utcstandard                   = 0x2011001cu;
constexpr uint32_t navspg_dynmodel                      = 0x20110021u;
constexpr uint32_t navspg_ackaiding                     = 0x10110025u;
constexpr uint32_t navspg_usrdat                        = 0x10110061u;
constexpr uint32_t navspg_usrdat_maja                   = 0x50110062u;
constexpr uint32_t navspg_usrdat_flat                   = 0x50110063u;
constexpr uint32_t navspg_usrdat_dx                     = 0x40110064u;
constexpr uint32_t navspg_usrdat_dy                     = 0x40110065u;
constexpr uint32_t navspg_usrdat_dz                     = 0x40110066u;
constexpr uint32_t navspg_usrdat_rotx                   = 0x40110067u;
constexpr uint32_t navspg_usrdat_roty                   = 0x40110068u;
constexpr uint32_t navspg_usrdat_rotz                   = 0x40110069u;
constexpr uint32_t navspg_usrdat_scale                  = 0x4011006au;
constexpr uint32_t navspg_infil_minsvs                  = 0x201100a1u;
constexpr uint32_t navspg_infil_maxsvs                  = 0x201100a2u;
constexpr uint32_t navspg_infil_mincno                  = 0x201100a3u;
constexpr uint32_t navspg_infil_minelev                 = 0x201100a4u;
constexpr uint32_t navspg_infil_ncnothrs                = 0x201100aau;
constexpr uint32_t navspg_infil_cnothrs                 = 0x201100abu;
constexpr uint32_t navspg_outfil_pdop                   = 0x301100b1u;
constexpr uint32_t navspg_outfil_tdop                   = 0x301100b2u;
constexpr uint32_t navspg_outfil_pacc                   = 0x301100b3u;
constexpr uint32_t navspg_outfil_tacc                   = 0x301100b4u;
constexpr uint32_t navspg_outfil_facc                   = 0x301100b5u;
constexpr uint32_t navspg_constr_alt                    = 0x401100c1u;
constexpr uint32_t navspg_constr_altvar                 = 0x401100c2u;
constexpr uint32_t navspg_constr_dgnssto                = 0x201100c4u;
constexpr uint32_t navspg_sigattcomp                    = 0x201100d6u;
constexpr uint32_t navspg_pl_ena                        = 0x101100d7u;

// NMEA
constexpr uint32_t nmea_protver                         = 0x20930001u;
constexpr uint32_t nmea_maxsvs                          = 0x20930002u;
constexpr uint32_t nmea_compat                          = 0x10930003u;
constexpr uint32_t nmea_consider                        = 0x10930004u;
constexpr uint32_t nmea_limit82                         = 0x10930005u;
constexpr uint32_t nmea_highprec                        = 0x10930006u;
constexpr uint32_t nmea_svnumbering                     = 0x20930007u;
constexpr uint32_t nmea_filt_gps                        = 0x10930011u;
constexpr uint32_t nmea_filt_sbas                       = 0x10930012u;
constexpr uint32_t nmea_filt_gal                        = 0x10930013u;
constexpr uint32_t nmea_filt_qzss                       = 0x10930015u;
constexpr uint32_t nmea_filt_glo                        = 0x10930016u;
constexpr uint32_t nmea_filt_bds                        = 0x10930017u;
constexpr uint32_t nmea_out_invfix                      = 0x10930021u;
constexpr uint32_t nmea_out_mskfix                      = 0x10930022u;
constexpr uint32_t nmea_out_invtime                     = 0x10930023u;
constexpr uint32_t nmea_out_invdate                     = 0x10930024u;
constexpr uint32_t nmea_out_onlygps                     = 0x10930025u;
constexpr uint32_t nmea_out_frozencog                   = 0x10930026u;
constexpr uint32_t nmea_maintalkerid                    = 0x20930031u;
constexpr uint32_t nmea_gsvtalkerid                     = 0x20930032u;
constexpr uint32_t nmea_bdstalkerid                     = 0x30930033u;

// ODO
constexpr uint32_t odo_use_odo                          = 0x10220001u;
constexpr uint32_t odo_use_cog                          = 0x10220002u;
constexpr uint32_t odo_outlpvel                         = 0x10220003u;
constexpr uint32_t odo_outlpcog                         = 0x10220004u;
constexpr uint32_t odo_profile                          = 0x20220005u;
constexpr uint32_t odo_cogmaxspeed                      = 0x20220021u;
constexpr uint32_t odo_cogmaxposacc                     = 0x20220022u;
constexpr uint32_t odo_vellpgain                        = 0x20220031u;
constexpr uint32_t odo_coglpgain                        = 0x20220032u;

// PM
constexpr uint32_t pm_operatemode                       = 0x20d00001u;
constexpr uint32_t pm_posupdateperiod                   = 0x40d00002u;
constexpr uint32_t pm_acqperiod                         = 0x40d00003u;
constexpr uint32_t pm_gridoffset                        = 0x40d00004u;
constexpr uint32_t pm_ontime                            = 0x30d00005u;
constexpr uint32_t pm_minfixacc                         = 0x40d00006u;
constexpr uint32_t pm_donotenterhot                     = 0x10d00007u;
constexpr uint32_t pm_waittimefix                       = 0x10d00008u;
constexpr uint32_t pm_updateeph                         = 0x10d0000au;
constexpr uint32_t pm_extintsel                         = 0x20d0000bu;
constexpr uint32_t pm_extintwake                        = 0x10d0000cu;
constexpr uint32_t pm_extintbackup                      = 0x10d0000du;
constexpr uint32_t pm_extintinactive                    = 0x10d0000eu;
constexpr uint32_t pm_extintinactivity                  = 0x40d0000fu;
constexpr uint32_t pm_limitpeakcurr                     = 0x10d00010u;

// PMP
constexpr uint32_t pmp_center_frequency                 = 0x40b10011u;
constexpr uint32_t pmp_search_window                    = 0x30b10012u;
constexpr uint32_t pmp_use_service_id                   = 0x10b10016u;
constexpr uint32_t pmp_service_id                       = 0x30b10017u;
constexpr uint32_t pmp_data_rate                        = 0x30b10013u;
constexpr uint32_t pmp_use_descrambler                  = 0x10b10014u;
constexpr uint32_t pmp_descrambler_init                 = 0x30b10015u;
constexpr uint32_t pmp_use_prescrambling                = 0x10b10019u;
constexpr uint32_t pmp_unique_word                      = 0x50b1001au;

// QZSS
constexpr uint32_t qzss_use_slas_dgnss                  = 0x10370005u;
constexpr uint32_t qzss_use_slas_testmode               = 0x10370006u;
constexpr uint32_t qzss_use_slas_raim_uncorr            = 0x10370007u;
constexpr uint32_t qzss_slas_max_baseline               = 0x30370008u;
constexpr uint32_t qzss_l6_svida                        = 0x20370020u;
constexpr uint32_t qzss_l6_svidb                        = 0x20370030u;
constexpr uint32_t qzss_l6_msga                         = 0x20370050u;
constexpr uint32_t qzss_l6_msgb                         = 0x20370060u;
constexpr uint32_t qzss_l6_rsdecoder                    = 0x20370080u;

// RATE
constexpr uint32_t rate_meas                            = 0x30210001u;
constexpr uint32_t rate_nav                             = 0x30210002u;
constexpr uint32_t rate_timeref                         = 0x20210003u;
constexpr uint32_t rate_nav_prio                        = 0x20210004u;

// RINV
constexpr uint32_t rinv_dump                            = 0x10c70001u;
constexpr uint32_t rinv_binary                          = 0x10c70002u;
constexpr uint32_t rinv_data_size                       = 0x20c70003u;
constexpr uint32_t rinv_chunk0                          = 0x50c70004u;
constexpr uint32_t rinv_chunk1                          = 0x50c70005u;
constexpr uint32_t rinv_chunk2                          = 0x50c70006u;
constexpr uint32_t rinv_chunk3                          = 0x50c70007u;

// RTCM
constexpr uint32_t rtcm_df003_out                       = 0x30090001u;
constexpr uint32_t rtcm_df003_in                        = 0x30090008u;
constexpr uint32_t rtcm_df003_in_filter                 = 0x20090009u;

// SBAS
constexpr uint32_t sbas_use_testmode                    = 0x10360002u;
constexpr uint32_t sbas_use_ranging                     = 0x10360003u;
constexpr uint32_t sbas_use_diffcorr                    = 0x10360004u;
constexpr uint32_t sbas_use_integrity                   = 0x10360005u;
constexpr uint32_t sbas_prnscanmask                     = 0x50360006u;

// SEC
constexpr uint32_t sec_cfg_lock                         = 0x10f60009u;
constexpr uint32_t sec_cfg_lock_unlockgrp1              = 0x30f6000au;
constexpr uint32_t sec_cfg_lock_unlockgrp2              = 0x30f6000bu;
constexpr uint32_t sec_jamdet_sensitivity_hi            = 0x10f60051u;

// SFCORE
constexpr uint32_t sfcore_use_sf                        = 0x10080001u;
constexpr uint32_t sfcore_imu2crp_la_x                  = 0x30080002u;
constexpr uint32_t sfcore_imu2crp_la_y                  = 0x30080003u;
constexpr uint32_t sfcore_imu2crp_la_z                  = 0x30080004u;
constexpr uint32_t sfcore_sen_ttag_fact                 = 0x40080006u;
constexpr uint32_t sfcore_sen_ttag_max                  = 0x40080007u;
constexpr uint32_t sfcore_outfil_pacc                   = 0x3008000fu;

// SFIMU
constexpr uint32_t sfimu_gyro_tc_update_period          = 0x30060007u;
constexpr uint32_t sfimu_gyro_rmsthdl                   = 0x20060008u;
constexpr uint32_t sfimu_gyro_frequency                 = 0x20060009u;
constexpr uint32_t sfimu_gyro_latency                   = 0x3006000au;
constexpr uint32_t sfimu_gyro_accuracy                  = 0x3006000bu;
constexpr uint32_t sfimu_accel_rmsthdl                  = 0x20060015u;
constexpr uint32_t sfimu_accel_frequency                = 0x20060016u;
constexpr uint32_t sfimu_accel_latency                  = 0x30060017u;
constexpr uint32_t sfimu_accel_accuracy                 = 0x30060018u;
constexpr uint32_t sfimu_imu_en                         = 0x1006001du;
constexpr uint32_t sfimu_imu_i2c_scl_pio                = 0x2006001eu;
constexpr uint32_t sfimu_imu_i2c_sda_pio                = 0x2006001fu;
constexpr uint32_t sfimu_imu2ant_la_x                   = 0x30060020u;
constexpr uint32_t sfimu_imu2ant_la_y                   = 0x30060021u;
constexpr uint32_t sfimu_imu2ant_la_z                   = 0x30060022u;
constexpr uint32_t sfimu_auto_mntalg_ena                = 0x10060027u;
constexpr uint32_t sfimu_imu_mntalg_yaw                 = 0x4006002du;
constexpr uint32_t sfimu_imu_mntalg_pitch               = 0x3006002eu;
constexpr uint32_t sfimu_imu_mntalg_roll                = 0x3006002fu;

// SFODO
constexpr uint32_t sfodo_combine_ticks                  = 0x10070001u;
constexpr uint32_t sfodo_use_speed                      = 0x10070003u;
constexpr uint32_t sfodo_dis_autocountmax               = 0x10070004u;
constexpr uint32_t sfodo_dis_autodirpinpol              = 0x10070005u;
constexpr uint32_t sfodo_dis_autospeed                  = 0x10070006u;
constexpr uint32_t sfodo_factor                         = 0x40070007u;
constexpr uint32_t sfodo_quant_error                    = 0x40070008u;
constexpr uint32_t sfodo_count_max                      = 0x40070009u;
constexpr uint32_t sfodo_latency                        = 0x3007000au;
constexpr uint32_t sfodo_frequency                      = 0x2007000bu;
constexpr uint32_t sfodo_cnt_both_edges                 = 0x1007000du;
constexpr uint32_t sfodo_speed_band                     = 0x3007000eu;
constexpr uint32_t sfodo_use_wt_pin                     = 0x1007000fu;
constexpr uint32_t sfodo_dir_pinpol                     = 0x10070010u;
constexpr uint32_t sfodo_dis_autosw                     = 0x10070011u;
constexpr uint32_t sfodo_imu2vrp_la_x                   = 0x30070012u;
constexpr uint32_t sfodo_imu2vrp_la_y                   = 0x30070013u;
constexpr uint32_t sfodo_imu2vrp_la_z                   = 0x30070014u;
constexpr uint32_t sfodo_trackgauge_rear                = 0x30070018u;
constexpr uint32_t sfodo_dis_dir_info                   = 0x1007001cu;

// SIGNAL
constexpr uint32_t signal_gps_ena                       = 0x1031001fu;
constexpr uint32_t signal_gps_l1ca_ena                  = 0x10310001u;
constexpr uint32_t signal_gps_l2c_ena                   = 0x10310003u;
constexpr uint32_t signal_gps_l5_ena                    = 0x10310004u;
constexpr uint32_t signal_sbas_ena                      = 0x10310020u;
constexpr uint32_t signal_sbas_l1ca_ena                 = 0x10310005u;
constexpr uint32_t signal_gal_ena                       = 0x10310021u;
constexpr uint32_t signal_gal_e1_ena                    = 0x10310007u;
constexpr uint32_t signal_gal_e5a_ena                   = 0x10310009u;
constexpr uint32_t signal_gal_e5b_ena                   = 0x1031000au;
constexpr uint32_t signal_bds_ena                       = 0x10310022u;
constexpr uint32_t signal_bds_b1_ena                    = 0x1031000du;
constexpr uint32_t signal_bds_b2_ena                    = 0x1031000eu;
constexpr uint32_t signal_bds_b1c_ena                   = 0x1031000fu;
constexpr uint32_t signal_bds_b2a_ena                   = 0x10310028u;
constexpr uint32_t signal_qzss_ena                      = 0x10310024u;
constexpr uint32_t signal_qzss_l1ca_ena                 = 0x10310012u;
constexpr uint32_t signal_qzss_l1s_ena                  = 0x10310014u;
constexpr uint32_t signal_qzss_l2c_ena                  = 0x10310015u;
constexpr uint32_t signal_qzss_l5_ena                   = 0x10310017u;
constexpr uint32_t signal_glo_ena                       = 0x10310025u;
constexpr uint32_t signal_glo_l1_ena                    = 0x10310018u;
constexpr uint32_t signal_glo_l2_ena                    = 0x1031001au;
constexpr uint32_t signal_navic_ena                     = 0x10310026u;
constexpr uint32_t signal_navic_l5_ena                  = 0x1031001du;

// SPARTN
constexpr uint32_t spartn_use_source                    = 0x20a70001u;

// SPI
constexpr uint32_t spi_maxff                            = 0x20640001u;
constexpr uint32_t spi_cpolarity                        = 0x10640002u;
constexpr uint32_t spi_cphase                           = 0x10640003u;
constexpr uint32_t spi_extendedtimeout                  = 0x10640005u;
constexpr uint32_t spi_enabled                          = 0x10640006u;

// SPIINPROT
constexpr uint32_t spiinprot_ubx                        = 0x10790001u;
constexpr uint32_t spiinprot_nmea                       = 0x10790002u;
constexpr uint32_t spiinprot_rtcm3x                     = 0x10790004u;
constexpr uint32_t spiinprot_spartn                     = 0x10790005u;

// SPIOUTPROT
constexpr uint32_t spioutprot_ubx                       = 0x107a0001u;
constexpr uint32_t spioutprot_nmea                      = 0x107a0002u;
constexpr uint32_t spioutprot_rtcm3x                    = 0x107a0004u;

// TMODE
constexpr uint32_t tmode_mode                           = 0x20030001u;
constexpr uint32_t tmode_pos_type                       = 0x20030002u;
constexpr uint32_t tmode_ecef_x                         = 0x40030003u;
constexpr uint32_t tmode_ecef_y                         = 0x40030004u;
constexpr uint32_t tmode_ecef_z                         = 0x40030005u;
constexpr uint32_t tmode_ecef_x_hp                      = 0x20030006u;
constexpr uint32_t tmode_ecef_y_hp                      = 0x20030007u;
constexpr uint32_t tmode_ecef_z_hp                      = 0x20030008u;
constexpr uint32_t tmode_lat                            = 0x40030009u;
constexpr uint32_t tmode_lon                            = 0x4003000au;
constexpr uint32_t tmode_height                         = 0x4003000bu;
constexpr uint32_t tmode_lat_hp                         = 0x2003000cu;
constexpr uint32_t tmode_lon_hp                         = 0x2003000du;
constexpr uint32_t tmode_height_hp                      = 0x2003000eu;
constexpr uint32_t tmode_fixed_pos_acc                  = 0x4003000fu;
constexpr uint32_t tmode_svin_min_dur                   = 0x40030010u;
constexpr uint32_t tmode_svin_acc_limit                 = 0x40030011u;

// TP
constexpr uint32_t tp_pulse_def                         = 0x20050023u;
constexpr uint32_t tp_pulse_length_def                  = 0x20050030u;
constexpr uint32_t tp_ant_cabledelay                    = 0x30050001u;
constexpr uint32_t tp_period_tp1                        = 0x40050002u;
constexpr uint32_t tp_period_lock_tp1                   = 0x40050003u;
constexpr uint32_t tp_freq_tp1                          = 0x40050024u;
constexpr uint32_t tp_freq_lock_tp1                     = 0x40050025u;
constexpr uint32_t tp_len_tp1                           = 0x40050004u;
constexpr uint32_t tp_len_lock_tp1                      = 0x40050005u;
constexpr uint32_t tp_duty_tp1                          = 0x5005002au;
constexpr uint32_t tp_duty_lock_tp1                     = 0x5005002bu;
constexpr uint32_t tp_user_delay_tp1                    = 0x40050006u;
constexpr uint32_t tp_tp1_ena                           = 0x10050007u;
constexpr uint32_t tp_sync_gnss_tp1                     = 0x10050008u;
constexpr uint32_t tp_use_locked_tp1                    = 0x10050009u;
constexpr uint32_t tp_align_to_tow_tp1                  = 0x1005000au;
constexpr uint32_t tp_pol_tp1                           = 0x1005000bu;
constexpr uint32_t tp_timegrid_tp1                      = 0x2005000cu;
constexpr uint32_t tp_drstr_tp1                         = 0x20050035u;
constexpr uint32_t tp_period_tp2                        = 0x4005000du;
constexpr uint32_t tp_period_lock_tp2                   = 0x4005000eu;
constexpr uint32_t tp_freq_tp2                          = 0x40050026u;
constexpr uint32_t tp_freq_lock_tp2                     = 0x40050027u;
constexpr uint32_t tp_len_tp2                           = 0x4005000fu;
constexpr uint32_t tp_len_lock_tp2                      = 0x40050010u;
constexpr uint32_t tp_duty_tp2                          = 0x5005002cu;
constexpr uint32_t tp_duty_lock_tp2                     = 0x5005002du;
constexpr uint32_t tp_user_delay_tp2                    = 0x40050011u;
constexpr uint32_t tp_tp2_ena                           = 0x10050012u;
constexpr uint32_t tp_sync_gnss_tp2                     = 0x10050013u;
constexpr uint32_t tp_use_locked_tp2                    = 0x10050014u;
constexpr uint32_t tp_align_to_tow_tp2                  = 0x10050015u;
constexpr uint32_t tp_pol_tp2                           = 0x10050016u;
constexpr uint32_t tp_timegrid_tp2                      = 0x20050017u;
constexpr uint32_t tp_drstr_tp2                         = 0x20050036u;

// TXREADY
constexpr uint32_t txready_enabled                      = 0x10a20001u;
constexpr uint32_t txready_polarity                     = 0x10a20002u;
constexpr uint32_t txready_pin                          = 0x20a20003u;
constexpr uint32_t txready_threshold                    = 0x30a20004u;
constexpr uint32_t txready_interface                    = 0x20a20005u;

// UART1
constexpr uint32_t uart1_baudrate                       = 0x40520001u;
constexpr uint32_t uart1_stopbits                       = 0x20520002u;
constexpr uint32_t uart1_databits                       = 0x20520003u;
constexpr uint32_t uart1_parity                         = 0x20520004u;
constexpr uint32_t uart1_enabled                        = 0x10520005u;

// UART1INPROT
constexpr uint32_t uart1inprot_ubx                      = 0x10730001u;
constexpr uint32_t uart1inprot_nmea                     = 0x10730002u;
constexpr uint32_t uart1inprot_rtcm3x                   = 0x10730004u;
constexpr uint32_t uart1inprot_spartn                   = 0x10730005u;

// UART1OUTPROT
constexpr uint32_t uart1outprot_ubx                     = 0x10740001u;
constexpr uint32_t uart1outprot_nmea                    = 0x10740002u;
constexpr uint32_t uart1outprot_rtcm3x                  = 0x10740004u;

// UART2
constexpr uint32_t uart2_baudrate                       = 0x40530001u;
constexpr uint32_t uart2_stopbits                       = 0x20530002u;
constexpr uint32_t uart2_databits                       = 0x20530003u;
constexpr uint32_t uart2_parity                         = 0x20530004u;
constexpr uint32_t uart2_enabled                        = 0x10530005u;

// UART2INPROT
constexpr uint32_t uart2inprot_ubx                      = 0x10750001u;
constexpr uint32_t uart2inprot_nmea                     = 0x10750002u;
constexpr uint32_t uart2inprot_rtcm3x                   = 0x10750004u;
constexpr uint32_t uart2inprot_spartn                   = 0x10750005u;

// UART2OUTPROT
constexpr uint32_t uart2outprot_ubx                     = 0x10760001u;
constexpr uint32_t uart2outprot_nmea                    = 0x10760002u;
constexpr uint32_t uart2outprot_rtcm3x                  = 0x10760004u;

// USB
constexpr uint32_t usb_enabled                          = 0x10650001u;
constexpr uint32_t usb_selfpow                          = 0x10650002u;
constexpr uint32_t usb_vendor_id                        = 0x3065000au;
constexpr uint32_t usb_product_id                       = 0x3065000bu;
constexpr uint32_t usb_power                            = 0x3065000cu;
constexpr uint32_t usb_vendor_str0                      = 0x5065000du;
constexpr uint32_t usb_vendor_str1                      = 0x5065000eu;
constexpr uint32_t usb_vendor_str2                      = 0x5065000fu;
constexpr uint32_t usb_vendor_str3                      = 0x50650010u;
constexpr uint32_t usb_product_str0                     = 0x50650011u;
constexpr uint32_t usb_product_str1                     = 0x50650012u;
constexpr uint32_t usb_product_str2                     = 0x50650013u;
constexpr uint32_t usb_product_str3                     = 0x50650014u;
constexpr uint32_t usb_serial_no_str0                   = 0x50650015u;
constexpr uint32_t usb_serial_no_str1                   = 0x50650016u;
constexpr uint32_t usb_serial_no_str2                   = 0x50650017u;
constexpr uint32_t usb_serial_no_str3                   = 0x50650018u;

// USBINPROT
constexpr uint32_t usbinprot_ubx                        = 0x10770001u;
constexpr uint32_t usbinprot_nmea                       = 0x10770002u;
constexpr uint32_t usbinprot_rtcm3x                     = 0x10770004u;
constexpr uint32_t usbinprot_spartn                     = 0x10770005u;

// USBOUTPROT
constexpr uint32_t usboutprot_ubx                       = 0x10780001u;
constexpr uint32_t usboutprot_nmea                      = 0x10780002u;
constexpr uint32_t usboutprot_rtcm3x                    = 0x10780004u;

} // namespace cfg_key
} // namespace config
} // namespace ubx
