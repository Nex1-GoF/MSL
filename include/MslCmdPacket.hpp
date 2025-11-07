#ifndef MSL_CMD_PACKET_HPP
#define MSL_CMD_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>

constexpr size_t MSL_CMD_BODY_SIZE = 1;
constexpr size_t MSL_CMD_PACKET_SIZE = HEADER_PACKET_SIZE + MSL_CMD_BODY_SIZE;

class MslCmdPacket {
private:
    HeaderPacket header;
    char commandType; 

public:
    MslCmdPacket();
    MslCmdPacket(const HeaderPacket& hdr, char cmd);

    std::vector<uint8_t> serialize() const;
    static MslCmdPacket deserialize(const std::vector<uint8_t>& buffer);
    void print() const;

    const HeaderPacket& getHeader() const;
    char getCommandType() const;
};

#endif
