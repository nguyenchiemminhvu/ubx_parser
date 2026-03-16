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
// examples/basic_parse_demo.cpp
//
// Demonstrates the full ubx_parser + ubx_database integration:
//
//   Parser layer  (ubx_parser):
//     1. Build UBX frames with ubx_message_builder.
//     2. Feed bytes in fragments to simulate partial serial reads.
//     3. Observe raw / error callbacks for unregistered / malformed frames.
//
//   Database layer  (ubx_database / ubx_database_adapter):
//     4. Create a ubx_database with a default epoch_commit_policy
//        (HP epoch = NAV-PVT, LP epoch = NAV-SAT).
//     5. Attach message update handlers for NAV-PVT, NAV-SAT, NAV-DOP,
//        NAV-EELL, NAV-TIMEGPS, NAV-STATUS, TIM-TP.
//     6. Wire the adapter into the parser registry via register_with_parser().
//     7. On every HP or LP commit the snapshot callback prints a location
//        summary read from ubx_location_snapshot.

// ── ubx_parser library ────────────────────────────────────────────────────────
#include "ubx_parser.h"
#include "ubx_decoder_registry.h"
#include "ubx_message_builder.h"
#include "ubx_protocol.h"
#include "decoders/cfg_valget_decoder.h"
#include "decoders/sec_crc_decoder.h"
#include "messages/ubx_nav_pvt.h"
#include "messages/ubx_nav_sat.h"
#include "messages/ubx_cfg_valget.h"
#include "messages/ubx_sec_crc.h"
#include "messages/ubx_raw_message.h"
#include "ubx_errors.h"

// ── ubx_database layer ────────────────────────────────────────────────────────
#include "database/ubx_database.h"
#include "database/ubx_database_adapter.h"
#include "database/epoch_commit_policy.h"
#include "database/ubx_location_snapshot.h"
#include "database/ubx_data_fields.h"
#include "database/handlers/db_nav_pvt_handler.h"
#include "database/handlers/db_nav_sat_handler.h"
#include "database/handlers/db_nav_dop_handler.h"
#include "database/handlers/db_nav_eell_handler.h"
#include "database/handlers/db_nav_timegps_handler.h"
#include "database/handlers/db_nav_status_handler.h"
#include "database/handlers/db_tim_tp_handler.h"

#include <cstdio>
#include <memory>

// ─── Payload builders ─────────────────────────────────────────────────────────

// NAV-PVT: 92-byte payload with a few meaningful fields set.
// lat  =  107500000 (10.75 deg), lon = 1068750000 (106.875 deg),
// fixType = 3 (3D), numSV = 10, year=2026, month=3, day=17
static std::vector<uint8_t> make_nav_pvt_payload()
{
    std::vector<uint8_t> p(ubx::parser::UBX_NAV_PVT_PAYLOAD_LEN, 0x00u);
    // iTOW (offset 0, U4) = 400000 ms
    uint32_t itow = 400000u;
    p[0] = itow & 0xFFu; p[1] = (itow>>8)&0xFFu; p[2] = (itow>>16)&0xFFu; p[3] = (itow>>24)&0xFFu;
    // year (offset 4, U2) = 2026
    p[4] = 0xEAu; p[5] = 0x07u;
    // month=3, day=17, hour=10, min=30, sec=0
    p[6]=3u; p[7]=17u; p[8]=10u; p[9]=30u; p[10]=0u;
    // valid = 0x07 (date+time+fully resolved)
    p[11] = 0x07u;
    // fixType (offset 20) = 3
    p[20] = 0x03u;
    // flags (offset 21) = 0x01 (gnssFixOk)
    p[21] = 0x01u;
    // numSV (offset 23) = 10
    p[23] = 10u;
    // lon (offset 24, I4) = 1068750000
    uint32_t lon = static_cast<uint32_t>(1068750000);
    p[24]=(lon)&0xFFu; p[25]=(lon>>8)&0xFFu; p[26]=(lon>>16)&0xFFu; p[27]=(lon>>24)&0xFFu;
    // lat (offset 28, I4) = 107500000
    uint32_t lat = static_cast<uint32_t>(107500000);
    p[28]=(lat)&0xFFu; p[29]=(lat>>8)&0xFFu; p[30]=(lat>>16)&0xFFu; p[31]=(lat>>24)&0xFFu;
    // height (offset 32) = 50000 mm
    uint32_t h = 50000u;
    p[32]=(h)&0xFFu; p[33]=(h>>8)&0xFFu; p[34]=(h>>16)&0xFFu; p[35]=(h>>24)&0xFFu;
    // hAcc (offset 40) = 1200 mm
    uint32_t hacc=1200u;
    p[40]=(hacc)&0xFFu; p[41]=(hacc>>8)&0xFFu;
    // gSpeed (offset 60, I4) = 555 mm/s
    uint32_t gspd=555u;
    p[60]=(gspd)&0xFFu; p[61]=(gspd>>8)&0xFFu; p[62]=(gspd>>16)&0xFFu; p[63]=(gspd>>24)&0xFFu;
    return p;
}

// NAV-SAT: 3 satellites (header 8 bytes + 3 × 12 bytes = 44 bytes)
static std::vector<uint8_t> make_nav_sat_payload()
{
    std::vector<uint8_t> p;
    // iTOW=400000
    p.push_back(0x40u); p.push_back(0x1Au); p.push_back(0x06u); p.push_back(0x00u);
    p.push_back(0x01u); // version
    p.push_back(0x03u); // numSvs = 3
    p.push_back(0x00u); p.push_back(0x00u); // reserved
    // SV block: gnssId, svId, cno, elev, azim(2), prRes(2), flags(4)
    // flags: bit3=used, bit4=healthy, bit11=eph, bit12=alm → 0x00001818
    uint32_t flags = (1u<<3)|(1u<<4)|(1u<<11)|(1u<<12);
    auto push_sv = [&](uint8_t gid, uint8_t sid, uint8_t cno, int8_t el, int16_t az) {
        p.push_back(gid); p.push_back(sid); p.push_back(cno);
        p.push_back(static_cast<uint8_t>(el));
        p.push_back(az & 0xFF); p.push_back((az>>8)&0xFF);
        p.push_back(0x00u); p.push_back(0x00u); // prRes
        p.push_back(flags&0xFFu); p.push_back((flags>>8)&0xFFu);
        p.push_back((flags>>16)&0xFFu); p.push_back((flags>>24)&0xFFu);
    };
    push_sv(0,  1, 38,  45,  90);
    push_sv(0,  5, 32,  30, 180);
    push_sv(2, 12, 27,  20,  45);
    return p;
}

// CFG-VALGET response: one key (rate_meas = 1000 ms)
static std::vector<uint8_t> make_cfg_valget_payload()
{
    std::vector<uint8_t> p;
    p.push_back(0x01u); p.push_back(0x00u); p.push_back(0x00u); p.push_back(0x00u);
    p.push_back(0x01u); p.push_back(0x00u); p.push_back(0x21u); p.push_back(0x30u); // key
    p.push_back(0xE8u); p.push_back(0x03u); // value = 1000
    return p;
}

// SEC-CRC minimal payload
static std::vector<uint8_t> make_sec_crc_payload()
{
    std::vector<uint8_t> p;
    p.push_back(0x00u); p.push_back(0x01u); p.push_back(0x00u); p.push_back(0x00u);
    p.push_back(0xEFu); p.push_back(0xBEu); p.push_back(0xADu); p.push_back(0xDEu);
    return p;
}

// ─── Commit callbacks ─────────────────────────────────────────────────────────

// High-priority commit: triggered by NAV-PVT.
// Prints position, fix quality, and ground speed.
static void on_hp_commit(const ubx::database::ubx_location_snapshot& snap)
{
    using namespace ubx::database;

    std::printf("\n┌─ [COMMIT HP] msg_mask=0x%016llX\n",
                static_cast<unsigned long long>(snap.msg_mask()));

    double lat = 0.0, lon = 0.0, alt = 0.0;
    snap.get(DATA_UBX_NAV_PVT_LAT, lat);
    snap.get(DATA_UBX_NAV_PVT_LON, lon);
    snap.get(DATA_UBX_NAV_PVT_ALT, alt);

    uint8_t  fix    = 0u;
    uint32_t numsv  = 0u;
    float    gspeed = 0.f;
    int64_t  ts     = 0;
    snap.get(DATA_UBX_NAV_PVT_FIXTYPE,   fix);
    snap.get(DATA_UBX_NAV_PVT_NUMSV,     numsv);
    snap.get(DATA_UBX_NAV_PVT_GSPEED,    gspeed);
    snap.get(DATA_UBX_NAV_PVT_TIMESTAMP, ts);

    std::printf("│  lat=%.7f°  lon=%.7f°  alt=%.1f mm\n", lat, lon, alt);
    std::printf("│  fixType=%u  numSV=%u  gSpeed=%.1f mm/s  timestamp=%lld ms\n",
                fix, numsv, gspeed, static_cast<long long>(ts));
    std::printf("└──────────────────────────────────────\n");
}

// Low-priority commit: triggered by NAV-SAT.
// Prints the satellite sky-view table plus the cached position for context.
static void on_lp_commit(const ubx::database::ubx_location_snapshot& snap)
{
    using namespace ubx::database;

    std::printf("\n┌─ [COMMIT LP] msg_mask=0x%016llX\n",
                static_cast<unsigned long long>(snap.msg_mask()));

    uint32_t sat_count = 0u;
    snap.get(DATA_SATELLITES_IN_VIEW, sat_count);
    std::printf("│  satellites_in_view=%u\n", sat_count);

    for (uint32_t i = 0u; i < sat_count; ++i)
    {
        uint8_t gnss_id = 0u, sv_id = 0u, cno = 0u;
        bool    used    = false;
        snap.get(db_index_with_offset(DATA_SATELLITES_IN_VIEW_GNSSID,    static_cast<uint8_t>(i)), gnss_id);
        snap.get(db_index_with_offset(DATA_SATELLITES_IN_VIEW_SVID,      static_cast<uint8_t>(i)), sv_id);
        snap.get(db_index_with_offset(DATA_SATELLITES_IN_VIEW_CN0_RATIO, static_cast<uint8_t>(i)), cno);
        snap.get(db_index_with_offset(DATA_SATELLITES_IN_VIEW_USED,      static_cast<uint8_t>(i)), used);
        std::printf("│    [%u] gnss=%u sv=%-3u cno=%u dBHz  used=%s\n",
                    i, gnss_id, sv_id, cno, used ? "yes" : "no");
    }

    // Also print the last-known position for context (carried over from the HP epoch).
    double lat = 0.0, lon = 0.0;
    snap.get(DATA_UBX_NAV_PVT_LAT, lat);
    snap.get(DATA_UBX_NAV_PVT_LON, lon);
    std::printf("│  (last position: lat=%.7f°  lon=%.7f°)\n", lat, lon);
    std::printf("└──────────────────────────────────────\n");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    using namespace ubx::parser;
    using namespace ubx::database;

    std::printf("=== ubx_parser + ubx_database demo ===\n\n");

    // ── Step 1: Create the database with HP=NAV-PVT, LP=NAV-SAT ──────────────

    auto policy = std::unique_ptr<i_commit_policy>(
        new epoch_commit_policy(MSG_UBX_NAV_PVT, MSG_UBX_NAV_SAT));

    auto db = std::make_shared<ubx_database>(std::move(policy));
    db->set_commit_callback(commit_kind::high_priority, on_hp_commit);
    db->set_commit_callback(commit_kind::low_priority,  on_lp_commit);

    // ── Step 2: Register message update handlers ──────────────────────────────

    ubx_database_adapter adapter(db);
    adapter.add_handler(std::unique_ptr<i_msg_update_handler>(new db_nav_pvt_handler()));
    adapter.add_handler(std::unique_ptr<i_msg_update_handler>(new db_nav_sat_handler()));
    adapter.add_handler(std::unique_ptr<i_msg_update_handler>(new db_nav_dop_handler()));
    adapter.add_handler(std::unique_ptr<i_msg_update_handler>(new db_nav_eell_handler()));
    adapter.add_handler(std::unique_ptr<i_msg_update_handler>(new db_nav_timegps_handler()));
    adapter.add_handler(std::unique_ptr<i_msg_update_handler>(new db_nav_status_handler()));
    adapter.add_handler(std::unique_ptr<i_msg_update_handler>(new db_tim_tp_handler()));

    // ── Step 3: Build the parser registry and wire the adapter into it ────────

    ubx_decoder_registry registry;

    // Wire all database handlers → parser decoders via the adapter.
    adapter.register_with_parser(registry);

    // Add non-database decoders directly (CFG-VALGET, SEC-CRC).
    registry.register_decoder(
        std::unique_ptr<cfg_valget_decoder>(
            new cfg_valget_decoder([](const ubx_cfg_valget& m) {
                std::printf("[CFG-VALGET] layer=%u  entries=%zu\n",
                            m.layer, m.entries.size());
                for (const auto& e : m.entries)
                    std::printf("  key=0x%08X  value=%llu\n",
                                e.key_id,
                                static_cast<unsigned long long>(e.value));
            })));

    registry.register_decoder(
        std::unique_ptr<sec_crc_decoder>(
            new sec_crc_decoder([](const ubx_sec_crc& m) {
                std::printf("[SEC-CRC] crc32=0x%08X\n", m.crc32);
            })));

    // ── Step 4: Create the parser, install raw + error callbacks ─────────────

    ubx_parser parser(std::move(registry));

    parser.set_raw_message_callback([](const ubx_raw_message& raw) {
        std::printf("[RAW] class=0x%02X  id=0x%02X  payload=%u bytes\n",
                    raw.frame.header.msg_class,
                    raw.frame.header.msg_id,
                    raw.frame.header.payload_length);
    });

    parser.set_error_callback([](const parse_error_info& e) {
        std::printf("[ERROR] %s (class=0x%02X id=0x%02X)\n",
                    e.description.c_str(), e.msg_class, e.msg_id);
    });

    // ── Step 5: Build UBX frames ──────────────────────────────────────────────

    const std::vector<uint8_t> nav_pvt_frame =
        ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_PVT,
                                         make_nav_pvt_payload());

    const std::vector<uint8_t> nav_sat_frame =
        ubx_message_builder::make_frame(UBX_CLASS_NAV, UBX_ID_NAV_SAT,
                                         make_nav_sat_payload());

    const std::vector<uint8_t> cfg_valget_frame =
        ubx_message_builder::make_frame(UBX_CLASS_CFG, UBX_ID_CFG_VALGET,
                                         make_cfg_valget_payload());

    const std::vector<uint8_t> sec_crc_frame =
        ubx_message_builder::make_frame(UBX_CLASS_SEC, UBX_ID_SEC_CRC,
                                         make_sec_crc_payload());

    // MON-VER: no decoder registered → raw callback
    const std::vector<uint8_t> mon_ver_frame =
        ubx_message_builder::make_frame(UBX_CLASS_MON, UBX_ID_MON_VER,
                                         { 0x01u, 0x02u, 0x03u });

    // ── Step 6: Feed frames to the parser ─────────────────────────────────────

    // 6a. NAV-PVT single chunk → handler writes fields → HP commit fires.
    std::printf("-- 6a. Feed NAV-PVT (single chunk) --\n");
    parser.feed(nav_pvt_frame);

    // 6b. NAV-SAT fragmented → handler writes satellite table → LP commit fires.
    std::printf("\n-- 6b. Feed NAV-SAT (fragmented, 4-byte chunks) --\n");
    for (std::size_t i = 0u; i < nav_sat_frame.size(); )
    {
        const std::size_t end = std::min(i + 4u, nav_sat_frame.size());
        parser.feed(std::vector<uint8_t>(
            nav_sat_frame.begin() + static_cast<long>(i),
            nav_sat_frame.begin() + static_cast<long>(end)));
        i = end;
    }

    // 6c. CFG-VALGET → direct callback only (not through database).
    std::printf("\n-- 6c. Feed CFG-VALGET (single chunk) --\n");
    parser.feed(cfg_valget_frame);

    // 6d. SEC-CRC → direct callback.
    std::printf("\n-- 6d. Feed SEC-CRC (single chunk) --\n");
    parser.feed(sec_crc_frame);

    // 6e. MON-VER → raw callback (no decoder registered).
    std::printf("\n-- 6e. Feed MON-VER (raw callback) --\n");
    parser.feed(mon_ver_frame);

    // 6f. Two frames back-to-back in one chunk (NAV-PVT + NAV-SAT).
    std::printf("\n-- 6f. Feed NAV-PVT + NAV-SAT concatenated (two frames, two commits) --\n");
    std::vector<uint8_t> combined;
    combined.insert(combined.end(), nav_pvt_frame.begin(), nav_pvt_frame.end());
    combined.insert(combined.end(), nav_sat_frame.begin(),  nav_sat_frame.end());
    parser.feed(combined);

    // 6g. Noise bytes before a valid NAV-PVT (resync test → one HP commit).
    std::printf("\n-- 6g. Feed noise + NAV-PVT (resync) --\n");
    std::vector<uint8_t> noisy = { 0xAAu, 0xBBu, 0xCCu };
    noisy.insert(noisy.end(), nav_pvt_frame.begin(), nav_pvt_frame.end());
    parser.feed(noisy);

    // 6h. Bad checksum → error callback, no commit.
    std::printf("\n-- 6h. Feed NAV-PVT with corrupted checksum (error callback) --\n");
    std::vector<uint8_t> bad = nav_pvt_frame;
    bad[bad.size() - 1u] ^= 0xFFu;
    parser.feed(bad);

    // ── Step 7: Final statistics ──────────────────────────────────────────────

    std::printf("\n=== Stats: decoded=%llu  discarded=%llu ===\n",
                static_cast<unsigned long long>(parser.frames_decoded()),
                static_cast<unsigned long long>(parser.frames_discarded()));

    return 0;
}
