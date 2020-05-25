#ifndef TankController_LoRaData_HXX
#define TankController_LoRaData_HXX

#include <memory>

#include "PS4Lib/ps4.h"

#include "LoRaComm.hpp"

namespace TankController
{

struct LoRaData
{
	char start;
	ps4_button_t buttons;
	ps4_analog_t axis;
	char end;

	static const size_t button_bytes = sizeof(buttons);
	static const size_t axis_bytes = sizeof(axis);
	static const size_t data_bytes =
		sizeof(start) +
		button_bytes +
		axis_bytes +
		sizeof(end);
	static const size_t total_bytes = ((data_bytes + LoRaComm::BLOCK_SIZE) / LoRaComm::BLOCK_SIZE) * LoRaComm::BLOCK_SIZE; // pad for aes
	static const size_t pad_bytes = total_bytes - data_bytes;

	char padding[pad_bytes];

	LoRaData()
	{
		zero();
	}

	inline void zero()
	{
		memset(this, 0, sizeof(LoRaData));
	}

} __attribute__ ((aligned (1)));

} // namespace TankController
#endif
