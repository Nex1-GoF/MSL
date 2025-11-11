#ifndef PIP_PACKET_HPP
#define PIP_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>

constexpr size_t PIP_BODY_SIZE   = sizeof(int32_t) * 3; // X, Y, Z
constexpr size_t PIP_PACKET_SIZE = HEADER_PACKET_SIZE + PIP_BODY_SIZE;

class PipPacket {
private:
    HeaderPacket header;

    int32_t x; // PIP 전방 X
    int32_t y; // PIP 좌방 Y
    int32_t z; // PIP 천방 Z

public:
    PipPacket();
    PipPacket(const HeaderPacket& hdr, int32_t x_, int32_t y_, int32_t z_);

    std::vector<uint8_t> serialize() const;
    static PipPacket deserialize(const std::vector<uint8_t>& buffer);

    void print() const;
    const HeaderPacket& getHeader() const;

    // 편의 접근자
    int32_t getX() const;
    int32_t getY() const;
    int32_t getZ() const;
};

#endif // PIP_PACKET_HPP
