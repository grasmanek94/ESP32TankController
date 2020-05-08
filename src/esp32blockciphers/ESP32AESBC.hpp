#pragma once

#include "BlockCipher.h"

class ESP32AESBC: public BlockCipher
{
public:
    ESP32AESBC();
    virtual ~ESP32AESBC();
    virtual size_t blockSize() const override;
    virtual size_t keySize() const override;

    virtual bool setKey(const uint8_t *key, size_t len) override;

    virtual void encryptBlock(uint8_t *output, const uint8_t *input) override;
    virtual void decryptBlock(uint8_t *output, const uint8_t *input) override;

    virtual void clear() override;
};