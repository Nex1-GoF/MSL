#include "KeyPacket.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <iomanip>

KeyPacket::KeyPacket() : header() {
    std::memset(key, 0, KEY_BODY_SIZE);
}

KeyPacket::KeyPacket(const HeaderPacket& hdr, const uint8_t in_key[KEY_BODY_SIZE])
    : header(hdr)
{
    std::memcpy(key, in_key, KEY_BODY_SIZE);
}

std::vector<uint8_t> KeyPacket::serialize() const {
    std::vector<uint8_t> buffer;

    // 헤더 직렬화
    auto hbuf = header.serialize();
    buffer.insert(buffer.end(), hbuf.begin(), hbuf.end());

    // 32바이트 키 추가
    buffer.insert(buffer.end(), key, key + KEY_BODY_SIZE);

    return buffer;
}

KeyPacket KeyPacket::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < KEY_PACKET_SIZE) {
        throw std::runtime_error("Buffer too small for KeyPacket");
    }

    // 헤더 역직렬화
    HeaderPacket hdr = HeaderPacket::deserialize(
        {buffer.begin(), buffer.begin() + HEADER_PACKET_SIZE});

    KeyPacket pkt;
    pkt.header = hdr;

    // 32바이트 키 복사
    std::memcpy(pkt.key, &buffer[HEADER_PACKET_SIZE], KEY_BODY_SIZE);

    return pkt;
}

void KeyPacket::print() const {
    std::cout << "[KeyPacket]\n";
    header.print();
    std::cout << "SessionKey(32 bytes): ";
    for (size_t i = 0; i < KEY_BODY_SIZE; ++i) {
        std::cout << std::uppercase << std::hex
                  << std::setw(2) << std::setfill('0')
                  << static_cast<int>(key[i]);
    }
    std::cout << std::dec << "\n";
}

const HeaderPacket& KeyPacket::getHeader() const {
    return header;
}

void KeyPacket::getKey(uint8_t out[KEY_BODY_SIZE]) const {
    std::memcpy(out, key, KEY_BODY_SIZE);
}

void KeyPacket::setKey(const uint8_t in[KEY_BODY_SIZE]) {
    std::memcpy(key, in, KEY_BODY_SIZE);
}
