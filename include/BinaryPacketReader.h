#pragma once

#include "PacketReader.h"
#include <vector>

// Читатель бинарных пакетов
class BinaryPacketReader : public PacketReader {
public:
    BinaryPacketReader(std::vector<char>& dataBuffer);

    CharSpan process(CharSpan data) override;
    CharSpan packet() const override;

private:
    enum class State {
        READ_SIZE,
        READ_DATA,
        COMPLETE,
    };

    // Читаем размер пакета за один или несколько попыток, используем m_dataBuffer в качестве буфера
    CharSpan readSize(CharSpan data);
    CharSpan readData(CharSpan data);

    State m_state{State::READ_SIZE};
    size_t m_dataSize{0};

    static constexpr size_t BYTES_PER_SIZE{4};
};
