#include "PipPacket.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>

PipPacket::PipPacket()
    : x(0), y(0), z(0) {}

PipPacket::PipPacket(const HeaderPacket& hdr, int32_t x_, int32_t y_, int32_t z_)
    : header(hdr), x(x_), y(y_), z(z_) {}

std::vector<uint8_t> PipPacket::serialize() const {
    std::vector<uint8_t> buffer;

    // 헤더
    auto hbuf = header.serialize();
    buffer.insert(buffer.end(), hbuf.begin(), hbuf.end());

    // 바디: X, Y, Z (각 4바이트)
    std::vector<uint8_t> body(PIP_BODY_SIZE);
    std::memcpy(&body[0],  &x, sizeof(x));
    std::memcpy(&body[4],  &y, sizeof(y));
    std::memcpy(&body[8],  &z, sizeof(z));

    buffer.insert(buffer.end(), body.begin(), body.end());
    return buffer;
}

PipPacket PipPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < PIP_PACKET_SIZE) {
        throw std::runtime_error("Buffer too small for PipPacket");
    }

    HeaderPacket hdr = HeaderPacket::deserialize(
        {buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE});

    PipPacket pkt;
    pkt.header = hdr;

    std::memcpy(&pkt.x, &buffer[HEADER_PACKET_SIZE + 0], sizeof(pkt.x));
    std::memcpy(&pkt.y, &buffer[HEADER_PACKET_SIZE + 4], sizeof(pkt.y));
    std::memcpy(&pkt.z, &buffer[HEADER_PACKET_SIZE + 8], sizeof(pkt.z));

    return pkt;
}

void PipPacket::print() const {
    std::cout << "[PipPacket]\n";
    header.print();
    std::cout << "PIP(X=" << x << ", Y=" << y << ", Z=" << z << ")\n";
}

const HeaderPacket& PipPacket::getHeader() const {
    return header;
}

int32_t PipPacket::getX() const { return x; }
int32_t PipPacket::getY() const { return y; }
int32_t PipPacket::getZ() const { return z; }

