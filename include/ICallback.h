#pragma once

#include <cstddef>

struct ICallback {
    virtual ~ICallback() = default;

    virtual void BinaryPacket(const char* data, std::size_t size) = 0;
    virtual void TextPacket(const char* data, std::size_t size) = 0;
};
