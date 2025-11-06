#ifndef HEADER_PACKET_HPP
#define HEADER_PACKET_HPP

#include <cstdint>
#include <vector>
#include <iostream>
#include <cstring>

class HeaderPacket {
public:
    static constexpr size_t SIZE = 13;

    HeaderPacket(const char* s_id, const char* d_id, uint32_t s, uint8_t size);

    std::vector<uint8_t> serialize() const;
    static HeaderPacket deserialize(const std::vector<uint8_t>& buffer);
    void print() const;

    const char* getSrcId() const;
    const char* getDestId() const;
    uint32_t getSeq() const;
    uint8_t getMsgSize() const;

private:
    char src_id[5];
    char dest_id[5];
    uint32_t seq;
    uint8_t msg_size;
};

#endif
