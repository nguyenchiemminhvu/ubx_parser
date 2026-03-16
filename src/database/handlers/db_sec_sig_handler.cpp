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
// src/database/handlers/db_sec_sig_handler.cpp
//
// Maps ubx_sec_sig fields to database data_field entries.

#include "database/handlers/db_sec_sig_handler.h"
#include "database/ubx_field_store.h"
#include "database/ubx_msg_mask.h"
#include "database/ubx_data_fields.h"

namespace ubx
{
namespace database
{

void db_sec_sig_handler::update(const parser::ubx_sec_sig& msg)
{
    msg_ = msg;
}

void db_sec_sig_handler::handle(ubx_field_store& store, ubx_msg_mask& mask)
{
    const parser::ubx_sec_sig& p = msg_;

    store.set(DATA_UBX_SEC_SIG_VERSION,         static_cast<uint32_t>(p.version));

    store.set(DATA_UBX_SEC_SIG_JAM_FLAGS,        static_cast<uint32_t>(p.jam_flags));
    store.set(DATA_UBX_SEC_SIG_JAM_DET_ENABLED,  static_cast<uint32_t>(p.jam_det_enabled));
    store.set(DATA_UBX_SEC_SIG_JAMMING_STATE,    static_cast<uint32_t>(p.jamming_state));

    store.set(DATA_UBX_SEC_SIG_SPF_FLAGS,        static_cast<uint32_t>(p.spf_flags));
    store.set(DATA_UBX_SEC_SIG_SPF_DET_ENABLED,  static_cast<uint32_t>(p.spf_det_enabled));
    store.set(DATA_UBX_SEC_SIG_SPOOFING_STATE,   static_cast<uint32_t>(p.spoofing_state));

    mask.set(MSG_UBX_SEC_SIG);
}

} // namespace database
} // namespace ubx
