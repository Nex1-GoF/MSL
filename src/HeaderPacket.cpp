//HeaderPacket.cpp
#include "HeaderPacket.hpp"
#include <stdexcept>

HeaderPacket::HeaderPacket(const char* s_id, const char* d_id, uint32_t s, uint8_t size)
    : seq(s), msg_size(size) {
    std::memcpy(src_id, s_id, 4);
    src_id[4] = '\0';  // 널 종료 보장

    std::memcpy(dest_id, d_id, 4);
    dest_id[4] = '\0';  // 널 종료 보장
}

std::vector<uint8_t> HeaderPacket::serialize() const {
    std::vector<uint8_t> buffer(SIZE);
    std::memcpy(&buffer[0], src_id, 4);
    std::memcpy(&buffer[4], dest_id, 4);
    buffer[8] = (seq >> 24) & 0xFF;
    buffer[9] = (seq >> 16) & 0xFF;
    buffer[10] = (seq >> 8) & 0xFF;
    buffer[11] = seq & 0xFF;
    buffer[12] = msg_size;
    return buffer;
}

HeaderPacket HeaderPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < SIZE) {
        throw std::invalid_argument("Buffer too small for HeaderPacket");
    }

    char s_id[5], d_id[5];
    std::memcpy(s_id, &buffer[0], 4);
    s_id[4] = '\0';  // 널 종료 보장

    std::memcpy(d_id, &buffer[4], 4);
    d_id[4] = '\0';  // 널 종료 보장

    uint32_t seq_val = (static_cast<uint32_t>(buffer[8]) << 24) |
                       (static_cast<uint32_t>(buffer[9]) << 16) |
                       (static_cast<uint32_t>(buffer[10]) << 8) |
                       static_cast<uint32_t>(buffer[11]);

    return HeaderPacket(s_id, d_id, seq_val, buffer[12]);
}

void HeaderPacket::print() const {
    std::cout << "HeaderPacket("
              << "src_id=" << src_id << ", "
              << "dest_id=" << dest_id << ", "
              << "seq=" << seq << ", "
              << "msg_size=" << static_cast<unsigned>(msg_size)
              << ")" << std::endl;
}

const char* HeaderPacket::getSrcId() const { return src_id; }
const char* HeaderPacket::getDestId() const { return dest_id; }
uint32_t HeaderPacket::getSeq() const { return seq; }
uint8_t HeaderPacket::getMsgSize() const { return msg_size; }
