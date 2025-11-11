#include "TgtInfoPacket.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>

TgtInfoPacket::TgtInfoPacket()
    : header(), x(0), y(0), z(0), vx(0), vy(0), vz(0), detected_msl_time(0) {}

TgtInfoPacket::TgtInfoPacket(const HeaderPacket& hdr,
                             int32_t x_, int32_t y_, int16_t z_,
                             int32_t vx_, int32_t vy_, int16_t vz_,
                             uint32_t ts)
    : header(hdr), x(x_), y(y_), z(z_),
      vx(vx_), vy(vy_), vz(vz_), detected_msl_time(ts) {}

std::vector<uint8_t> TgtInfoPacket::serialize() const {
    std::vector<uint8_t> buffer;
    auto hbuf = header.serialize();
    buffer.insert(buffer.end(), hbuf.begin(), hbuf.end());

    std::vector<uint8_t> body(TGT_INFO_BODY_SIZE);
    std::memcpy(&body[0], &x, sizeof(x));
    std::memcpy(&body[4], &y, sizeof(y));
    std::memcpy(&body[8], &z, sizeof(z));
    std::memcpy(&body[10], &vx, sizeof(vx));
    std::memcpy(&body[14], &vy, sizeof(vy));
    std::memcpy(&body[18], &vz, sizeof(vz));
    std::memcpy(&body[20], &detected_msl_time, sizeof(detected_msl_time));

    buffer.insert(buffer.end(), body.begin(), body.end());
    return buffer;
}

TgtInfoPacket TgtInfoPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < TGT_INFO_PACKET_SIZE)
        throw std::runtime_error("Buffer too small for TgtInfoPacket");

    HeaderPacket hdr = HeaderPacket::deserialize(
        {buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE});

    TgtInfoPacket pkt;
    pkt.header = hdr;

    std::memcpy(&pkt.x, &buffer[HEADER_PACKET_SIZE], sizeof(pkt.x));
    std::memcpy(&pkt.y, &buffer[HEADER_PACKET_SIZE + 4], sizeof(pkt.y));
    std::memcpy(&pkt.z, &buffer[HEADER_PACKET_SIZE + 8], sizeof(pkt.z));
    std::memcpy(&pkt.vx, &buffer[HEADER_PACKET_SIZE + 10], sizeof(pkt.vx));
    std::memcpy(&pkt.vy, &buffer[HEADER_PACKET_SIZE + 14], sizeof(pkt.vy));
    std::memcpy(&pkt.vz, &buffer[HEADER_PACKET_SIZE + 18], sizeof(pkt.vz));
    std::memcpy(&pkt.detected_msl_time, &buffer[HEADER_PACKET_SIZE + 20],
                sizeof(pkt.detected_msl_time));

    return pkt;
}

void TgtInfoPacket::print() const {
    std::cout << "[TgtInfoPacket]\n";
    header.print();
    std::cout << "Detection(x=" << x << ", y=" << y << ", z=" << z
              << ", vx=" << vx << ", vy=" << vy << ", vz=" << vz
              << ", detected_msl_time=" << detected_msl_time << " ms)\n";
}

const HeaderPacket& TgtInfoPacket::getHeader() const {
    return header;
}

target_state_t
TgtInfoPacket::getTargetState() {
    target_state_t tg;
    tg.r_t = {i32ToDouble(x),i32ToDouble(y),i32ToDouble(z)};
    tg.v_t = {i16ToDouble(vx),i16ToDouble(vy),i16ToDouble(vz)};
    tg.t = i32ToDouble(detected_msl_time);
    
    return tg;
}
