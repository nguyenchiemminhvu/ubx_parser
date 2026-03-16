// ubx_msg_mask.h
//
// Atomic bitmask tracking which UBX messages have arrived in the current epoch.
//
// This is the new equivalent of CDatabase::MSG_MASK.
// msg_type values 0–63 map to bit positions.  (MSG_NUM = 40 which fits in uint64.)
//
// Thread-safety:  All operations use std::atomic with default sequential
//                 consistency, matching the old CDatabase behaviour.

#pragma once

#include "database/ubx_data_fields.h"
#include <atomic>
#include <cstdint>

namespace ubx
{
namespace database
{

class ubx_msg_mask
{
public:
    ubx_msg_mask() : mask_(0u) {}

    void set(msg_type msg)
    {
        mask_.fetch_or(bit_of(msg));
    }

    bool is_set(msg_type msg) const
    {
        return (mask_.load() & bit_of(msg)) != 0u;
    }

    uint64_t load() const
    {
        return mask_.load();
    }

    void clear(uint64_t bits)
    {
        mask_.fetch_and(~bits);
    }

    void reset()
    {
        mask_.store(0u);
    }

private:
    static uint64_t bit_of(msg_type msg)
    {
        return static_cast<uint64_t>(1u) << static_cast<uint64_t>(msg);
    }

    std::atomic<uint64_t> mask_;
};

} // namespace database
} // namespace ubx
