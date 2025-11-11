#ifndef KEY_PACKET_HPP
#define KEY_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>
#include <cstddef>

constexpr size_t KEY_BODY_SIZE   = 32; // AES-256 session key (32 bytes)
constexpr size_t KEY_PACKET_SIZE = HEADER_PACKET_SIZE + KEY_BODY_SIZE;

class KeyPacket {
private:
    HeaderPacket header;
    uint8_t key[KEY_BODY_SIZE]; // 32-byte session key payload

public:
    KeyPacket();
    explicit KeyPacket(const HeaderPacket& hdr, const uint8_t in_key[KEY_BODY_SIZE]);

    // 직렬화/역직렬화
    std::vector<uint8_t> serialize() const;
    static KeyPacket deserialize(const std::vector<uint8_t>& buffer);

    // 출력/접근
    void print() const;
    const HeaderPacket& getHeader() const;

    // 편의 함수: 키 읽기/설정
    void getKey(uint8_t out[KEY_BODY_SIZE]) const;     // out 에 32바이트 복사
    void setKey(const uint8_t in[KEY_BODY_SIZE]);      // 내부 key를 32바이트로 설정
};

#endif // KEY_PACKET_HPP
