#ifndef TGT_INFO_PACKET_HPP
#define TGT_INFO_PACKET_HPP
#include "TargetStateManager.hpp"
#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>
#include <string>

constexpr size_t TGT_INFO_BODY_SIZE = 27;
constexpr size_t TGT_INFO_PACKET_SIZE = HEADER_PACKET_SIZE + TGT_INFO_BODY_SIZE;
    
class TgtInfoPacket {
private:

    HeaderPacket header;

    char id[5];         // 탐지체 ID (예: "E001")
    int32_t x;          // 전방 좌표 X
    int32_t y;          // 좌방 좌표 Y
    int32_t z;          // 천방 좌표 Z
    int16_t vx;         // 전방 속도 Vx
    int16_t vy;         // 좌방 속도 Vy
    int16_t vz;         // 천방 속도 Vz
    char type;          // 탐지체 구분
    uint32_t timestamp; // 표적탐지시간

public:
    TgtInfoPacket();
    TgtInfoPacket(const HeaderPacket& hdr, const char* eid,
                  int32_t x_, int32_t y_, int32_t z_,
                  int16_t vx_, int16_t vy_, int16_t vz_,
                  char type_, uint32_t ts);

    std::vector<uint8_t> serialize() const;
    static TgtInfoPacket deserialize(const std::vector<uint8_t>& buffer);
    void print() const;

    const HeaderPacket& getHeader() const;
    target_state_t getTargetState();
};


#endif