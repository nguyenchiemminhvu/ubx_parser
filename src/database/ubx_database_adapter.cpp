// src/database/ubx_database_adapter.cpp

#include "database/ubx_database_adapter.h"
#include "database/ubx_data_fields.h"
#include "database/handlers/db_nav_pvt_handler.h"
#include "database/handlers/db_nav_sat_handler.h"
#include "database/handlers/db_nav_dop_handler.h"
#include "database/handlers/db_nav_eell_handler.h"
#include "database/handlers/db_nav_timegps_handler.h"
#include "database/handlers/db_nav_status_handler.h"
#include "database/handlers/db_tim_tp_handler.h"
#include "database/handlers/db_nav2_pvt_handler.h"
#include "database/handlers/db_nav2_eell_handler.h"
#include "database/handlers/db_nav2_dop_handler.h"
#include "database/handlers/db_nav2_timegps_handler.h"
#include "database/handlers/db_nav_odo_handler.h"
#include "database/handlers/db_esf_ins_handler.h"
#include "database/handlers/db_esf_status_handler.h"
#include "database/handlers/db_esf_meas_handler.h"
#include "database/handlers/db_nav_sig_handler.h"
#include "database/handlers/db_nav_timeutc_handler.h"
#include "database/handlers/db_nav_clock_handler.h"
#include "database/handlers/db_nav_att_handler.h"
#include "database/handlers/db_rxm_measx_handler.h"
#include "database/handlers/db_mon_io_handler.h"
#include "database/handlers/db_mon_txbuf_handler.h"
#include "database/handlers/db_sec_sig_handler.h"

#include "database/internal/db_decoder_stub.h"

#include "decoders/nav_pvt_decoder.h"
#include "decoders/nav_sat_decoder.h"
#include "decoders/nav_dop_decoder.h"
#include "decoders/nav_eell_decoder.h"
#include "decoders/nav_timegps_decoder.h"
#include "decoders/nav_status_decoder.h"
#include "decoders/tim_tp_decoder.h"
#include "decoders/nav2_pvt_decoder.h"
#include "decoders/nav2_eell_decoder.h"
#include "decoders/nav2_dop_decoder.h"
#include "decoders/nav2_timegps_decoder.h"
#include "decoders/nav_odo_decoder.h"
#include "decoders/esf_ins_decoder.h"
#include "decoders/esf_status_decoder.h"
#include "decoders/esf_meas_decoder.h"
#include "decoders/nav_sig_decoder.h"
#include "decoders/nav_timeutc_decoder.h"
#include "decoders/nav_clock_decoder.h"
#include "decoders/nav_att_decoder.h"
#include "decoders/rxm_measx_decoder.h"
#include "decoders/mon_io_decoder.h"
#include "decoders/mon_txbuf_decoder.h"
#include "decoders/sec_sig_decoder.h"

#include <utility>

namespace ubx
{
namespace database
{

ubx_database_adapter::ubx_database_adapter(std::shared_ptr<ubx_database> db)
    : db_(std::move(db))
{
}

void ubx_database_adapter::add_handler(std::unique_ptr<i_msg_update_handler> handler)
{
    handlers_.push_back(std::move(handler));
}

void ubx_database_adapter::register_with_parser(parser::ubx_decoder_registry& registry)
{
    for (auto& h : handlers_)
    {
        using namespace internal;
        msg_type mt = h->handled_msg();

        switch (mt)
        {
        case MSG_UBX_NAV_PVT:
        {
            auto* typed = static_cast<db_nav_pvt_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_pvt_decoder,
                                        db_nav_pvt_handler,
                                        parser::ubx_nav_pvt>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_SAT:
        {
            auto* typed = static_cast<db_nav_sat_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_sat_decoder,
                                        db_nav_sat_handler,
                                        parser::ubx_nav_sat>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_DOP:
        {
            auto* typed = static_cast<db_nav_dop_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_dop_decoder,
                                        db_nav_dop_handler,
                                        parser::ubx_nav_dop>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_EELL:
        {
            auto* typed = static_cast<db_nav_eell_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_eell_decoder,
                                        db_nav_eell_handler,
                                        parser::ubx_nav_eell>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_TIMEGPS:
        {
            auto* typed = static_cast<db_nav_timegps_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_timegps_decoder,
                                        db_nav_timegps_handler,
                                        parser::ubx_nav_timegps>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_STATUS:
        {
            auto* typed = static_cast<db_nav_status_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_status_decoder,
                                        db_nav_status_handler,
                                        parser::ubx_nav_status>(typed, db_.get())));
            break;
        }
        case MSG_UBX_TIM_TP:
        {
            auto* typed = static_cast<db_tim_tp_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::tim_tp_decoder,
                                        db_tim_tp_handler,
                                        parser::ubx_tim_tp>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV2_PVT:
        {
            auto* typed = static_cast<db_nav2_pvt_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav2_pvt_decoder,
                                        db_nav2_pvt_handler,
                                        parser::ubx_nav2_pvt>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV2_EELL:
        {
            auto* typed = static_cast<db_nav2_eell_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav2_eell_decoder,
                                        db_nav2_eell_handler,
                                        parser::ubx_nav2_eell>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV2_DOP:
        {
            auto* typed = static_cast<db_nav2_dop_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav2_dop_decoder,
                                        db_nav2_dop_handler,
                                        parser::ubx_nav2_dop>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV2_TIMEGPS:
        {
            auto* typed = static_cast<db_nav2_timegps_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav2_timegps_decoder,
                                        db_nav2_timegps_handler,
                                        parser::ubx_nav2_timegps>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_ODO:
        {
            auto* typed = static_cast<db_nav_odo_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_odo_decoder,
                                        db_nav_odo_handler,
                                        parser::ubx_nav_odo>(typed, db_.get())));
            break;
        }
        case MSG_UBX_ESF_INS:
        {
            auto* typed = static_cast<db_esf_ins_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::esf_ins_decoder,
                                        db_esf_ins_handler,
                                        parser::ubx_esf_ins>(typed, db_.get())));
            break;
        }
        case MSG_UBX_ESF_STATUS:
        {
            auto* typed = static_cast<db_esf_status_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::esf_status_decoder,
                                        db_esf_status_handler,
                                        parser::ubx_esf_status>(typed, db_.get())));
            break;
        }
        case MSG_UBX_ESF_MEAS:
        {
            auto* typed = static_cast<db_esf_meas_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::esf_meas_decoder,
                                        db_esf_meas_handler,
                                        parser::ubx_esf_meas>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_SIG:
        {
            auto* typed = static_cast<db_nav_sig_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_sig_decoder,
                                        db_nav_sig_handler,
                                        parser::ubx_nav_sig>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_TIMEUTC:
        {
            auto* typed = static_cast<db_nav_timeutc_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_timeutc_decoder,
                                        db_nav_timeutc_handler,
                                        parser::ubx_nav_timeutc>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_CLOCK:
        {
            auto* typed = static_cast<db_nav_clock_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_clock_decoder,
                                        db_nav_clock_handler,
                                        parser::ubx_nav_clock>(typed, db_.get())));
            break;
        }
        case MSG_UBX_NAV_ATT:
        {
            auto* typed = static_cast<db_nav_att_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::nav_att_decoder,
                                        db_nav_att_handler,
                                        parser::ubx_nav_att>(typed, db_.get())));
            break;
        }
        case MSG_UBX_RXM_MEASX:
        {
            auto* typed = static_cast<db_rxm_measx_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::rxm_measx_decoder,
                                        db_rxm_measx_handler,
                                        parser::ubx_rxm_measx>(typed, db_.get())));
            break;
        }
        case MSG_UBX_MON_IO:
        {
            auto* typed = static_cast<db_mon_io_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::mon_io_decoder,
                                        db_mon_io_handler,
                                        parser::ubx_mon_io>(typed, db_.get())));
            break;
        }
        case MSG_UBX_MON_TXBUF:
        {
            auto* typed = static_cast<db_mon_txbuf_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::mon_txbuf_decoder,
                                        db_mon_txbuf_handler,
                                        parser::ubx_mon_txbuf>(typed, db_.get())));
            break;
        }
        case MSG_UBX_SEC_SIG:
        {
            auto* typed = static_cast<db_sec_sig_handler*>(h.get());
            registry.register_decoder(
                std::unique_ptr<parser::i_message_decoder>(
                    new db_decoder_stub<parser::sec_sig_decoder,
                                        db_sec_sig_handler,
                                        parser::ubx_sec_sig>(typed, db_.get())));
            break;
        }
        default:
            // Handler type not wired to a decoder stub yet; skip.
            break;
        }
    }
}

} // namespace database
} // namespace ubx

