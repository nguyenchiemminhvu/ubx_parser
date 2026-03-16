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
// ubx_types.h
//
// Fundamental data types shared across the ubx_parser library.
// All types live in the ubx::parser namespace to avoid collision with the
// ubx_config library (namespace ubx).
//
// Thread-safety: All types defined here are plain data types with no internal
// synchronisation. Caller is responsible for any locking.

#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <string>

namespace ubx
{
namespace parser
{

// ─── UBX message class identifiers ────────────────────────────────────────────

static constexpr uint8_t UBX_CLASS_NAV  = 0x01u;  ///< Navigation results
static constexpr uint8_t UBX_CLASS_INF  = 0x04u;  ///< Informational
static constexpr uint8_t UBX_CLASS_ACK  = 0x05u;  ///< Acknowledgement
static constexpr uint8_t UBX_CLASS_CFG  = 0x06u;  ///< Configuration input/output
static constexpr uint8_t UBX_CLASS_MON  = 0x0Au;  ///< Monitoring
static constexpr uint8_t UBX_CLASS_TIM  = 0x0Du;  ///< Timing
static constexpr uint8_t UBX_CLASS_ESF  = 0x10u;  ///< External sensor fusion
static constexpr uint8_t UBX_CLASS_SEC  = 0x27u;  ///< Security
static constexpr uint8_t UBX_CLASS_NAV2 = 0x29u;  ///< Secondary navigation results

// ─── UBX message ID identifiers ───────────────────────────────────────────────

// ── NAV message IDs (class 0x01) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_NAV_STATUS  = 0x03u;  ///< Navigation receiver status
static constexpr uint8_t UBX_ID_NAV_DOP     = 0x04u;  ///< Dilution of precision
static constexpr uint8_t UBX_ID_NAV_ATT     = 0x05u;  ///< Attitude solution (vehicle dynamics)
static constexpr uint8_t UBX_ID_NAV_PVT     = 0x07u;  ///< Navigation PVT solution
static constexpr uint8_t UBX_ID_NAV_ODO     = 0x09u;  ///< Odometer solution
static constexpr uint8_t UBX_ID_NAV_CLOCK   = 0x22u;  ///< Clock solution
static constexpr uint8_t UBX_ID_NAV_TIMEGPS = 0x20u;  ///< GPS time solution
static constexpr uint8_t UBX_ID_NAV_TIMEUTC = 0x21u;  ///< UTC time solution
static constexpr uint8_t UBX_ID_NAV_SAT     = 0x35u;  ///< Satellite information
static constexpr uint8_t UBX_ID_NAV_EELL    = 0x3Du;  ///< Extended error-ellipse
static constexpr uint8_t UBX_ID_NAV_SIG     = 0x43u;  ///< Signal information

// ── ACK message IDs (class 0x05) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_ACK_NACK = 0x00u;  ///< Message not-acknowledged
static constexpr uint8_t UBX_ID_ACK_ACK  = 0x01u;  ///< Message acknowledged

// ── INF message IDs (class 0x04) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_INF_ERROR   = 0x00u;  ///< INF-ERROR informational message
static constexpr uint8_t UBX_ID_INF_WARNING = 0x01u;  ///< INF-WARNING informational message
static constexpr uint8_t UBX_ID_INF_NOTICE  = 0x02u;  ///< INF-NOTICE informational message
static constexpr uint8_t UBX_ID_INF_TEST    = 0x03u;  ///< INF-TEST informational message
static constexpr uint8_t UBX_ID_INF_DEBUG   = 0x04u;  ///< INF-DEBUG informational message

// ── CFG message IDs (class 0x06) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_CFG_VALSET         = 0x8Au;  ///< Configuration value set
static constexpr uint8_t UBX_ID_CFG_VALGET         = 0x8Bu;  ///< Configuration value get response

// ── CFG-VALGET / CFG-VALSET protocol version bytes ───────────────────────────
static constexpr uint8_t UBX_VALSET_VERSION        = 0x00u;  ///< VALSET payload version
static constexpr uint8_t UBX_VALGET_VERSION_POLL   = 0x00u;  ///< VALGET poll request version
static constexpr uint8_t UBX_VALGET_VERSION_RESP   = 0x01u;  ///< VALGET response version

// ── UBX-CFG-RST constants ─────────────────────────────────────────────────────
// Note: UBX_CLASS_CFG_RST == UBX_CLASS_CFG (0x06); provided as a named alias
// for readability at call sites that deal exclusively with the reset message.
static constexpr uint8_t  UBX_CLASS_CFG_RST    = 0x06u;  ///< CFG class (same as UBX_CLASS_CFG)
static constexpr uint8_t  UBX_ID_CFG_RST       = 0x04u;  ///< UBX-CFG-RST message ID

/// Controlled software reset mode byte (payload offset 2 of UBX-CFG-RST).
/// Value 0x01 is the "controlled software reset" as defined in the u-blox
/// ZED-F9R Interface Description (UBX-19056845), Table UBX-CFG-RST fields.
static constexpr uint8_t  CFG_RST_MODE_CTRL_SW = 0x01u;  ///< Controlled software reset
static constexpr uint8_t  CFG_RST_MODE_CTRL_SW_GNSS_ONLY = 0x02u;  ///< Controlled software reset (GNSS only)

/// Controlled stop/start mode byte (payload offset 2 of UBX-CFG-RST).
/// Value 0x08 is the "controlled stop" and value 0x09 is the "controlled start"
/// as defined in the u-blox ZED-F9R Interface Description (UBX-19056845), Table UBX-CFG-RST fields.
static constexpr uint8_t  CFG_RST_MODE_CTRL_STOP = 0x08u; ///< Controlled stop
static constexpr uint8_t  CFG_RST_MODE_CTRL_START = 0x09u; ///< Controlled start

/// navBbrMask values for UBX-CFG-RST (payload bytes 0-1, little-endian U2).
/// These control which Battery-Backed RAM data is cleared before the reset.
static constexpr uint16_t NAV_BBR_HOT_START    = 0x0000u; ///< All aiding retained  (hot start)
static constexpr uint16_t NAV_BBR_WARM_START   = 0x0001u; ///< Ephemeris discarded  (warm start)
static constexpr uint16_t NAV_BBR_COLD_START   = 0xFFFFu; ///< All aiding erased    (cold start)

// ── UPD message class and ID ──────────────────────────────────────────────────
static constexpr uint8_t UBX_CLASS_UPD         = 0x09u;  ///< Firmware update / Save-on-Shutdown

// ── RXM message class and message IDs ────────────────────────────────────────
static constexpr uint8_t UBX_CLASS_RXM         = 0x02u;  ///< Receiver Manager
static constexpr uint8_t UBX_ID_RXM_MEASX      = 0x14u;  ///< Satellite measurements for RRLP

// ── UBX-UPD-SOS message IDs (class 0x09) ─────────────────────────────────────
static constexpr uint8_t UBX_ID_UPD_SOS        = 0x14u;  ///< UBX-UPD-SOS (Save-on-Shutdown)

// ── UBX-UPD-SOS sub-command bytes (payload byte 0 of every message) ──────────
// Reference: ZED-F9R Interface Description UBX-19056845, section UBX-UPD-SOS.
//
// Input commands (host → receiver), 4-byte payload [cmd, reserved0×3]:
static constexpr uint8_t UPD_SOS_CMD_SAVE      = 0x00u;  ///< Host→receiver: save backup to flash
static constexpr uint8_t UPD_SOS_CMD_CLEAR     = 0x01u;  ///< Host→receiver: clear backup in flash

// Output messages (receiver → host), 8-byte payload [cmd, reserved0×3, response, reserved1×3]:
static constexpr uint8_t UPD_SOS_CMD_ACK       = 0x02u;  ///< Receiver→host: backup creation acknowledge
static constexpr uint8_t UPD_SOS_CMD_RESTORE   = 0x03u;  ///< Receiver→host: system restored from backup

// ── UBX-UPD-SOS response field values (output payload byte 4) ────────────────
static constexpr uint8_t UPD_SOS_RESP_NOT_ACK  = 0x00u;  ///< Not acknowledged / not restored
static constexpr uint8_t UPD_SOS_RESP_ACK      = 0x01u;  ///< Backup creation acknowledged / restored

// ── MON message IDs (class 0x0A) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_MON_VER     = 0x04u;  ///< Receiver/software version
static constexpr uint8_t UBX_ID_MON_IO      = 0x02u;  ///< I/O subsystem status
static constexpr uint8_t UBX_ID_MON_TXBUF   = 0x08u;  ///< TX buffer status
static constexpr uint8_t UBX_ID_MON_SPAN    = 0x31u;  ///< Signal characteristics / spectrum

// ── TIM message IDs (class 0x0D) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_TIM_TP      = 0x01u;  ///< Timepulse timedata

// ── ESF message IDs (class 0x10) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_ESF_MEAS   = 0x02u;  ///< ESF measurement data
static constexpr uint8_t UBX_ID_ESF_STATUS = 0x10u;  ///< ESF sensor fusion status
static constexpr uint8_t UBX_ID_ESF_INS    = 0x15u;  ///< Vehicle dynamics (INS)

// ── SEC message IDs (class 0x27) ─────────────────────────────────────────────
static constexpr uint8_t UBX_ID_SEC_CRC     = 0x01u;  ///< CRC security check result
static constexpr uint8_t UBX_ID_SEC_SIG     = 0x09u;  ///< Signal security information (jamming/spoofing)

// ── NAV2 message IDs (class 0x29, same values as NAV) ────────────────────────
static constexpr uint8_t UBX_ID_NAV2_DOP     = 0x04u;  ///< NAV2 dilution of precision
static constexpr uint8_t UBX_ID_NAV2_PVT     = 0x07u;  ///< NAV2 PVT solution
static constexpr uint8_t UBX_ID_NAV2_TIMEGPS = 0x20u;  ///< NAV2 GPS time solution
static constexpr uint8_t UBX_ID_NAV2_EELL    = 0x3Du;  ///< NAV2 extended error-ellipse

// ─── UBX protocol framing constants ───────────────────────────────────────────

static constexpr uint8_t UBX_SYNC_CHAR_1 = 0xB5u;  ///< First UBX sync byte
static constexpr uint8_t UBX_SYNC_CHAR_2 = 0x62u;  ///< Second UBX sync byte

/// Minimum UBX frame size: 2 sync + 1 class + 1 id + 2 length + 2 checksum = 8 bytes
static constexpr uint16_t UBX_FRAME_OVERHEAD = 8u;

/// Maximum supported payload length (64 KiB − 1).
/// This is the hard ceiling imposed by the 16-bit length field in the UBX
/// frame header.  It is NOT used as the accept/reject threshold in the parser;
/// see UBX_SAFE_MAX_PAYLOAD_LEN for the practical limit.
static constexpr uint16_t UBX_MAX_PAYLOAD_LEN = 0xFFFFu;

/// Safe practical maximum payload length for real-world UBX messages.
///
/// All known u-blox message types fit well within 4 096 bytes:
///   NAV-PVT   = 92 B,  NAV-SAT ≤ 872 B,  NAV-SIG ≤ 1 928 B.
///
/// ubx_parser uses this as its default accept/reject threshold.  Frames
/// declaring a payload larger than this value are immediately rejected with a
/// payload_too_large error and the parser re-synchronises, preventing
/// unbounded buffer growth from malformed or malicious input.
///
/// Override with ubx_parser::set_max_payload_length() when a specific
/// deployment is known to emit legitimately larger frames.
static constexpr uint16_t UBX_SAFE_MAX_PAYLOAD_LEN = 4096u;

// ─── ubx_message_header ───────────────────────────────────────────────────────
//
// Decoded header portion of a UBX frame, extracted before the payload is fully
// buffered. Stored in the parse state machine during multi-chunk reception.

struct ubx_message_header
{
    uint8_t  msg_class;      ///< UBX message class byte
    uint8_t  msg_id;         ///< UBX message ID byte
    uint16_t payload_length; ///< Declared payload length in bytes (little-endian)
};

// ─── ubx_frame ────────────────────────────────────────────────────────────────
//
// A fully validated UBX frame, passed to decoder implementations and callback
// functions.  The payload field holds only the message payload bytes (not the
// framing bytes or the checksum).

struct ubx_frame
{
    ubx_message_header header;   ///< Parsed header
    std::vector<uint8_t> payload; ///< Raw payload bytes (length == header.payload_length)
    uint8_t ck_a;                 ///< Received CK_A checksum byte
    uint8_t ck_b;                 ///< Received CK_B checksum byte
};

// ─── Callback type aliases ────────────────────────────────────────────────────

/// Generic callback receiving a fully validated raw UBX frame.
/// Used for unsupported message types and raw-frame observers.
using raw_message_callback = std::function<void(const ubx_frame&)>;

} // namespace parser
} // namespace ubx
