// ubx_field_store.h
//
// Flat, double-precision value store indexed by data_field.
//
// ubx_field_store replaces CDatabase::m_varO[].  It stores every database
// field as a double (sufficient for all UBX scalar quantities) and provides
// typed Set<T>/Get<T> helpers.
//
// Thread-safety:  NOT thread-safe on its own.  The owning ubx_database protects
//                 this store with a shared_mutex (writers take exclusive lock,
//                 readers take shared lock via snapshot export).

#pragma once

#include "database/ubx_data_fields.h"
#include <cstring>
#include <cstdint>

namespace ubx
{
namespace database
{

class ubx_field_store
{
public:
    ubx_field_store() { reset(); }

    // ── Write ─────────────────────────────────────────────────────────────────

    /// Store any arithmetic value as double.
    template <typename T>
    void set(data_field field, T value)
    {
        if (field < DATA_PARSE)
        {
            values_[field]  = static_cast<double>(value);
            valid_[field]   = true;
        }
    }

    // ── Read ──────────────────────────────────────────────────────────────────

    /// Read back as the requested type.  Returns false if field was never set.
    template <typename T>
    bool get(data_field field, T& out) const
    {
        if (field < DATA_NUM && valid_[field])
        {
            out = static_cast<T>(values_[field]);
            return true;
        }
        return false;
    }

    bool is_valid(data_field field) const
    {
        return (field < DATA_NUM) && valid_[field];
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    void reset()
    {
        std::memset(values_, 0, sizeof(values_));
        std::memset(valid_,  0, sizeof(valid_));
    }

    void reset(data_field field)
    {
        if (field < DATA_NUM)
        {
            values_[field] = 0.0;
            valid_[field]  = false;
        }
    }

private:
    double values_[DATA_NUM];
    bool   valid_ [DATA_NUM];
};

} // namespace database
} // namespace ubx
