#include <PacketReceiver.h>
#include <TextPacketReader.h>
#include <Utils.h>

std::vector<char> createBinaryPacket(const std::vector<char>& data)
{
    std::vector<char> packet;
    packet.push_back(PacketReceiver::BINARY_MARKER);

    std::uint32_t size = static_cast<std::uint32_t>(data.size());
    packet.push_back(static_cast<char>(size & 0xff));
    packet.push_back(static_cast<char>((size >> 8) & 0xff));
    packet.push_back(static_cast<char>((size >> 16) & 0xff));
    packet.push_back(static_cast<char>((size >> 24) & 0xff));

    packet.insert(packet.end(), data.begin(), data.end());
    return packet;
}

// TODO: Нет экранирования TEXT_END_MARKER, если он встречается в исходном тексте
std::vector<char> createTextPacket(const std::string& text)
{
    constexpr auto& endMarker = TextPacketReader::TEXT_END_MARKER;

    std::vector<char> packet(text.begin(), text.end());
    packet.insert(packet.end(), endMarker.begin(), endMarker.end());
    return packet;
}
