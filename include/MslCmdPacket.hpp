#ifndef MSL_CMD_PACKET_HPP
#define MSL_CMD_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>

constexpr size_t MSL_CMD_PACKET_SIZE = HEADER_PACKET_SIZE;

class MslCmdPacket {
private:
    HeaderPacket header;

public:
    MslCmdPacket();
    MslCmdPacket(const HeaderPacket& hdr);

    std::vector<uint8_t> serialize() const;
    static MslCmdPacket deserialize(const std::vector<uint8_t>& buffer);
    void print() const;

    const HeaderPacket& getHeader() const;
};

#endif
