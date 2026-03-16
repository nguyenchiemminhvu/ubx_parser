// src/database/internal/db_decoder_stub.h
//
// A generic ubx_parser decoder stub that:
//   1. Decodes the UBX frame into a typed message struct using the real decoder.
//   2. Calls handler.update(msg).
//   3. Calls db.apply_update(handler).
//
// One stub template is instantiated per message type.  This avoids duplicating
// boiler-plate for each new message.

#pragma once

#include "decoders/i_message_decoder.h"
#include "database/ubx_database.h"
#include <functional>
#include <memory>

namespace ubx
{
namespace database
{
namespace internal
{

// ─── db_decoder_stub ─────────────────────────────────────────────────────────
//
// Template parameters:
//   Decoder  – the real ubx_parser decoder (e.g. nav_pvt_decoder).
//              Must accept a callback of type Callback in its constructor.
//   Handler  – the i_msg_update_handler subclass (e.g. nav_pvt_handler).
//              Must have a member: void update(const Msg&).
//   Msg      – the decoded message struct (e.g. ubx_nav_pvt).

template <typename Decoder, typename Handler, typename Msg>
class db_decoder_stub : public parser::i_message_decoder
{
public:
    db_decoder_stub(Handler* handler, ubx_database* db)
        : handler_(handler)
        , db_(db)
        , decoder_(
            [this](const Msg& msg) {
                handler_->update(msg);
                db_->apply_update(*handler_);
            })
    {
    }

    uint8_t msg_class() const override { return decoder_.msg_class(); }
    uint8_t msg_id()    const override { return decoder_.msg_id();    }

    bool decode(const parser::ubx_frame& frame) override
    {
        return decoder_.decode(frame);
    }

private:
    Handler*     handler_;
    ubx_database* db_;
    Decoder      decoder_;
};

} // namespace internal
} // namespace database
} // namespace ubx
