#include <vector>
#include <cstdint>
#include <cstring> // Для memcpy
#include <iostream>

class AMSPacket
{
private:
    uint32_t _indexGroup;
    uint32_t _indexOffset;
    uint32_t _length;

public:
    AMSPacket(uint32_t indexGroup, uint32_t indexOffset, uint32_t length) : _indexGroup(indexGroup), _indexOffset(indexOffset), _length(length) {}

    std::vector<uint8_t> create() // Read command packet
    {
        std::vector<uint8_t> request;

        // AMS/TCP Header
        uint8_t targetNetId[6] = {5, 114, 111, 44, 1, 1};
        std::memcpy(&request[0], targetNetId, 6);

        uint16_t targetPort = htons(801);
        std::memcpy(&request[6], &targetPort, 2);

        uint8_t sourceNetId[6] = {192, 168, 248, 165, 1, 1};
        std::memcpy(&request[8], sourceNetId, 6);

        uint16_t sourcePort = htons(33013);
        std::memcpy(&request[14], &sourcePort, 2);

        uint16_t commandId = htons(0x0002); // 0x0002 - Read command
        std::memcpy(&request[16], &commandId, 2);

        uint16_t flagState = htons(0x0004); // ADS command flag
        std::memcpy(&request[18], &flagState, 2);

        uint32_t dataLength = htonl(12); // 4 indexGroup + 4 indexOffset + 4 length
        std::memcpy(&request[20], &dataLength, 4);

        uint32_t errorCode = htonl(0);
        std::memcpy(&request[24], &errorCode, 4);

        uint32_t invokeId = htonl(0);
        std::memcpy(&request[28], &invokeId, 4);

        // ADS Data

        uint32_t indexGroup = htonl(_indexGroup);
        std::memcpy(&request[32], &indexGroup, 4);

        uint32_t indexOffset = htonl(_indexOffset);
        std::memcpy(&request[36], &indexOffset, 4);

        uint32_t length = htonl(_length);
        std::memcpy(&request[40], &length, 4);

        return request;
    }
};