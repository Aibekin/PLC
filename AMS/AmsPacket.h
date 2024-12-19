#include <cstdint>
#include <array>
#include <vector>
#include <cstring>
#include <iostream>

// AMS/TCP header
struct AmsTcpHeader
{
    const uint16_t reserved = 0x0000;
    uint32_t length;

    AmsTcpHeader() : length(50) {}

    void setLength(uint32_t len)
    {
        length = len;
    }
};

// AMS header
struct AmsHeader
{
    std::array<uint8_t, 6> targetNetId;
    uint16_t targetPort;
    std::array<uint8_t, 6> sourceNetId;
    uint16_t sourcePort;
    uint16_t commandId;
    uint16_t stateFlags;
    uint32_t dataLength;
    uint32_t errorCode;
    uint32_t invokeId;

    AmsHeader()
        : targetNetId({5, 114, 111, 44, 1, 1}), targetPort(801), sourceNetId({192, 168, 248, 165, 1, 1}), sourcePort(33013),
          commandId(0x0002), stateFlags(0x0004), dataLength(12), errorCode(0), invokeId(0) {}
};

// AMS/TCP packet
struct AMSPacket
{
    AmsTcpHeader tcpHeader;
    AmsHeader amsHeader;
    uint32_t _indexGroup;
    uint32_t _indexOffset;
    uint32_t _length;

    AMSPacket(uint32_t indexGroup, uint32_t indexOffset, uint32_t length) : tcpHeader(), amsHeader(),
                                                                            _indexGroup(indexGroup),
                                                                            _indexOffset(indexOffset),
                                                                            _length(length) {}

    std::vector<uint8_t> create() const
    {
        std::vector<uint8_t> buffer;
        std::vector<uint32_t> data = {_indexGroup, _indexOffset, _length};

        buffer.resize(sizeof(tcpHeader));
        std::memcpy(buffer.data(), &tcpHeader, sizeof(tcpHeader));

        size_t offset = buffer.size();
        buffer.resize(offset + sizeof(amsHeader));
        std::memcpy(buffer.data() + offset, &amsHeader, sizeof(amsHeader));

        buffer.insert(buffer.end(), data.begin(), data.end());

        return buffer;
    }
};