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
// ubx_esf_meas_builder.cpp
//
// Packs a populated ubx_esf_meas struct into a complete, checksummed UBX frame.
//
// UBX-ESF-MEAS payload (class 0x10, ID 0x02):
//   Bytes  0- 3 : timeTag (U4 LE)
//   Bytes  4- 5 : flags   (X2 LE) — bits[15:11] = numMeas, rest = 0
//   Bytes  6- 7 : id      (U2 LE)
//   Repeated numMeas × 4 bytes:
//     bits[23: 0] = dataField  (24-bit, signed or direction-flagged)
//     bits[29:24] = dataType   (6-bit sensor type)
//     bits[31:30] = 0          (reserved)
//
// No calibTtag is appended (calibTtagValid = 0 in flags).

#include "ubx_esf_meas_builder.h"
#include "ubx_message_builder.h"

namespace ubx
{
namespace parser
{

std::vector<uint8_t> ubx_esf_meas_builder::build(const ubx_esf_meas& meas)
{
    if (meas.num_meas == 0u || meas.num_meas > UBX_ESF_MEAS_MAX_DATA)
    {
        return {};
    }

    // flags: bits[15:11] = numMeas; all other bits = 0 (no calibTtag, no timeMarkSent)
    const uint16_t flags =
        static_cast<uint16_t>((static_cast<uint16_t>(meas.num_meas) << 11u) & 0xF800u);

    ubx_message_builder b;
    b.set_class(UBX_CLASS_ESF)
     .set_id(UBX_ID_ESF_MEAS)
     .append_le32(meas.time_tag)   // bytes 0-3: timeTag
     .append_le16(flags)           // bytes 4-5: flags
     .append_le16(meas.id);        // bytes 6-7: id

    for (uint8_t i = 0u; i < meas.num_meas; ++i)
    {
        // Pack: bits[23:0] = dataField, bits[29:24] = dataType, bits[31:30] = 0
        const uint32_t data_word =
            (static_cast<uint32_t>(meas.data[i].data_type) << 24u)
            | (static_cast<uint32_t>(meas.data[i].data_value) & 0x00FFFFFFu);

        b.append_le32(data_word);
    }

    return b.build();
}

} // namespace parser
} // namespace ubx
