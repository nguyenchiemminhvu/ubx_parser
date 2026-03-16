// i_commit_policy.h
//
// Abstraction for the HP / LP epoch-commit decision.
//
// The old CDatabase::afterProcessing() decided whether to call Commit(true) or
// Commit(false) based on two configured epoch-trigger messages (m_epochHP and
// m_epochLP).  This interface decouples that decision from the store itself.
//
// Design:
//   • epoch_commit_policy — default implementation, mirrors old CDatabase logic.
//   • Any alternative scheduling logic (EOE-based, timer-based, etc.) can be
//     injected without touching ubx_database.
//
// Thread-safety:  should_commit() is called under the write lock; implementations
//                 must be fast and non-blocking.

#pragma once

#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

// ─── commit_kind ─────────────────────────────────────────────────────────────

enum class commit_kind
{
    high_priority,
    low_priority,
    none,
};

// ─── i_commit_policy ─────────────────────────────────────────────────────────

class i_commit_policy
{
public:
    virtual ~i_commit_policy() = default;

    i_commit_policy()                                      = default;
    i_commit_policy(const i_commit_policy&)                = delete;
    i_commit_policy& operator=(const i_commit_policy&)     = delete;

    /// Called after a message has been written into the field store.
    /// Returns the commit kind that should be performed (may be none).
    virtual commit_kind should_commit(msg_type msg) const = 0;
};

} // namespace database
} // namespace ubx
