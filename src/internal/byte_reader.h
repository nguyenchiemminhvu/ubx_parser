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
// internal/byte_reader.h
//
// Trivial helper that reads a contiguous block of typed values from a raw
// byte pointer with bounds checking.  Used by decoder implementations to
// parse fixed-offset fields from a payload buffer without unsafe pointer
// arithmetic spread across the codebase.

#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace ubx
{
namespace parser
{
namespace internal
{

// ─── byte_reader ──────────────────────────────────────────────────────────────
//
// Cursor-based reader over a const byte span.
// All read methods return false if not enough bytes remain, leaving the cursor
// unchanged on failure (strong read guarantee).

class byte_reader
{
public:
    /// Construct from a raw pointer and length.
    byte_reader(const uint8_t* data, std::size_t length)
        : data_(data), length_(length), pos_(0u)
    {}

    /// Construct from a vector.
    explicit byte_reader(const std::vector<uint8_t>& vec)
        : byte_reader(vec.data(), vec.size())
    {}

    /// @returns the number of bytes remaining (not yet read).
    std::size_t remaining() const { return length_ - pos_; }

    /// @returns the current read position.
    std::size_t position() const { return pos_; }

    /// Read a single byte.
    /// @param[out] out  Destination byte.
    /// @returns false if no bytes remain.
    bool read_u8(uint8_t& out)
    {
        if (remaining() < 1u) return false;
        out = data_[pos_++];
        return true;
    }

    /// Read a 2-byte little-endian unsigned integer.
    bool read_le16(uint16_t& out)
    {
        if (remaining() < 2u) return false;
        out = static_cast<uint16_t>(data_[pos_])
            | (static_cast<uint16_t>(data_[pos_ + 1u]) << 8u);
        pos_ += 2u;
        return true;
    }

    /// Read a 4-byte little-endian unsigned integer.
    bool read_le32(uint32_t& out)
    {
        if (remaining() < 4u) return false;
        out =  static_cast<uint32_t>(data_[pos_])
            | (static_cast<uint32_t>(data_[pos_ + 1u]) <<  8u)
            | (static_cast<uint32_t>(data_[pos_ + 2u]) << 16u)
            | (static_cast<uint32_t>(data_[pos_ + 3u]) << 24u);
        pos_ += 4u;
        return true;
    }

    /// Read a 4-byte little-endian signed integer.
    bool read_le32s(int32_t& out)
    {
        uint32_t tmp = 0u;
        if (!read_le32(tmp)) return false;
        out = static_cast<int32_t>(tmp);
        return true;
    }

    /// Read an 8-byte little-endian unsigned integer.
    bool read_le64(uint64_t& out)
    {
        if (remaining() < 8u) return false;
        out = 0u;
        for (unsigned i = 0u; i < 8u; ++i)
            out |= (static_cast<uint64_t>(data_[pos_ + i]) << (8u * i));
        pos_ += 8u;
        return true;
    }

    /// Read a 2-byte little-endian signed integer.
    bool read_le16s(int16_t& out)
    {
        uint16_t tmp = 0u;
        if (!read_le16(tmp)) return false;
        out = static_cast<int16_t>(tmp);
        return true;
    }

    /// Skip 'n' bytes.
    /// @returns false if fewer than n bytes remain.
    bool skip(std::size_t n)
    {
        if (remaining() < n) return false;
        pos_ += n;
        return true;
    }

private:
    const uint8_t* data_;
    std::size_t    length_;
    std::size_t    pos_;
};

} // namespace internal
} // namespace parser
} // namespace ubx
