// ubx_database.h
//
// Core database: owns the field store, the message mask, and the commit policy.
//
// Responsibilities
// ─────────────────
//   1. Receive calls from message update handlers (via apply_update).
//   2. Write fields into ubx_field_store under an exclusive lock.
//   3. Consult the commit policy after each message.
//   4. On commit: snapshot the store and invoke the commit callback registered
//      for that specific commit_kind (high_priority or low_priority).
//
// Commit callback registration
// ────────────────────────────
//   Clients register per-kind callbacks:
//     db.set_commit_callback(commit_kind::high_priority, hp_fn);
//     db.set_commit_callback(commit_kind::low_priority,  lp_fn);
//
//   The snapshot passed to the callback always carries its commit_kind so
//   the receiver can also branch on snap.kind() if needed.
//
// Thread-safety
// ─────────────
//   • apply_update() is called from the parser thread (write path).
//     It acquires the exclusive lock for the duration of the handler call and
//     the commit decision.
//   • snapshot() is called from any consumer thread (read path) and acquires
//     the shared lock.
//   • std::shared_timed_mutex (C++14) is used so concurrent reads are cheap.
//
// Extension
// ─────────
//   No subclassing needed.  The per-kind commit callbacks and the policy
//   object are the only extension points.

#pragma once

#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/i_msg_update_handler.h"
#include "database/i_commit_policy.h"
#include "database/ubx_location_snapshot.h"
#include "database/epoch_commit_policy.h"

#include <functional>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

namespace ubx
{
namespace database
{

/// Commit callback signature:  void(const ubx_location_snapshot&)
using commit_callback = std::function<void(const ubx_location_snapshot&)>;

// ─── ubx_database ─────────────────────────────────────────────────────────────

class ubx_database
{
public:
    /// Construct with a commit policy.  If nullptr, epoch_commit_policy with
    /// default HP/LP triggers (NAV-PVT / NAV-SAT) is used.
    explicit ubx_database(std::unique_ptr<i_commit_policy> policy = nullptr);

    ~ubx_database() = default;

    // Non-copyable (owns mutex and unique_ptr)
    ubx_database(const ubx_database&)            = delete;
    ubx_database& operator=(const ubx_database&) = delete;

    // ── Configuration (call before first apply_update) ────────────────────────

    /// Replace the commit policy.
    void set_commit_policy(std::unique_ptr<i_commit_policy> policy);

    /// Register a callback for a specific commit kind.
    ///
    /// @param kind  commit_kind::high_priority or commit_kind::low_priority.
    ///              Passing commit_kind::none is a no-op.
    /// @param cb    Callback invoked synchronously from apply_update() when the
    ///              policy returns @p kind.  Pass a null std::function to clear.
    void set_commit_callback(commit_kind kind, commit_callback cb);

    // ── Write path (parser thread) ────────────────────────────────────────────

    /// Drive the update handler, then check the commit policy.
    ///
    /// Flow:
    ///   1. Acquire exclusive lock.
    ///   2. handler.handle(store_, mask_)  → writes fields + sets mask bit.
    ///   3. policy.should_commit(msg)      → returns commit_kind.
    ///   4. If not none: snapshot store, call the callback registered for that
    ///      kind, then reset the message mask.
    ///
    /// @param handler  The update handler for this message type.
    void apply_update(i_msg_update_handler& handler);

    // ── Read path (any thread) ────────────────────────────────────────────────

    /// Take a consistent snapshot of the current store under the shared lock.
    ubx_location_snapshot snapshot(commit_kind kind = commit_kind::none) const;

    // ── Housekeeping ──────────────────────────────────────────────────────────

    void reset();

private:
    void do_commit(commit_kind kind);

    mutable std::shared_timed_mutex  rw_mutex_;
    ubx_field_store                  store_;
    ubx_msg_mask                     mask_;
    std::unique_ptr<i_commit_policy> policy_;

    // Per-kind callbacks, keyed by commit_kind.
    std::unordered_map<commit_kind, commit_callback> commit_callbacks_;
};

} // namespace database
} // namespace ubx
