#pragma once

#include "Types.h"
#include <vector>

// Базовый класс для читателей пакетов
class PacketReader {
public:
    PacketReader(std::vector<char>& dataBuffer);
    virtual ~PacketReader() = default;

    // Обработка потока данных, возвращает необработанный остаток
    virtual CharSpan process(CharSpan data) = 0;

    // Возвращает пакет с данными, если он готов, иначе пустой пакет
    virtual CharSpan packet() const = 0;

protected:
    // Шарим буфер данных между всеми читателями для экономии памяти
    std::vector<char>& m_dataBuffer;

    CharSpan m_directPacket;
};
