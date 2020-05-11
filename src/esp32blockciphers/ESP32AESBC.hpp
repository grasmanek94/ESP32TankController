#pragma once

#include <vector>

#include <hwcrypto/aes.h>

#include "BlockCipher.h"

enum class ESP32AESKeySize
{
    S128 = 16,
    S192 = 24,
    S256 = 32
};

class ESP32AESBC : public BlockCipher
{
private:
    esp_aes_context aes_context;
    
public:
    ESP32AESBC(ESP32AESKeySize size = ESP32AESKeySize::S128);
    virtual ~ESP32AESBC();
    virtual size_t blockSize() const override;
    virtual size_t keySize() const override;

    virtual bool setKey(const uint8_t *key, size_t len) override;

    virtual void encryptBlock(uint8_t *output, const uint8_t *input) override;
    virtual void decryptBlock(uint8_t *output, const uint8_t *input) override;

    virtual void clear() override;
};