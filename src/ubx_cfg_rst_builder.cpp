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

#include "ubx_cfg_rst_builder.h"
#include "ubx_message_builder.h"

namespace ubx
{
namespace parser
{

// ─── ubx_cfg_rst_builder private helper ───────────────────────────────────────

std::vector<uint8_t>
ubx_cfg_rst_builder::build(uint16_t nav_bbr_mask, uint8_t reset_mode)
{
    // UBX-CFG-RST payload (4 bytes):
    //   [0-1]  navBbrMask  U2  (little-endian) – BBR sectors to clear
    //   [2]    resetMode   U1  – reset type
    //   [3]    reserved1   U1  – always 0x00
    ubx_message_builder b;
    b.set_class(UBX_CLASS_CFG_RST)
     .set_id(UBX_ID_CFG_RST)
     .append_le16(nav_bbr_mask)
     .append_u8(reset_mode)
     .append_u8(0x00u); // reserved1
    return b.build();
}

// ─── public factory methods ───────────────────────────────────────────────────

std::vector<uint8_t> ubx_cfg_rst_builder::build_hot_stop()
{
    return build(NAV_BBR_HOT_START, CFG_RST_MODE_CTRL_STOP);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_warm_stop()
{
    return build(NAV_BBR_WARM_START, CFG_RST_MODE_CTRL_STOP);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_cold_stop()
{
    return build(NAV_BBR_COLD_START, CFG_RST_MODE_CTRL_STOP);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_hot_start()
{
    return build(NAV_BBR_HOT_START, CFG_RST_MODE_CTRL_START);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_warm_start()
{
    return build(NAV_BBR_WARM_START, CFG_RST_MODE_CTRL_START);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_cold_start()
{
    return build(NAV_BBR_COLD_START, CFG_RST_MODE_CTRL_START);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_hot_reset()
{
    return build(NAV_BBR_HOT_START, CFG_RST_MODE_CTRL_SW_GNSS_ONLY);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_warm_reset()
{
    return build(NAV_BBR_WARM_START, CFG_RST_MODE_CTRL_SW_GNSS_ONLY);
}

std::vector<uint8_t> ubx_cfg_rst_builder::build_cold_reset()
{
    return build(NAV_BBR_COLD_START, CFG_RST_MODE_CTRL_SW_GNSS_ONLY);
}

} // namespace parser
} // namespace ubx
