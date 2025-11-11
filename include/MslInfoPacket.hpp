#ifndef MSL_INFO_PACKET_HPP
#define MSL_INFO_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>
#include <string>

constexpr size_t MSL_INFO_BODY_SIZE = 36;
constexpr size_t MSL_INFO_PACKET_SIZE = HEADER_PACKET_SIZE + MSL_INFO_BODY_SIZE;

class MslInfoPacket {
private:
    HeaderPacket header;

    // 위치 좌표
    int32_t pos_x;   // 전방 좌표 X
    int32_t pos_y;   // 좌방 좌표 Y
    int16_t pos_z;   // 천방 좌표 Z

    // 속도
    int32_t vel_x;   // 전방 속도 Vx
    int32_t vel_y;   // 좌방 속도 Vy
    int16_t vel_z;   // 천방 속도 Vz

    // PIP 좌표
    int32_t pip_x;
    int32_t pip_y;
    int16_t pip_z;

    // 비행 정보
    uint32_t flight_time;   // ms
    char     flight_status;
    uint8_t  telemetry_status;

public:
     MslInfoPacket();
    MslInfoPacket(const HeaderPacket& hdr,
                  int32_t x, int32_t y, int16_t z,
                  int32_t vx, int32_t vy, int16_t vz,
                  int32_t pip_x, int32_t pip_y, int16_t pip_z,
                  uint32_t time, char f_status, uint8_t t_status);

    /// 직렬화
    std::vector<uint8_t> serialize() const;

    /// 역직렬화
    static MslInfoPacket deserialize(const std::vector<uint8_t>& buffer);

    /// 출력
    void print() const;

    /// 헤더 반환
    const HeaderPacket& getHeader() const;
};



#endif


