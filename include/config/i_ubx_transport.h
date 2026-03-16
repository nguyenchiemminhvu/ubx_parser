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
// config/i_ubx_transport.h
//
// Interface for the UART transport layer used by the configuration subsystem.
//
// The concrete implementation is provided by the application (e.g. Location
// Service's UART driver). This interface satisfies the Dependency Inversion
// Principle by decoupling the configuration logic from any OS or driver
// specifics.

#pragma once

#include <cstdint>
#include <vector>

namespace ubx
{
namespace config
{

class i_ubx_transport
{
public:
    virtual ~i_ubx_transport() = default;

    /// Send a fully framed UBX message (sync bytes through checksum) over UART.
    /// Returns true if the write completed without error.
    virtual bool send_async(const std::vector<uint8_t>& message) = 0;

    /// Synchronous version of send() support the callers a stronger delivery guarantees
    virtual bool send_sync(const std::vector<uint8_t>& message) = 0;
    virtual void handle_ack(uint8_t msg_class, uint8_t msg_id, bool is_ack) = 0;
};

} // namespace config
} // namespace ubx
