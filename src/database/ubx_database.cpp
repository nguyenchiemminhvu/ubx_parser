// src/database/ubx_database.cpp

#include "database/ubx_database.h"
#include <utility>
#include <mutex>

namespace ubx
{
namespace database
{

ubx_database::ubx_database(std::unique_ptr<i_commit_policy> policy)
    : policy_(policy
              ? std::move(policy)
              : std::unique_ptr<i_commit_policy>(new epoch_commit_policy()))
{
}

void ubx_database::set_commit_policy(std::unique_ptr<i_commit_policy> policy)
{
    std::unique_lock<std::shared_timed_mutex> lock(rw_mutex_);
    policy_ = std::move(policy);
}

void ubx_database::set_commit_callback(commit_kind kind, commit_callback cb)
{
    if (kind == commit_kind::none)
        return; // no-op
    std::unique_lock<std::shared_timed_mutex> lock(rw_mutex_);
    if (cb)
        commit_callbacks_[kind] = std::move(cb);
    else
        commit_callbacks_.erase(kind);
}

void ubx_database::apply_update(i_msg_update_handler& handler)
{
    std::unique_lock<std::shared_timed_mutex> lock(rw_mutex_);
    handler.handle(store_, mask_);
    const commit_kind kind = policy_->should_commit(handler.handled_msg());
    if (kind != commit_kind::none)
    {
        do_commit(kind);   // snapshot + fire per-kind callback while lock held
    }
}

ubx_location_snapshot ubx_database::snapshot(commit_kind kind) const
{
    std::shared_lock<std::shared_timed_mutex> lock(rw_mutex_);
    return ubx_location_snapshot(store_, mask_.load(), kind);
}

void ubx_database::reset()
{
    std::unique_lock<std::shared_timed_mutex> lock(rw_mutex_);
    store_.reset();
    mask_.reset();
}

// ── private ───────────────────────────────────────────────────────────────────

void ubx_database::do_commit(commit_kind kind)
{
    // Called while the exclusive lock is already held.
    auto it = commit_callbacks_.find(kind);
    if (it == commit_callbacks_.end() || !it->second)
    {
        mask_.reset();
        return;
    }

    ubx_location_snapshot snap(store_, mask_.load(), kind);

    // Clear the mask after snapshotting so the next epoch starts fresh.
    mask_.reset();

    // Fire the kind-specific callback.  Consumers must NOT call apply_update()
    // from within the callback (would deadlock).
    it->second(snap);
}

} // namespace database
} // namespace ubx
