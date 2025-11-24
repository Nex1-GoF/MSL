#include "SecurityHandler.hpp"
#include "HeaderPacket.hpp"
#include <openssl/aes.h>  

std::vector<uint8_t> SecurityHandler::addPkcs7Padding(const std::vector<uint8_t>& plain)
{
    std::vector<uint8_t> padded = plain;

    size_t n = padded.size();
    uint8_t padLen = 16 - (n % 16);
    if (padLen == 0) padLen = 16;

    padded.reserve(n + padLen);
    for (uint8_t i = 0; i < padLen; ++i) {
        padded.push_back(padLen);
    }

    return padded;
}


void SecurityHandler::aes256EcbEncrypt(const std::vector<uint8_t>& plain,
                                     std::vector<uint8_t>& cipher,
                                     const std::array<uint8_t, 32>& key)
{
    AES_KEY aesKey;
    AES_set_encrypt_key(key.data(), 256, &aesKey);

    std::vector<uint8_t> padded = addPkcs7Padding(plain);

    cipher.resize(padded.size());

    for (size_t i = 0; i < padded.size(); i += 16) {
        AES_encrypt(&padded[i], &cipher[i], &aesKey);
    }
}

void SecurityHandler::aes256EcbDecrypt(const std::vector<uint8_t>& cipher,
                                     std::vector<uint8_t>& plain,
                                     const std::array<uint8_t, 32>& key)
{
    AES_KEY aesKey;
    AES_set_decrypt_key(key.data(), 256, &aesKey);

    if (cipher.size() % 16 != 0) {
        plain = cipher;
        return;
    }

    std::vector<uint8_t> padded(cipher.size());

    for (size_t i = 0; i < cipher.size(); i += 16) {
        AES_decrypt(&cipher[i], &padded[i], &aesKey);
    }

    if (!padded.empty()) {
        uint8_t padLen = padded.back();
        if (padLen > 0 && padLen <= 16 && padLen <= padded.size()) {
            padded.resize(padded.size() - padLen);
        }
    }
    plain = padded;
}

std::vector<uint8_t> SecurityHandler::encryptPayload(
    const std::vector<uint8_t>& fullPacket,
    const std::array<uint8_t, 32>& key)
{
    if (fullPacket.size() <= HEADER_PACKET_SIZE)
        return fullPacket;

    std::vector<uint8_t> header(fullPacket.begin(),
                                fullPacket.begin() + HEADER_PACKET_SIZE);

    std::vector<uint8_t> body(fullPacket.begin() + HEADER_PACKET_SIZE,
                              fullPacket.end());

    std::vector<uint8_t> cipherBody;
    aes256EcbEncrypt(body, cipherBody, key);

    std::vector<uint8_t> result;
    result.reserve(header.size() + cipherBody.size());
    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), cipherBody.begin(), cipherBody.end());

    return result;
}

std::vector<uint8_t> SecurityHandler::decryptPayload(
    const std::vector<uint8_t>& fullPacket,
    const std::array<uint8_t, 32>& key)
{
    if (fullPacket.size() <= HEADER_PACKET_SIZE)
        return fullPacket;

    std::vector<uint8_t> header(fullPacket.begin(),
                                fullPacket.begin() + HEADER_PACKET_SIZE);

    std::vector<uint8_t> cipherBody(fullPacket.begin() + HEADER_PACKET_SIZE,
                                    fullPacket.end());

    std::vector<uint8_t> plainBody;
    aes256EcbDecrypt(cipherBody, plainBody, key);

    std::vector<uint8_t> result;
    result.reserve(header.size() + plainBody.size());
    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), plainBody.begin(), plainBody.end());

    return result;
}