#include <hwcrypto/aes.h>

#include "ESP32AESBC.hpp"

ESP32AESBC::ESP32AESBC()
{
}

ESP32AESBC::~ESP32AESBC()
{
}

size_t ESP32AESBC::blockSize() const
{
    return 0;
}

size_t ESP32AESBC::keySize() const
{
    return 0;
}

bool ESP32AESBC::setKey(const uint8_t *key, size_t len)
{
    return false;
}

void ESP32AESBC::encryptBlock(uint8_t *output, const uint8_t *input)
{
}

void ESP32AESBC::decryptBlock(uint8_t *output, const uint8_t *input)
{
}

void ESP32AESBC::clear()
{
}
/*
void encrypt_message(String msg)
{
    const unsigned char *const_str = (const unsigned char *)(msg.c_str());
    unsigned char key[16] = "0123456789ABCDE";
    char in[msg.length()];
    snprintf(in, sizeof(in), "%s", msg);
    unsigned char out[16];

    esp_aes_context ctx;
    esp_aes_setkey(&ctx, key, 128);                  //set key and choose 128Bit-Encryption
    esp_aes_encrypt(&ctx, (unsigned char *)in, out); //This function can only handle 16 chars
    Serial.println((char *)out);

    //encrypt the message
    size_t len = msg.length();
    unsigned char iv[16];
    size_t offset;
    unsigned char ciphertext[len]; //cast it for esp_aes_crypt_cfb128
    esp_aes_setkey(&ctx, key, 128);
    int encrypt_res = -1;
    encrypt_res = esp_aes_crypt_cfb128(&ctx, ESP_AES_ENCRYPT, len, &offset, iv, (unsigned char *)in, ciphertext);
    if (encrypt_res == 0)
    {
        Serial.println((char *)ciphertext);
    }
    else
    {
        Serial.println("Encryption FAILED");
    }
}*/