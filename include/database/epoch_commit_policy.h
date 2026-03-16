// epoch_commit_policy.h
//
// Default commit policy that mirrors old CDatabase::afterProcessing() logic:
//   • When the high-priority epoch message arrives → high_priority commit.
//   • When the low-priority  epoch message arrives → low_priority  commit.
//
// Defaults: HP = MSG_UBX_NAV_PVT, LP = MSG_UBX_NAV_SAT.
//
// Thread-safety: read-only after construction; safe to call from any thread.

#pragma once

#include "database/i_commit_policy.h"

namespace ubx
{
namespace database
{

class epoch_commit_policy : public i_commit_policy
{
public:
    explicit epoch_commit_policy(
        msg_type hp_epoch = MSG_UBX_NAV_PVT,
        msg_type lp_epoch = MSG_UBX_NAV_SAT);

    void set_hp_epoch(msg_type msg);
    void set_lp_epoch(msg_type msg);

    commit_kind should_commit(msg_type msg) const override;

private:
    msg_type hp_epoch_;
    msg_type lp_epoch_;
};

} // namespace database
} // namespace ubx
