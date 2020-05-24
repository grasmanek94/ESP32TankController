#include <array>

#include "RadioHead/RH_RF95.h"
#include "RadioHead/RHEncryptedDriver.h"
#include "esp32blockciphers/ESP32AESBC.hpp"

#include "LoRaComm.hpp"

namespace TankController
{

const int slaveSelect = 5;
const int commInterrupt = 4;
const float frequency = 868.0;
const unsigned char encryptkey[16] = {'6', 'x', '3', 'L', 'h', '5', '=', 'g', 'p', 'h', 'Q', '[', '{', 'B', 'K', 'V'};

RH_RF95 rf95(slaveSelect, commInterrupt);
ESP32AESBC cipher;
RHEncryptedDriver driver(rf95, cipher);

void LoRaComm::setup()
{
    while (!rf95.init())
    {
        delay(10);
    }

    rf95.setFrequency(frequency);
    rf95.setTxPower(14);

    cipher.setKey(encryptkey, sizeof(encryptkey));

	is_initialized = true;
}

bool LoRaComm::send(uint8_t* data, uint8_t len)
{
	return driver.send(data, len);
}

bool LoRaComm::available()
{
	return driver.available();
}

bool LoRaComm::recv(uint8_t* buffer, uint8_t* len)
{
	return driver.recv(buffer, len);
}

bool LoRaComm::initialized()
{
	return is_initialized;
}

bool LoRaComm::waitPacketSent()
{
    return driver.waitPacketSent();
}

LoRaComm::LoRaComm()
	: is_initialized{false}
{}

} // namespace TankController