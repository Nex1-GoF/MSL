#include "MslInfoPacket.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>

namespace {
    // 새 멤버 레이아웃 기준 바디 크기(바이트)
    constexpr size_t kBodySizeExpected = 
        sizeof(int32_t) * 6  // msl_x, msl_y, msl_z, pip_x, pip_y, pip_z
      + sizeof(uint32_t)     // flight_time
      + sizeof(char)         // flight_status
      + sizeof(char);        // telemetry_status

    // 필드별 오프셋(HEADER 뒤가 아니라 BODY 시작 기준)
    constexpr size_t OFF_MSL_X   = 0;
    constexpr size_t OFF_MSL_Y   = OFF_MSL_X   + sizeof(int32_t);
    constexpr size_t OFF_MSL_Z   = OFF_MSL_Y   + sizeof(int32_t);
    constexpr size_t OFF_PIP_X   = OFF_MSL_Z   + sizeof(int32_t);
    constexpr size_t OFF_PIP_Y   = OFF_PIP_X   + sizeof(int32_t);
    constexpr size_t OFF_PIP_Z   = OFF_PIP_Y   + sizeof(int32_t);
    constexpr size_t OFF_FTIME   = OFF_PIP_Z   + sizeof(int32_t);
    constexpr size_t OFF_FSTAT   = OFF_FTIME   + sizeof(uint32_t);
    constexpr size_t OFF_TSTAT   = OFF_FSTAT   + sizeof(char);
    constexpr size_t BODY_SIZE   = OFF_TSTAT   + sizeof(char); // = 30
}

MslInfoPacket::MslInfoPacket() = default;

MslInfoPacket::MslInfoPacket(const HeaderPacket& hdr,
                             int32_t msl_x, int32_t msl_y, int32_t msl_z,
                             int32_t pip_x, int32_t pip_y, int32_t pip_z,
                             uint32_t time, char f_status, char t_status)
    : header_(hdr),
      msl_x_(msl_x), msl_y_(msl_y), msl_z_(msl_z),
      pip_x_(pip_x), pip_y_(pip_y), pip_z_(pip_z),
      flight_time_(time),
      flight_status_(f_status),
      telemetry_status_(t_status) {}

/**
 * 직렬화 규칙 (Host endian 그대로 memcpy – 기존 규칙과 동일)
 * BODY(30B) = msl_x(4) msl_y(4) msl_z(4) pip_x(4) pip_y(4) pip_z(4)
 *             flight_time(4) flight_status(1) telemetry_status(1)
 */
std::vector<uint8_t> MslInfoPacket::serialize() const {
    // 안전: 헤더의 매크로와 실제 레이아웃이 불일치하면 경고/예외 유발
    if (MSL_INFO_BODY_SIZE != kBodySizeExpected) {
        // 개발 단계에서 빨리 드러나게 예외 처리
        throw std::logic_error("MSL_INFO_BODY_SIZE mismatch: header says " 
            + std::to_string(MSL_INFO_BODY_SIZE) + ", but layout needs " 
            + std::to_string(kBodySizeExpected));
    }

    // 1) 헤더 직렬화
    std::vector<uint8_t> buffer = header_.serialize();

    // 2) 바디(30B) 직렬화
    std::vector<uint8_t> body(BODY_SIZE);

    std::memcpy(&body[OFF_MSL_X], &msl_x_, sizeof(msl_x_));
    std::memcpy(&body[OFF_MSL_Y], &msl_y_, sizeof(msl_y_));
    std::memcpy(&body[OFF_MSL_Z], &msl_z_, sizeof(msl_z_));

    std::memcpy(&body[OFF_PIP_X], &pip_x_, sizeof(pip_x_));
    std::memcpy(&body[OFF_PIP_Y], &pip_y_, sizeof(pip_y_));
    std::memcpy(&body[OFF_PIP_Z], &pip_z_, sizeof(pip_z_));

    std::memcpy(&body[OFF_FTIME], &flight_time_, sizeof(flight_time_));
    body[OFF_FSTAT] = static_cast<uint8_t>(flight_status_);
    body[OFF_TSTAT] = static_cast<uint8_t>(telemetry_status_);

    // 3) 헤더 뒤에 바디 붙이기
    buffer.insert(buffer.end(), body.begin(), body.end());
    return buffer;
}

MslInfoPacket MslInfoPacket::deserialize(const std::vector<uint8_t>& buffer) {
    // 안전: 헤더 매크로와 실제 레이아웃 불일치 시 조기 검출
    if (MSL_INFO_BODY_SIZE != kBodySizeExpected) {
        throw std::logic_error("MSL_INFO_BODY_SIZE mismatch: header says " 
            + std::to_string(MSL_INFO_BODY_SIZE) + ", but layout needs " 
            + std::to_string(kBodySizeExpected));
    }

    // 전체 길이 검사: HEAD + BODY(30)
    const size_t need_min = HEADER_PACKET_SIZE + kBodySizeExpected;
    if (buffer.size() < need_min)
        throw std::runtime_error("Buffer too small for MslInfoPacket");

    // 1) 헤더 역직렬화
    HeaderPacket hdr = HeaderPacket::deserialize(
        { buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE });

    // 2) BODY 시작 위치
    const size_t base = HEADER_PACKET_SIZE;

    // 3) 바디 필드 추출
    MslInfoPacket pkt;
    pkt.header_ = hdr;

    std::memcpy(&pkt.msl_x_, &buffer[base + OFF_MSL_X], sizeof(pkt.msl_x_));
    std::memcpy(&pkt.msl_y_, &buffer[base + OFF_MSL_Y], sizeof(pkt.msl_y_));
    std::memcpy(&pkt.msl_z_, &buffer[base + OFF_MSL_Z], sizeof(pkt.msl_z_));

    std::memcpy(&pkt.pip_x_, &buffer[base + OFF_PIP_X], sizeof(pkt.pip_x_));
    std::memcpy(&pkt.pip_y_, &buffer[base + OFF_PIP_Y], sizeof(pkt.pip_y_));
    std::memcpy(&pkt.pip_z_, &buffer[base + OFF_PIP_Z], sizeof(pkt.pip_z_));

    std::memcpy(&pkt.flight_time_, &buffer[base + OFF_FTIME], sizeof(pkt.flight_time_));
    pkt.flight_status_    = static_cast<char>(buffer[base + OFF_FSTAT]);
    pkt.telemetry_status_ = static_cast<char>(buffer[base + OFF_TSTAT]);

    return pkt;
}

void MslInfoPacket::print() const {
    std::cout << "[MslInfoPacket]\n";
    header_.print();
    std::cout
        << "MSL(x=" << msl_x_
        << ", y="   << msl_y_
        << ", z="   << msl_z_ << ")\n"
        << "PIP(x=" << pip_x_
        << ", y="   << pip_y_
        << ", z="   << pip_z_ << ")\n"
        << "FlightTime=" << flight_time_ << " ms, "
        << "Status="     << static_cast<int>(flight_status_) << ", "
        << "Telemetry="  << static_cast<int>(telemetry_status_) << "\n";
}

const HeaderPacket& MslInfoPacket::getHeader() const {
    return header_;
}
