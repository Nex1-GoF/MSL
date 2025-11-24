#include "KeyPacket.hpp"
#include <iostream>

#include <stdexcept>
#include <iomanip>
#include <algorithm> // std::copy

KeyPacket::KeyPacket() : header() {
    // std::array는 fill()로 0 초기화
    key.fill(0);
}

// 생성자: 멤버 초기화 리스트를 사용해 바로 대입 (가장 효율적)
KeyPacket::KeyPacket(const HeaderPacket& hdr, const KeyData& in_key)
    : header(hdr), key(in_key)
{

}

std::vector<uint8_t> KeyPacket::serialize() const {
    std::vector<uint8_t> buffer;
    buffer.reserve(KEY_PACKET_SIZE); // 성능 최적화

    // 헤더 직렬화 및 추가
    auto hbuf = header.serialize();
    buffer.insert(buffer.end(), hbuf.begin(), hbuf.end());

    // 키 추가: array의 iterator 사용
    buffer.insert(buffer.end(), key.begin(), key.end());

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

    // 벡터의 데이터 -> pkt.key(std::array)로 복사
    auto keyStart = buffer.begin() + HEADER_PACKET_SIZE;
    std::copy(keyStart, keyStart + KEY_BODY_SIZE, pkt.key.begin());

    return pkt;
}

void KeyPacket::print() const {
    std::cout << "[KeyPacket]\n";
    header.print();
    std::cout << "SessionKey(32 bytes): ";
    // 범위 기반 for 문으로 깔끔하게 출력
    for (const auto& byte : key) {
        std::cout << std::uppercase << std::hex
                  << std::setw(2) << std::setfill('0')
                  << static_cast<int>(byte);
    }
    std::cout << std::dec << "\n";
}

const HeaderPacket& KeyPacket::getHeader() const {
    return header;
}

// Getter 구현: 단순히 멤버를 리턴하면 됨
KeyData KeyPacket::getKey() const {
    return key;
}

// Setter 구현: 대입 연산자로 처리
void KeyPacket::setKey(const KeyData& in_key) {
    key = in_key;
}
