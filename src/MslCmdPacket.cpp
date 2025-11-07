#include "MslCmdPacket.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

MslCmdPacket::MslCmdPacket() = default;

MslCmdPacket::MslCmdPacket(const HeaderPacket& hdr, char cmd)
    : header(hdr), commandType(cmd) {}

std::vector<uint8_t> MslCmdPacket::serialize() const {
    std::vector<uint8_t> buffer = header.serialize();
    buffer.push_back(static_cast<uint8_t>(commandType));
    return buffer;
}

MslCmdPacket MslCmdPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < MSL_CMD_PACKET_SIZE)
        throw std::runtime_error("Buffer too small for MslCmdPacket");

    HeaderPacket hdr = HeaderPacket::deserialize({buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE});
    char cmd = static_cast<char>(buffer[HEADER_PACKET_SIZE]);

    return MslCmdPacket(hdr, cmd);
}

void MslCmdPacket::print() const {
    header.print();
    std::cout << "MslCmdPacket(commandType=" << commandType << ")\n";
}

const HeaderPacket& MslCmdPacket::getHeader() const {
    return header;
}

char MslCmdPacket::getCommandType() const {
    return commandType;
}
