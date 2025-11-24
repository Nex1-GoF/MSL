#ifndef KEY_PACKET_HPP
#define KEY_PACKET_HPP

#include "HeaderPacket.hpp"
#include <cstdint>
#include <vector>
#include <array> // 필수: std::array
#include <cstddef>

constexpr size_t KEY_BODY_SIZE   = 32; // AES-256 session key (32 bytes)
constexpr size_t KEY_PACKET_SIZE = HEADER_PACKET_SIZE + KEY_BODY_SIZE;

// [중요] 시스템 전체에서 사용할 키 타입 정의
using KeyData = std::array<uint8_t, KEY_BODY_SIZE>;

class KeyPacket {
private:
    HeaderPacket header;
    KeyData key; // std::array<uint8_t, 32>

public:
    KeyPacket();
    
    // 생성자: 이제 배열 포인터가 아닌 std::array 레퍼런스를 받습니다.
    explicit KeyPacket(const HeaderPacket& hdr, const KeyData& in_key);

    // 직렬화/역직렬화
    std::vector<uint8_t> serialize() const;
    static KeyPacket deserialize(const std::vector<uint8_t>& buffer);

    // 출력/접근
    void print() const;
    const HeaderPacket& getHeader() const;

    // [수정됨] Getter: 인자로 받는 게 아니라, 값을 직접 리턴합니다.
    KeyData getKey() const; 

    // [수정됨] Setter: std::array를 인자로 받습니다.
    void setKey(const KeyData& in_key);
};

#endif // KEY_PACKET_HPP
