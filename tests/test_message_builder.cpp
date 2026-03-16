// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/test_message_builder.cpp — Unit tests for ubx_message_builder

#include "ubx_message_builder.h"
#include "ubx_protocol.h"
#include "ubx_types.h"
#include <cassert>
#include <cstdio>
#include <string>

using namespace ubx::parser;

static void pass(const std::string& name)
{
    std::printf("[PASS] %s\n", name.c_str());
}

static void fail(const std::string& name, const std::string& reason)
{
    std::printf("[FAIL] %s: %s\n", name.c_str(), reason.c_str());
}

// ─── tests ────────────────────────────────────────────────────────────────────

/// Static make_frame with payload produces valid, checksummed frame.
static void test_make_frame_with_payload()
{
    const char* name = "make_frame_with_payload";

    const std::vector<uint8_t> payload = { 0x01u, 0x02u, 0x03u };
    const auto frame = ubx_message_builder::make_frame(0x01u, 0x07u, payload);

    if (frame.size() != static_cast<std::size_t>(UBX_FRAME_OVERHEAD + payload.size()))
    {
        fail(name, "unexpected frame size");
        return;
    }
    if (frame[0] != UBX_SYNC_CHAR_1 || frame[1] != UBX_SYNC_CHAR_2)
    {
        fail(name, "sync bytes incorrect");
        return;
    }
    if (!protocol::verify_frame_checksum(frame))
    {
        fail(name, "checksum invalid");
        return;
    }
    pass(name);
}

/// Static make_frame with empty payload produces valid, minimum-size frame.
static void test_make_frame_no_payload()
{
    const char* name = "make_frame_no_payload";

    const auto frame = ubx_message_builder::make_frame(0x06u, 0x8Bu);

    if (frame.size() != static_cast<std::size_t>(UBX_FRAME_OVERHEAD))
    {
        fail(name, "unexpected frame size");
        return;
    }
    if (!protocol::verify_frame_checksum(frame))
    {
        fail(name, "checksum invalid");
        return;
    }
    pass(name);
}

/// Builder fluent API assembles correct frame.
static void test_builder_fluent_api()
{
    const char* name = "builder_fluent_api";

    ubx_message_builder b;
    const auto frame = b.set_class(0x01u)
                        .set_id(0x07u)
                        .append_u8(0xAAu)
                        .append_le16(0x1234u)
                        .append_le32(0xDEADBEEFu)
                        .build();

    if (!protocol::verify_frame_checksum(frame))
    {
        fail(name, "checksum invalid");
        return;
    }

    // Check class and id bytes in frame
    if (frame[2] != 0x01u || frame[3] != 0x07u)
    {
        fail(name, "class/id bytes incorrect");
        return;
    }
    pass(name);
}

/// clear_payload actually clears the accumulated payload.
static void test_builder_clear_payload()
{
    const char* name = "builder_clear_payload";

    ubx_message_builder b;
    b.set_class(0x0Au).set_id(0x04u).append_u8(0x01u).append_u8(0x02u);
    b.clear_payload();
    const auto frame = b.build();

    // Payload length field should be 0x00 0x00
    if (frame[4] != 0x00u || frame[5] != 0x00u)
    {
        fail(name, "payload not cleared");
        return;
    }
    pass(name);
}

/// Two different messages have different checksums.
static void test_different_messages_different_checksums()
{
    const char* name = "different_messages_different_checksums";

    const auto f1 = ubx_message_builder::make_frame(0x01u, 0x07u, { 0x00u });
    const auto f2 = ubx_message_builder::make_frame(0x01u, 0x07u, { 0xFFu });

    const uint8_t ck_a_1 = f1[f1.size() - 2u];
    const uint8_t ck_b_1 = f1[f1.size() - 1u];
    const uint8_t ck_a_2 = f2[f2.size() - 2u];
    const uint8_t ck_b_2 = f2[f2.size() - 1u];

    if (ck_a_1 != ck_a_2 || ck_b_1 != ck_b_2)
        pass(name);
    else
        fail(name, "different payloads produced same checksum");
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== test_message_builder ===\n");
    test_make_frame_with_payload();
    test_make_frame_no_payload();
    test_builder_fluent_api();
    test_builder_clear_payload();
    test_different_messages_different_checksums();
    std::printf("Done.\n");
    return 0;
}
