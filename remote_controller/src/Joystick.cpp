#include <memory>

#include "PS4Lib/PS4Controller.h"

#include "LoRaComm.hpp"
#include "LoRaData.hpp"
#include "Joystick.hpp"

namespace TankController
{

Joystick::Joystick() : next_ping_time{0}
{}

bool Joystick::Update()
{
	unsigned long now = millis();

	if(now < next_ping_time)
	{
		return false;
	}

	next_ping_time = now + PING_TIME_MS;

	if(!comms.initialized())
	{
		comms.setup();
	}

	if (!CheckDevice())
	{
		return false;
	}

	static LoRaData data;

	data.zero();

	data.start = '{';

	memcpy((char*)&data.buttons, (char*)&PS4.data.button, sizeof(data.buttons));
	memcpy((char*)&data.axis, (char*)&PS4.data.analog, sizeof(data.axis));

	data.end = '}';

	comms.send((uint8_t*)&data, sizeof(data));
	comms.waitPacketSent();

	return true;
}

bool Joystick::CheckDevice()
{
	return PS4.isConnected();
}

} // namespace TankController