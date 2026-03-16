// i_msg_update_handler.h
//
// Extension point: one handler per UBX message type.
//
// Design (Open/Closed Principle):
//   To add a new UBX message to the database layer:
//     1. Implement i_msg_update_handler<Msg>.
//     2. Register it with ubx_database_adapter.
//   No changes to ubx_database or ubx_database_adapter are needed.
//
// Thread-safety:
//   handle() is called while the parser thread holds the write lock on
//   ubx_database.  Implementations must be fast (no I/O, no sleeping).

#pragma once

#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

// Forward-declare the store so handlers do not have to include the full store.
class ubx_field_store;
class ubx_msg_mask;

// ─── i_msg_update_handler ─────────────────────────────────────────────────────
//
// Pure interface for "write parsed message fields into the field store".

class i_msg_update_handler
{
public:
    virtual ~i_msg_update_handler() = default;

    i_msg_update_handler()                                         = default;
    i_msg_update_handler(const i_msg_update_handler&)              = delete;
    i_msg_update_handler& operator=(const i_msg_update_handler&)   = delete;

    /// Which msg_type does this handler serve?
    virtual msg_type handled_msg() const = 0;

    /// Write decoded fields into @p store and set the mask bit in @p mask.
    ///
    /// Called while the exclusive write-lock on ubx_database is held.
    /// Must not block.
    virtual void handle(ubx_field_store& store, ubx_msg_mask& mask) = 0;
};

} // namespace database
} // namespace ubx
