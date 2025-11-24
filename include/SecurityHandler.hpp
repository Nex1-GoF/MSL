#ifndef SECURITY_HANDLER_HPP
#define SECURITY_HANDLER_HPP

#include <vector>
#include <array>
#include <cstdint>

class SecurityHandler {
public:
    static std::vector<uint8_t> addPkcs7Padding(const std::vector<uint8_t>& plain);

    static void aes256EcbEncrypt(const std::vector<uint8_t>& plain,
                                 std::vector<uint8_t>& cipher,
                                 const std::array<uint8_t, 32>& key);

    static void aes256EcbDecrypt(const std::vector<uint8_t>& cipher,
                                 std::vector<uint8_t>& plain,
                                 const std::array<uint8_t, 32>& key);

    static std::vector<uint8_t> encryptPayload(const std::vector<uint8_t>& fullPacket,
                                               const std::array<uint8_t, 32>& key);

    static std::vector<uint8_t> decryptPayload(const std::vector<uint8_t>& fullPacket,
                                               const std::array<uint8_t, 32>& key);
};

#endif