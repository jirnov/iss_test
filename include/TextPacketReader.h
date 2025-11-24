#include "PacketReader.h"
#include <array>

// Читатель текстовых пакетов
class TextPacketReader : public PacketReader {
public:
    static constexpr std::array<char, 4> TEXT_END_MARKER = {'\r', '\n', '\r', '\n'};

    TextPacketReader(std::vector<char>& buffer);

    CharSpan process(CharSpan data) override;
    CharSpan packet() const override;

private:
    static constexpr std::size_t TEXT_END_MARKER_SIZE = TEXT_END_MARKER.size();

    size_t m_matchMarkerLen{0};
    bool m_complete{false};
};
