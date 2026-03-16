// ubx_location_snapshot.h
//
// Thread-safe, immutable snapshot of the current database field store.
//
// ubx_location_snapshot captures the field store under the shared (read) lock
// and exposes a const API for consumers on other threads.  Consumers never
// touch the live store directly.
//
// Thread-safety: the snapshot itself is immutable after construction; its const
//                getters are safe for any number of concurrent readers.

#pragma once

#include "database/ubx_field_store.h"
#include "database/ubx_data_fields.h"
#include "database/i_commit_policy.h"

namespace ubx
{
namespace database
{

class ubx_location_snapshot
{
public:
    /// Shallow-copy the store (called while caller holds the shared or
    /// exclusive lock on the live store).
    explicit ubx_location_snapshot(const ubx_field_store& store,
                                   uint64_t               msg_mask_snapshot,
                                   commit_kind            kind);

    ubx_location_snapshot()                                       = default;
    ubx_location_snapshot(const ubx_location_snapshot&)          = default;
    ubx_location_snapshot& operator=(const ubx_location_snapshot&) = default;

    // ── Accessors ─────────────────────────────────────────────────────────────

    commit_kind kind() const { return kind_; }
    uint64_t    msg_mask() const { return msg_mask_; }

    template <typename T>
    bool get(data_field field, T& out) const
    {
        return store_.get(field, out);
    }

    bool is_valid(data_field field) const
    {
        return store_.is_valid(field);
    }

private:
    ubx_field_store store_;     ///< Value-copy of the live store at commit time
    uint64_t        msg_mask_;  ///< Message mask at commit time
    commit_kind     kind_;      ///< What kind of commit triggered this snapshot
};

} // namespace database
} // namespace ubx
