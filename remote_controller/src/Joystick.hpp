#ifndef TankController_Joystick_HXX
#define TankController_Joystick_HXX

#include <array>

#include "LoRaComm.hpp"

namespace TankController
{

class Joystick
{
public:
	static const int PING_TIME_MS = 16;

	Joystick();

	bool Update();

private:

	int next_ping_time;

	LoRaComm comms;

	bool CheckDevice();
};

} // namespace TankController
#endif