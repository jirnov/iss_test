#include <PacketReceiver.h>
#include <Utils.h>
#include <iostream>

using namespace std;

struct Printer : ICallback {
    void BinaryPacket(const char* data, std::size_t size)
    {
        std::cout << "binary packet size = " << size << "\n";
        std::string s(data, size);
        std::cout << "binary data = \"" << s << "\"\n";
    }

    void TextPacket(const char* data, std::size_t size)
    {
        std::cout << "text packet size = " << size << "\n";
        std::string s(data, size);
        std::cout << "text data = \"" << s << "\"\n";
    }
};

int main()
{
    Printer printer;
    PacketReceiver receiver(printer);

    std::vector<char> stream;

    std::string textData = "text";
    auto textPacket = createTextPacket(textData);
    stream.insert(stream.end(), textPacket.begin(), textPacket.end());

    std::vector binaryData = {'b', 'i', 'n', 'a', 'r', 'y'};
    auto binaryPacket = createBinaryPacket(binaryData);
    stream.insert(stream.end(), binaryPacket.begin(), binaryPacket.end());

    const size_t chunkSize = 4;

    for (size_t i = 0; i < stream.size(); i += chunkSize) {
        const size_t bytesToRead = std::min(chunkSize, stream.size() - i);
        receiver.Receive(stream.data() + i, bytesToRead);
    }

    return 0;
}
