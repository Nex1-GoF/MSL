#ifndef TGT_INFO_PACKET_HPP
#define TGT_INFO_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

constexpr size_t TGT_INFO_BODY_SIZE = 24; // id/type/timestamp 제거 후 새 필드 합계
constexpr size_t TGT_INFO_PACKET_SIZE = HEADER_PACKET_SIZE + TGT_INFO_BODY_SIZE;

class TgtInfoPacket {
private:
    HeaderPacket header;
    int32_t x;          // 전방 좌표 X
    int32_t y;          // 좌방 좌표 Y
    int16_t z;          // 천방 좌표 Z
    int32_t vx;         // 전방 속도 Vx
    int32_t vy;         // 좌방 속도 Vy
    int16_t vz;         // 천방 속도 Vz
    uint32_t detected_msl_time; // 표적탐지시간

public:
    TgtInfoPacket();
    TgtInfoPacket(const HeaderPacket& hdr,
                  int32_t x_, int32_t y_, int16_t z_,
                  int32_t vx_, int32_t vy_, int16_t vz_,
                  uint32_t ts);

    std::vector<uint8_t> serialize() const;
    static TgtInfoPacket deserialize(const std::vector<uint8_t>& buffer);
    void print() const;

    const HeaderPacket& getHeader() const;
};

#endif
