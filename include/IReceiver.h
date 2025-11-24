#pragma once

#include <cstddef>

struct IReceiver {
    virtual ~IReceiver() = default;

    virtual void Receive(const char* data, std::size_t size) = 0;
};
