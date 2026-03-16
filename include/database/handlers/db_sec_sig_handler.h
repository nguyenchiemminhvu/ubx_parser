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
// handlers/db_sec_sig_handler.h
//
// Message update handler for UBX-SEC-SIG.
//
// Holds the most-recently decoded ubx_sec_sig and, when handle() is called,
// maps every field to the corresponding data_field entry in the store.

#pragma once

#include "database/i_msg_update_handler.h"
#include "messages/ubx_sec_sig.h"

namespace ubx
{
namespace database
{

class db_sec_sig_handler : public i_msg_update_handler
{
public:
    db_sec_sig_handler() = default;

    // Called by the ubx_parser decoder stub to cache the latest message.
    void update(const parser::ubx_sec_sig& msg);

    // i_msg_update_handler
    msg_type handled_msg() const override { return MSG_UBX_SEC_SIG; }
    void handle(ubx_field_store& store, ubx_msg_mask& mask) override;

private:
    parser::ubx_sec_sig msg_{};
};

} // namespace database
} // namespace ubx
