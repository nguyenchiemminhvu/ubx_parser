// src/database/epoch_commit_policy.cpp

#include "database/epoch_commit_policy.h"

namespace ubx
{
namespace database
{

epoch_commit_policy::epoch_commit_policy(msg_type hp_epoch, msg_type lp_epoch)
    : hp_epoch_(hp_epoch)
    , lp_epoch_(lp_epoch)
{
}

void epoch_commit_policy::set_hp_epoch(msg_type msg)
{
    if (msg != MSG_NUM)
    {
        hp_epoch_ = msg;
    }
}

void epoch_commit_policy::set_lp_epoch(msg_type msg)
{
    if (msg != MSG_NUM)
    {
        lp_epoch_ = msg;
    }
}

commit_kind epoch_commit_policy::should_commit(msg_type msg) const
{
    if (msg == hp_epoch_)
    {
        return commit_kind::high_priority;
    }
    if (msg == lp_epoch_)
    {
        return commit_kind::low_priority;
    }
    return commit_kind::none;
}

} // namespace database
} // namespace ubx
