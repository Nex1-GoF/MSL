#ifndef MSL_INFO_PACKET_HPP
#define MSL_INFO_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>
#include <string>

constexpr size_t MSL_INFO_BODY_SIZE = 30;
constexpr size_t MSL_INFO_PACKET_SIZE = HEADER_PACKET_SIZE + MSL_INFO_BODY_SIZE;

class MslInfoPacket {
private:
    HeaderPacket header_;
    int32_t msl_x_;     // 위도 (×1e7)
    int32_t msl_y_;    // 경도 (×1e7)
    int32_t msl_z_;     // 고도 (m)
    int32_t pip_x_;          // 요 (0.01°)
    int32_t pip_y_;        // 피치 (0.01°)
    int32_t pip_z_;
    uint32_t flight_time_; // 비행시간 (ms)
    char flight_status_;   // 비행상태 (1~5)
    char telemetry_status_;// 텔레메트리 상태 (1~3)

public:
    MslInfoPacket();
    MslInfoPacket(const HeaderPacket& hdr,
                  int32_t msl_x, int32_t msl_y, int32_t msl_z,
                  int32_t pip_x, int32_t pip_y, int32_t pip_z,
                  uint32_t time, char f_status, char t_status);

    std::vector<uint8_t> serialize() const;
    static MslInfoPacket deserialize(const std::vector<uint8_t>& buffer);
    void print() const;

    const HeaderPacket& getHeader() const;
};

#endif
