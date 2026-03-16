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

#include "ubx_upd_sos_builder.h"
#include "ubx_message_builder.h"

namespace ubx
{
namespace parser
{

std::vector<uint8_t> ubx_upd_sos_builder::build_poll()
{
    // Empty payload — the receiver replies with a backup creation acknowledge.
    return ubx_message_builder::make_frame(UBX_CLASS_UPD, UBX_ID_UPD_SOS);
}

std::vector<uint8_t> ubx_upd_sos_builder::build_save()
{
    // 4-byte payload: cmd=0x00 (save), reserved0[3].
    ubx_message_builder b;
    b.set_class(UBX_CLASS_UPD)
     .set_id(UBX_ID_UPD_SOS)
     .append_u8(UPD_SOS_CMD_SAVE)
     .append_u8(0x00u) // reserved0[0]
     .append_u8(0x00u) // reserved0[1]
     .append_u8(0x00u); // reserved0[2]
    return b.build();
}

std::vector<uint8_t> ubx_upd_sos_builder::build_clear()
{
    // 4-byte payload: cmd=0x01 (clear), reserved0[3].
    ubx_message_builder b;
    b.set_class(UBX_CLASS_UPD)
     .set_id(UBX_ID_UPD_SOS)
     .append_u8(UPD_SOS_CMD_CLEAR)
     .append_u8(0x00u) // reserved0[0]
     .append_u8(0x00u) // reserved0[1]
     .append_u8(0x00u); // reserved0[2]
    return b.build();
}

} // namespace parser
} // namespace ubx
