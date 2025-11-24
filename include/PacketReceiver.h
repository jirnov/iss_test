#include "PacketReader.h"
#include <ICallback.h>
#include <IReceiver.h>
#include <functional>
#include <memory>

using CharSpan = std::span<const char>;

// Реализация парсера пакетов
class PacketReceiver : IReceiver {
public:
    static constexpr char BINARY_MARKER = 0x24;

    explicit PacketReceiver(ICallback& callback);

    void Receive(const char* data, std::size_t size) override;

private:
    CharSpan WaitForPacket(CharSpan data);
    CharSpan ProcessPacket(CharSpan data);

    enum class State {
        WAIT_PACKET,
        PROCESS_PACKET,
    };

    State m_state{State::WAIT_PACKET};

    std::vector<char> m_buffer;
    std::unique_ptr<PacketReader> m_reader;

    ICallback& m_callback;
    std::function<void(CharSpan)> m_packetHandler;
};
