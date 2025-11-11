#include "MslCmdPacket.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

MslCmdPacket::MslCmdPacket() = default;

MslCmdPacket::MslCmdPacket(const HeaderPacket& hdr)
    : header(hdr){}

std::vector<uint8_t> MslCmdPacket::serialize() const {
    std::vector<uint8_t> buffer = header.serialize();
    return buffer;
}

MslCmdPacket MslCmdPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < MSL_CMD_PACKET_SIZE)
        throw std::runtime_error("Buffer too small for MslCmdPacket");

    HeaderPacket hdr = HeaderPacket::deserialize({buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE});

    return MslCmdPacket(hdr);
}

void MslCmdPacket::print() const {
    header.print();
}

const HeaderPacket& MslCmdPacket::getHeader() const {
    return header;
}

