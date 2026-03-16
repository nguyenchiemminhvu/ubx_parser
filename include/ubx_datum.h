// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ubx_datum.h
//
// Self-contained datum types used by the ubx_parser layer.

#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>

namespace ubx
{

namespace parser
{

static constexpr double MAJA_TOLERANCE = 1e-3;   // metres
static constexpr double FLAT_TOLERANCE = 1e-7;   // dimensionless (1/f)

// ─── datum_id ─────────────────────────────────────────────────────────────────
//
// Identifies the GNSS reference datum resolved from the chip's navspg_usrdat
// configuration keys.  The chip stores the ellipsoid semi-major axis (maja) and
// inverse-flattening (flat) as IEEE 754 double-precision floats; these values
// are compared against known datum signatures to derive this enum.

enum class datum_id : uint8_t
{
    unknown      = 0,  ///< User datum enabled but parameters don't match any known datum
    wgs84        = 1,  ///< WGS-84 (GPS standard, default when usrdat disabled)
    pz90         = 2,  ///< PZ-90.11 (GLONASS standard, GOST 32453-2017)
    user_defined = 255 ///< Custom / non-standard user datum
};

// ─── datum_ellipsoid_params ────────────────────────────────────────────────────
//
// Raw ellipsoid parameters returned by the chip for the active user datum.
// Stored as the chip reports them (R8 for maja/flat, R4 for Helmert params).

struct datum_ellipsoid_params
{
    double maja;    ///< Semi-major axis [m]      (CFG-NAVSPG-USRDAT-MAJA)
    double flat;    ///< Inverse flattening (1/f)  (CFG-NAVSPG-USRDAT-FLAT)
};

// ─── datum_data ───────────────────────────────────────────────────────────────
//
// Combined datum information buffer.  Populated after a successful
// CFG-NAVSPG-USRDAT VALGET cycle.

struct datum_data
{
    bool                   usrdat_enabled; ///< false → chip uses WGS-84 internally
    datum_id               id;             ///< Resolved datum identifier
    const char*            name;           ///< Human-readable datum name (static string)
    datum_ellipsoid_params params;         ///< Raw ellipsoid + Helmert parameters from chip
    bool                   valid;          ///< true when at least one VALGET response was processed
};

// ─── Known datum ellipsoid signatures ────────────────────────────────────────
//
// Each entry: { datum_id, semi-major axis [m], inverse flattening (1/f) }
// Match tolerances: maja ±1.0 m,  flat ±0.001

struct datum_signature
{
    ubx::parser::datum_id id;
    double         maja;    // semi-major axis [m]
    double         flat;    // inverse flattening (1/f = a / (a-b))
};

static const datum_signature KNOWN_DATUMS[] =
{
    //  id                            maja [m]         1/f
    { ubx::parser::datum_id::wgs84,        6378137.0,      298.257223563 },  // GPS / WGS-84
    { ubx::parser::datum_id::pz90,         6378136.0,      298.257839303 },  // GLONASS PZ-90.11
};

} // namespace parser

} // namespace ubx
