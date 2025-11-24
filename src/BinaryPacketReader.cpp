#include "BinaryPacketReader.h"
#include <cstdint>

BinaryPacketReader::BinaryPacketReader(std::vector<char>& buffer)
   : PacketReader{buffer}
{}

CharSpan BinaryPacketReader::process(CharSpan data)
{
    while (!data.empty()) {
        switch (m_state) {
            case State::READ_SIZE:
                data = readSize(data);
                break;

            case State::READ_DATA:
                data = readData(data);
                break;

            case State::COMPLETE:
                return data;
        }
    }

    return data;
}

CharSpan BinaryPacketReader::packet() const
{
    if (m_state == State::COMPLETE) {
        return m_directPacket.empty() ? m_dataBuffer : m_directPacket;
    }
    return {};
}

CharSpan BinaryPacketReader::readSize(CharSpan data)
{
    const size_t bytesToRead = std::min(BYTES_PER_SIZE - m_dataBuffer.size(), data.size());

    // Читаем минимально возможно количество байт в буфер
    m_dataBuffer.insert(m_dataBuffer.end(), data.begin(), data.begin() + bytesToRead);

    data = data.subspan(bytesToRead);

    // Накопили достаточно данных для чтения размера
    if (m_dataBuffer.size() == BYTES_PER_SIZE) {
        // clang-format off
        m_dataSize =
            (static_cast<std::uint32_t>(m_dataBuffer[0])) |
            (static_cast<std::uint32_t>(m_dataBuffer[1]) << 8) |
            (static_cast<std::uint32_t>(m_dataBuffer[2]) << 16) |
            (static_cast<std::uint32_t>(m_dataBuffer[3]) << 24);
        // clang-format on

        m_dataBuffer.clear();
        m_state = State::READ_DATA;
    }

    return data;
}

CharSpan BinaryPacketReader::readData(CharSpan data)
{
    const size_t bytesToRead = std::min(m_dataSize - m_dataBuffer.size(), data.size());

    if (m_dataBuffer.empty() && bytesToRead == m_dataSize) {
        m_directPacket = data.subspan(0, bytesToRead);
        m_state = State::COMPLETE;
    } else {
        m_dataBuffer.insert(m_dataBuffer.end(), data.begin(), data.begin() + bytesToRead);
        if (m_dataBuffer.size() == m_dataSize) {
            m_state = State::COMPLETE;
        }
    }
    return data.subspan(bytesToRead);
}
