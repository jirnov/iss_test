#include "BinaryPacketReader.h"
#include "PacketReceiver.h"
#include "TextPacketReader.h"

PacketReceiver::PacketReceiver(ICallback& callback)
   : m_callback{callback}
{}

void PacketReceiver::Receive(const char* data, std::size_t size)
{
    CharSpan input(data, size);

    while (!input.empty()) {

        switch (m_state) {
            case State::WAIT_PACKET:
                input = WaitForPacket(input);
                break;
            case State::PROCESS_PACKET:
                input = ProcessPacket(input);
                break;
        }
    }
}

CharSpan PacketReceiver::WaitForPacket(CharSpan data)
{
    if (data.empty()) {
        return data;
    }

    m_state = State::PROCESS_PACKET;

    if (data.front() == BINARY_MARKER) {
        m_reader = std::make_unique<BinaryPacketReader>(m_buffer);
        m_packetHandler = [this](auto packet) {
            m_callback.BinaryPacket(packet.data(), packet.size());
        };
        return data.subspan(1);
    }
    m_reader = std::make_unique<TextPacketReader>(m_buffer);
    m_packetHandler = [this](auto packet) {
        m_callback.TextPacket(packet.data(), packet.size());
    };

    return data;
}

CharSpan PacketReceiver::ProcessPacket(CharSpan data)
{
    data = m_reader->process(data);
    auto packet = m_reader->packet();
    if (!packet.empty()) {
        m_packetHandler(packet);
        m_packetHandler = nullptr;
        m_state = State::WAIT_PACKET;
        m_reader.reset();
    }
    return data;
}
