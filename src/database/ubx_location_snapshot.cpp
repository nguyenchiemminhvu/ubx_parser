// src/database/ubx_location_snapshot.cpp

#include "database/ubx_location_snapshot.h"

namespace ubx
{
namespace database
{

ubx_location_snapshot::ubx_location_snapshot(const ubx_field_store& store,
                                             uint64_t               msg_mask_snapshot,
                                             commit_kind            kind)
    : store_(store)
    , msg_mask_(msg_mask_snapshot)
    , kind_(kind)
{
}

} // namespace database
} // namespace ubx
