#ifndef TankController_LoRaComm_HXX
#define TankController_LoRaComm_HXX

#include <array>

#include "RadioHead/RH_RF95.h"
#include "RadioHead/RHEncryptedDriver.h"
#include "esp32blockciphers/ESP32AESBC.hpp"

namespace TankController
{

class LoRaComm
{
public:
	LoRaComm();
	bool initialized();
	void setup();
	bool send(uint8_t* data, uint8_t len);
	bool available();
	bool recv(uint8_t* buffer, uint8_t* len);

	static const size_t MAX_MESSAGE_LEN = ((int)(RH_RF95_MAX_MESSAGE_LEN / 16)) * 16;
private:
	bool is_initialized;
};

} // namespace TankController
#endif
