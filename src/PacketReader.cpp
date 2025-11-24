#include "PacketReader.h"

PacketReader::PacketReader(std::vector<char>& dataBuffer)
   : m_dataBuffer{dataBuffer}
{
    m_dataBuffer.clear();
}
