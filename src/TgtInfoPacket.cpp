#include "TgtInfoPacket.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <Utils.hpp>
TgtInfoPacket::TgtInfoPacket() = default;

TgtInfoPacket::TgtInfoPacket(const HeaderPacket& hdr, const char* eid,
                             int32_t x_, int32_t y_, int32_t z_,
                             int16_t vx_, int16_t vy_, int16_t vz_,
                             char type_, uint32_t ts)
    : header(hdr), x(x_), y(y_), z(z_),
      vx(vx_), vy(vy_), vz(vz_), type(type_), timestamp(ts) {
    std::strncpy(id, eid, sizeof(id) - 1);
    id[4] = '\0';
}

std::vector<uint8_t> TgtInfoPacket::serialize() const {
    std::vector<uint8_t> buffer;
    auto hbuf = header.serialize();
    buffer.insert(buffer.end(), hbuf.begin(), hbuf.end());

    std::vector<uint8_t> body(TGT_INFO_BODY_SIZE);
    std::memcpy(&body[0], id, 4);
    body[4] = static_cast<uint8_t>(type);
    std::memcpy(&body[5], &x, sizeof(x));
    std::memcpy(&body[9], &y, sizeof(y));
    std::memcpy(&body[13], &z, sizeof(z));
    std::memcpy(&body[17], &vx, sizeof(vx));
    std::memcpy(&body[19], &vy, sizeof(vy));
    std::memcpy(&body[21], &vz, sizeof(vz));
    std::memcpy(&body[23], &timestamp, sizeof(timestamp));

    buffer.insert(buffer.end(), body.begin(), body.end());
    return buffer;
}

TgtInfoPacket TgtInfoPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < TGT_INFO_PACKET_SIZE)
        throw std::runtime_error("Buffer too small for TgtInfoPacket");

    HeaderPacket hdr = HeaderPacket::deserialize({buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE});
    TgtInfoPacket pkt;
    pkt.header = hdr;

    std::memcpy(pkt.id, &buffer[HEADER_PACKET_SIZE], 4);
    pkt.id[4] = '\0';
    pkt.type = static_cast<char>(buffer[HEADER_PACKET_SIZE + 4]);
    std::memcpy(&pkt.x, &buffer[HEADER_PACKET_SIZE + 5], sizeof(pkt.x));
    std::memcpy(&pkt.y, &buffer[HEADER_PACKET_SIZE + 9], sizeof(pkt.y));
    std::memcpy(&pkt.z, &buffer[HEADER_PACKET_SIZE + 13], sizeof(pkt.z));
    std::memcpy(&pkt.vx, &buffer[HEADER_PACKET_SIZE + 17], sizeof(pkt.vx));
    std::memcpy(&pkt.vy, &buffer[HEADER_PACKET_SIZE + 19], sizeof(pkt.vy));
    std::memcpy(&pkt.vz, &buffer[HEADER_PACKET_SIZE + 21], sizeof(pkt.vz));
    std::memcpy(&pkt.timestamp, &buffer[HEADER_PACKET_SIZE + 23], sizeof(pkt.timestamp));

    return pkt;
}

void TgtInfoPacket::print() const {
    std::cout << "[TgtInfoPacket]\n";
    header.print();
    std::cout << "Detection(id=" << id
              << ", type=" << type
              << ", x=" << x << ", y=" << y << ", z=" << z
              << ", vx=" << vx << ", vy=" << vy << ", vz=" << vz
              << ", timestamp=" << timestamp << " ms)\n";
}

const HeaderPacket& TgtInfoPacket::getHeader() const {
    return header;
}

target_state_t
TgtInfoPacket::getTargetState() {
    target_state_t tg;
    tg.r_t = {i32ToDouble(x),i32ToDouble(y),i32ToDouble(z)};
    tg.v_t = {i16ToDouble(vx),i16ToDouble(vy),i16ToDouble(vz)};
    tg.t = i32ToDouble(timestamp);
    
    return tg;
}