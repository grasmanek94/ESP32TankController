#include <vector>
#include <limits>

#include <cstdlib>

#include <hwcrypto/aes.h>

#include "ESP32AESBC.hpp"

ESP32AESBC::ESP32AESBC(ESP32AESKeySize size)
    : aes_context{}
{
    esp_aes_init(&aes_context);
    aes_context.key_bytes = (int)size;
}

ESP32AESBC::~ESP32AESBC()
{
}

size_t ESP32AESBC::blockSize() const
{
    return 16;
}

size_t ESP32AESBC::keySize() const
{
    return aes_context.key_bytes;
}

bool ESP32AESBC::setKey(const uint8_t *key, size_t len)
{
    if (key == nullptr || len != keySize())
    {
        return false;
    }

    return esp_aes_setkey(&aes_context, key, keySize() * 8) == 0;
}

void ESP32AESBC::encryptBlock(uint8_t *output, const uint8_t *input)
{
    esp_aes_encrypt(&aes_context, (unsigned char *)input, output);
}

void ESP32AESBC::decryptBlock(uint8_t *output, const uint8_t *input)
{
    esp_aes_decrypt(&aes_context, input, output);
}

void ESP32AESBC::clear()
{
    auto size = aes_context.key_bytes;
    esp_aes_init(&aes_context);
    aes_context.key_bytes = size;
}
