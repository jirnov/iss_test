#include <ICallback.h>
#include <PacketReceiver.h>
#include <Utils.h>
#include <catch2/catch_all.hpp>
#include <span>
#include <vector>

struct MockCallback : ICallback {
    struct Packet {
        std::vector<char> payload;
        const char* original_ptr{nullptr};
    };

    void BinaryPacket(const char* data, std::size_t size) override
    {
        packets.emplace_back(std::vector<char>(data, data + size), data);
        binary_count++;
    }

    void TextPacket(const char* data, std::size_t size) override
    {
        packets.emplace_back(std::vector<char>(data, data + size), data);
        text_count++;
    }

    std::vector<Packet> packets;
    size_t binary_count = 0;
    size_t text_count = 0;
};

TEST_CASE(
   "Receive binary packet without copying"
   "[PacketReceiver][NoCopy]")
{
    MockCallback callback;
    PacketReceiver receiver(callback);

    const std::vector<char> binaryData = {'b', 'i', PacketReceiver::BINARY_MARKER, 'n'};
    auto packet = createBinaryPacket(binaryData);

    const char* original_data_ptr = packet.data() + 5;

    receiver.Receive(packet.data(), packet.size());

    REQUIRE(callback.packets.size() == 1);
    REQUIRE(callback.binary_count == 1);

    auto last = callback.packets.back();
    REQUIRE(last.original_ptr == original_data_ptr);
    REQUIRE(last.payload.size() == binaryData.size());
    REQUIRE(0 == memcmp(last.payload.data(), binaryData.data(), binaryData.size()));
}

TEST_CASE(
   "Receive text packet w/o copying"
   "[PacketReceiver][NoCopy]")
{
    MockCallback callback;
    PacketReceiver receiver(callback);

    const std::string textData = "t\r\n\re\nxt";
    auto packet = createTextPacket(textData);

    const char* original_data_ptr = packet.data();

    receiver.Receive(packet.data(), packet.size());

    REQUIRE(callback.packets.size() == 1);
    REQUIRE(callback.text_count == 1);

    auto last = callback.packets.back();
    REQUIRE(last.original_ptr == original_data_ptr);
    REQUIRE(last.payload.size() == textData.size());
}

TEST_CASE("Receive mixed packets w/o copying", "[PacketReceiver][NoCopy]")
{
    MockCallback callback;
    PacketReceiver receiver(callback);

    std::vector<char> stream;
    stream.reserve(1024);

    const std::vector<char> binaryData = {'b', PacketReceiver::BINARY_MARKER, 'i', 'n', 'a', 'r', 'y'};
    auto binaryPacket = createBinaryPacket(binaryData);
    stream.insert(stream.end(), binaryPacket.begin(), binaryPacket.end());
    const char* original_ptr0 = stream.data() + 5;

    const std::string textData = "abc";
    auto textPacket = createTextPacket(textData);
    stream.insert(stream.end(), textPacket.begin(), textPacket.end());
    const char* original_ptr1 = stream.data() + stream.size() - textPacket.size();

    stream.insert(stream.end(), binaryPacket.begin(), binaryPacket.end());
    const char* original_ptr2 = stream.data() + stream.size() - binaryPacket.size() + 5;

    stream.insert(stream.end(), textPacket.begin(), textPacket.end());
    const char* original_ptr3 = stream.data() + stream.size() - textPacket.size();

    receiver.Receive(stream.data(), stream.size());

    REQUIRE(callback.packets.size() == 4);
    REQUIRE(callback.binary_count == 2);
    REQUIRE(callback.text_count == 2);

    REQUIRE(callback.packets[0].original_ptr == original_ptr0);
    REQUIRE(callback.packets[1].original_ptr == original_ptr1);
    REQUIRE(callback.packets[2].original_ptr == original_ptr2);
    REQUIRE(callback.packets[3].original_ptr == original_ptr3);

    REQUIRE(callback.packets[0].payload.size() == binaryData.size());
    REQUIRE(0 == memcmp(callback.packets[0].payload.data(), binaryData.data(), binaryData.size()));

    REQUIRE(callback.packets[2].payload.size() == binaryData.size());
    REQUIRE(0 == memcmp(callback.packets[2].payload.data(), binaryData.data(), binaryData.size()));

    REQUIRE(callback.packets[1].payload.size() == textData.size());
    REQUIRE(0 == memcmp(callback.packets[1].payload.data(), textData.data(), textData.size()));

    REQUIRE(callback.packets[3].payload.size() == textData.size());
    REQUIRE(0 == memcmp(callback.packets[3].payload.data(), textData.data(), textData.size()));
}

TEST_CASE(
   "Receive binary packet with copying"
   "[PacketReceiver][Copy]")
{
    MockCallback callback;
    PacketReceiver receiver(callback);

    const std::vector<char> binaryData = {'b', 'i', 'n', PacketReceiver::BINARY_MARKER, 'a', 'r', 'y'};
    auto packet = createBinaryPacket(binaryData);

    const size_t chunkSize = GENERATE(1, 2, 3, 4, 5, 6);

    for (size_t i = 0; i < packet.size(); i += chunkSize) {
        const size_t bytesToRead = std::min(chunkSize, packet.size() - i);
        receiver.Receive(packet.data() + i, bytesToRead);
    }

    REQUIRE(callback.packets.size() == 1);
    REQUIRE(callback.binary_count == 1);

    auto last = callback.packets.back();
    REQUIRE(0 == memcmp(last.payload.data(), binaryData.data(), binaryData.size()));
    REQUIRE(last.payload.size() == binaryData.size());
}

TEST_CASE(
   "Receive text packet with copying"
   "[PacketReceiver][Copy]")
{
    MockCallback callback;
    PacketReceiver receiver(callback);

    const std::string textData = "t\r\n\re\nxt";
    auto packet = createTextPacket(textData);

    const size_t chunkSize = GENERATE(1, 2, 3, 4, 5, 6);

    for (size_t i = 0; i < packet.size(); i += chunkSize) {
        const size_t bytesToRead = std::min(chunkSize, packet.size() - i);
        receiver.Receive(packet.data() + i, bytesToRead);
    }

    REQUIRE(callback.packets.size() == 1);
    REQUIRE(callback.text_count == 1);

    auto last = callback.packets.back();
    REQUIRE(last.payload.size() == textData.size());
}

TEST_CASE("Receive mixed packets with copying", "[PacketReceiver][Copy]")
{
    MockCallback callback;
    PacketReceiver receiver(callback);

    std::vector<char> stream;

    const std::vector<char> binaryData = {'b', 'i', 'n', 'a', 'r', 'y'};
    auto binaryPacket = createBinaryPacket(binaryData);
    stream.insert(stream.end(), binaryPacket.begin(), binaryPacket.end());

    const std::string textData = "text";
    auto textPacket = createTextPacket(textData);
    stream.insert(stream.end(), textPacket.begin(), textPacket.end());

    stream.insert(stream.end(), binaryPacket.begin(), binaryPacket.end());
    stream.insert(stream.end(), textPacket.begin(), textPacket.end());

    const size_t chunkSize = GENERATE(1, 2, 3, 4, 5);

    for (size_t i = 0; i < stream.size(); i += chunkSize) {
        const size_t bytesToRead = std::min(chunkSize, stream.size() - i);
        receiver.Receive(stream.data() + i, bytesToRead);
    }

    REQUIRE(callback.packets.size() == 4);
    REQUIRE(callback.binary_count == 2);
    REQUIRE(callback.text_count == 2);

    REQUIRE(callback.packets[0].payload.size() == binaryData.size());
    REQUIRE(0 == memcmp(callback.packets[0].payload.data(), binaryData.data(), binaryData.size()));

    REQUIRE(callback.packets[2].payload.size() == binaryData.size());
    REQUIRE(0 == memcmp(callback.packets[2].payload.data(), binaryData.data(), binaryData.size()));

    REQUIRE(callback.packets[1].payload.size() == textData.size());
    REQUIRE(0 == memcmp(callback.packets[1].payload.data(), textData.data(), textData.size()));

    REQUIRE(callback.packets[3].payload.size() == textData.size());
    REQUIRE(0 == memcmp(callback.packets[3].payload.data(), textData.data(), textData.size()));
}
