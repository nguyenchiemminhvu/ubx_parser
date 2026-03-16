// ubx_database_adapter.h
//
// Wires ubx_parser callbacks → message update handlers → ubx_database.
//
//   1. Holds a shared_ptr to ubx_database.
//   2. Creates and owns one message update handler per registered UBX message.
//   3. Exposes a single method – register_with_parser(ubx_decoder_registry&) –
//      that installs lightweight ubx_parser decoder stubs whose sole job is to
//      update the handler's cached message and call db.apply_update().
//
// Thread-safety:
//   register_with_parser() must be called before the first parser feed().
//   After that, the adapter state is read-only; apply_update() is
//   thread-safe via ubx_database's internal mutex.
//
// Extensibility (OCP):
//   To add a new message:
//     1. Implement i_msg_update_handler for the new message type.
//     2. Call adapter.add_handler(std::unique_ptr<new_handler>(...)).
//     3. Call register_with_parser() once (or call it again after adding).
//   No changes to ubx_database or any existing handler are needed.

#pragma once

#include "database/ubx_database.h"
#include "database/i_msg_update_handler.h"
#include "ubx_decoder_registry.h"

#include <memory>
#include <vector>

namespace ubx
{
namespace database
{

class ubx_database_adapter
{
public:
    explicit ubx_database_adapter(std::shared_ptr<ubx_database> db);
    ~ubx_database_adapter() = default;

    // Non-copyable
    ubx_database_adapter(const ubx_database_adapter&)            = delete;
    ubx_database_adapter& operator=(const ubx_database_adapter&) = delete;

    // ── Handler registration ──────────────────────────────────────────────────

    /// Register a message update handler.  Must be called before
    /// register_with_parser().
    void add_handler(std::unique_ptr<i_msg_update_handler> handler);

    // ── Parser wiring ─────────────────────────────────────────────────────────

    /// Install decoder stubs into the registry for every registered handler.
    ///
    /// Each stub captures a raw pointer to the handler and the database and
    /// calls db_->apply_update(handler) when a matching UBX frame arrives.
    ///
    /// @param registry  The ubx_decoder_registry that owns the decoder lifetime.
    void register_with_parser(parser::ubx_decoder_registry& registry);

private:
    std::shared_ptr<ubx_database>                  db_;
    std::vector<std::unique_ptr<i_msg_update_handler>> handlers_;
};

} // namespace database
} // namespace ubx
