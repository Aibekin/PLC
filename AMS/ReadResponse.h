#include <iostream>
#include <vector>
#include <cstring>

struct ReadResponse
{
    uint32_t result;
    uint32_t length;
    std::vector<uint8_t> data;

    static ReadResponse fromBytes(const std::vector<uint8_t> &buffer)
    {
        ReadResponse res;
        size_t offset = 0;

        std::memcpy(&res.result, buffer.data() + offset, sizeof(res.result));
        offset += sizeof(res.result);

        std::memcpy(&res.length, buffer.data() + offset, sizeof(res.length));
        offset += sizeof(res.length);

        res.data.resize(res.length);
        std::memcpy(res.data.data(), buffer.data() + offset, res.length);

        return res;
    }
};