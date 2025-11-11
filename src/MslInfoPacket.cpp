#include "MslInfoPacket.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>

MslInfoPacket::MslInfoPacket() = default;

MslInfoPacket::MslInfoPacket(const HeaderPacket& hdr,
                             int32_t x, int32_t y, int16_t z,
                             int32_t vx, int32_t vy, int16_t vz,
                             int32_t pip_x, int32_t pip_y, int16_t pip_z,
                             uint32_t time, char f_status, uint8_t t_status)
    : header(hdr), pos_x(x), pos_y(y), pos_z(z),
      vel_x(vx), vel_y(vy), vel_z(vz),
      pip_x(pip_x), pip_y(pip_y), pip_z(pip_z),
      flight_time(time),
      flight_status(f_status), telemetry_status(t_status) {}

std::vector<uint8_t> MslInfoPacket::serialize() const {
    std::vector<uint8_t> buffer = header.serialize();

    std::vector<uint8_t> body(MSL_INFO_BODY_SIZE);
    std::memcpy(&body[0],  &pos_x, sizeof(pos_x));
    std::memcpy(&body[4],  &pos_y, sizeof(pos_y));
    std::memcpy(&body[8],  &pos_z, sizeof(pos_z));
    std::memcpy(&body[10], &vel_x, sizeof(vel_x));
    std::memcpy(&body[14], &vel_y, sizeof(vel_y));
    std::memcpy(&body[18], &vel_z, sizeof(vel_z));
    std::memcpy(&body[20], &pip_x, sizeof(pip_x));
    std::memcpy(&body[24], &pip_y, sizeof(pip_y));
    std::memcpy(&body[28], &pip_z, sizeof(pip_z));
    std::memcpy(&body[30], &flight_time, sizeof(flight_time));
    body[34] = static_cast<uint8_t>(flight_status);
    body[35] = telemetry_status;

    buffer.insert(buffer.end(), body.begin(), body.begin() + 36); // 총 36바이트 사용
    return buffer;
}

MslInfoPacket MslInfoPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < MSL_INFO_PACKET_SIZE)
        throw std::runtime_error("Buffer too small for MslInfoPacket");

    HeaderPacket hdr = HeaderPacket::deserialize({buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE});
    MslInfoPacket pkt;
    pkt.header = hdr;

    std::memcpy(&pkt.pos_x,  &buffer[HEADER_PACKET_SIZE + 0],  sizeof(pkt.pos_x));
    std::memcpy(&pkt.pos_y,  &buffer[HEADER_PACKET_SIZE + 4],  sizeof(pkt.pos_y));
    std::memcpy(&pkt.pos_z,  &buffer[HEADER_PACKET_SIZE + 8],  sizeof(pkt.pos_z));
    std::memcpy(&pkt.vel_x,  &buffer[HEADER_PACKET_SIZE + 10], sizeof(pkt.vel_x));
    std::memcpy(&pkt.vel_y,  &buffer[HEADER_PACKET_SIZE + 14], sizeof(pkt.vel_y));
    std::memcpy(&pkt.vel_z,  &buffer[HEADER_PACKET_SIZE + 18], sizeof(pkt.vel_z));
    std::memcpy(&pkt.pip_x,  &buffer[HEADER_PACKET_SIZE + 20], sizeof(pkt.pip_x));
    std::memcpy(&pkt.pip_y,  &buffer[HEADER_PACKET_SIZE + 24], sizeof(pkt.pip_y));
    std::memcpy(&pkt.pip_z,  &buffer[HEADER_PACKET_SIZE + 28], sizeof(pkt.pip_z));
    std::memcpy(&pkt.flight_time, &buffer[HEADER_PACKET_SIZE + 30], sizeof(pkt.flight_time));
    pkt.flight_status    = static_cast<char>(buffer[HEADER_PACKET_SIZE + 34]);
    pkt.telemetry_status = buffer[HEADER_PACKET_SIZE + 35];

    return pkt;
}

void MslInfoPacket::print() const {
    std::cout << "[MslInfoPacket]\n";
    header.print();
    std::cout << "Position(X=" << pos_x
              << ", Y=" << pos_y
              << ", Z=" << pos_z << ")\n"
              << "Velocity(Vx=" << vel_x
              << ", Vy=" << vel_y
              << ", Vz=" << vel_z << ")\n"
              << "PIP(X=" << pip_x
              << ", Y=" << pip_y
              << ", Z=" << pip_z << ")\n"
              << "FlightTime=" << flight_time << " ms, "
              << "Status=" << static_cast<int>(flight_status)
              << ", Telemetry=" << static_cast<int>(telemetry_status) << "\n";
}

const HeaderPacket& MslInfoPacket::getHeader() const {
    return header;
}
