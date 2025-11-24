#include "TextPacketReader.h"

TextPacketReader::TextPacketReader(std::vector<char>& buffer)
   : PacketReader{buffer}
{}

CharSpan TextPacketReader::process(CharSpan data)
{
    if (m_complete) {
        return data;
    }

    // Если буфер пустой, пытаемся найти максимально длинный кусок
    if (m_dataBuffer.empty()) {
        for (std::size_t i = 0; i < data.size(); ++i) {
            const char c = data[i];
            const char expected = TEXT_END_MARKER[m_matchMarkerLen % TEXT_END_MARKER_SIZE];

            // Если символ из маркера
            if (c == expected) {
                // Сравниваем количество подходящих символов
                if (++m_matchMarkerLen == TEXT_END_MARKER_SIZE) {
                    m_directPacket = data.first(i - TEXT_END_MARKER_SIZE + 1);
                    m_complete = true;
                    return data.subspan(i + 1);
                }
            } else {
                m_matchMarkerLen = 0;
            }
        }
        // Обработали весь буфер и не нашли TEXT_END_MARKER, отложим на будущее
        m_dataBuffer.assign(data.begin(), data.end());
        return {};
    }

    // Заполняем буфер, пока не встретим маркер конца текста TEXT_END_MARKER
    for (std::size_t i = 0; i < data.size(); ++i) {
        const char c = data[i];
        m_dataBuffer.push_back(c);
        const char expected = TEXT_END_MARKER[m_matchMarkerLen % TEXT_END_MARKER_SIZE];
        if (c == expected) {
            if (++m_matchMarkerLen == TEXT_END_MARKER_SIZE) {
                m_complete = true;
                return data.subspan(i + 1);
            }
        } else {
            m_matchMarkerLen = 0;
        }
    }

    return {};
}

CharSpan TextPacketReader::packet() const
{
    if (m_complete) {
        if (!m_directPacket.empty()) {
            return m_directPacket;
        }
        // В буфере лежит TEXT_END_MARKER, убираем его
        return {m_dataBuffer.data(), m_dataBuffer.size() - TEXT_END_MARKER_SIZE};
    }
    return {};
}
