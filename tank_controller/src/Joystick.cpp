#include <array>

#include "LoRaComm.hpp"
#include "LoRaData.hpp"
#include "Joystick.hpp"

namespace TankController
{

LoRaData data;

Joystick::Joystick() :  axis{}, button_state{}, axis_change{}, 
						button_pressed{}, button_released{}, 
						next_ping_time{0}, comms{}, old_axis{}, old_button_state{}
{}

bool Joystick::Update()
{
	if(!comms.initialized())
	{
		comms.setup();
	}

	bool update_available = comms.available();
	if(update_available)
	{
		next_ping_time = millis() + PING_TIME_MS;
	}

	memset(&axis_change, 0, sizeof(axis_change));
	memset(&button_released, 0, sizeof(button_released));

	if (!CheckDevice())
	{
		#define RELEASE_BUTTON(which) (button_released.which = button_pressed.which || button_state.which)
		RELEASE_BUTTON(options);
		RELEASE_BUTTON(l3);
		RELEASE_BUTTON(r3);
		RELEASE_BUTTON(share);
		RELEASE_BUTTON(up);
		RELEASE_BUTTON(right);
		RELEASE_BUTTON(down);
		RELEASE_BUTTON(left);
		RELEASE_BUTTON(upright);
		RELEASE_BUTTON(upleft);
		RELEASE_BUTTON(downright);
		RELEASE_BUTTON(downleft);
		RELEASE_BUTTON(l2);
		RELEASE_BUTTON(r2);
		RELEASE_BUTTON(l1);
		RELEASE_BUTTON(r1);
		RELEASE_BUTTON(triangle);
		RELEASE_BUTTON(circle);
		RELEASE_BUTTON(cross);
		RELEASE_BUTTON(square);
		RELEASE_BUTTON(ps);
		RELEASE_BUTTON(touchpad);
		#undef RELEASE_BUTTON

		#define AXIS_UNCHANGE(which) (axis_change.which = -axis.which)
		AXIS_UNCHANGE(button.l2);
		AXIS_UNCHANGE(button.r2);
		AXIS_UNCHANGE(stick.lx);
		AXIS_UNCHANGE(stick.ly);
		AXIS_UNCHANGE(stick.rx);
		AXIS_UNCHANGE(stick.ry);
		#undef AXIS_UNCHANGE

		memset(&button_pressed, 0, sizeof(button_pressed));
		memset(&button_state, 0, sizeof(button_state));
		memset(&axis, 0, sizeof(axis));

		return true;
	}

	memset(&button_pressed, 0, sizeof(button_pressed));

	data.zero();

	uint8_t size = sizeof(data);
	if(!update_available || !comms.recv((uint8_t*)&data, &size))
	{
		return false;
	}

	if(size != LoRaData::total_bytes)
	{
		return false;
	}

	/*Serial.printf("%d %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
		millis(), data.buffer[0],data.buffer[1],data.buffer[2],data.buffer[3],data.buffer[4],
		data.buffer[5],data.buffer[6],data.buffer[7],data.buffer[8],data.buffer[9],data.buffer[10]
	);*/

	if(data.start != '{' || data.end != '}')
	{
		return false;
	}

	memcpy(&old_button_state, &button_state, sizeof(button_state));
	memcpy(&old_axis, &axis, sizeof(axis));
	memcpy(&button_state, &data.buttons, sizeof(button_state));
	memcpy(&axis, &data.axis, sizeof(axis));

	#define PROCESS_BUTTON(which) \
		{ \
			button_released.which = (!button_state.which && old_button_state.which); \
			button_pressed.which = (button_state.which && !old_button_state.which); \
		}

	PROCESS_BUTTON(options);
	PROCESS_BUTTON(l3);
	PROCESS_BUTTON(r3);
	PROCESS_BUTTON(share);
	PROCESS_BUTTON(up);
	PROCESS_BUTTON(right);
	PROCESS_BUTTON(down);
	PROCESS_BUTTON(left);
	PROCESS_BUTTON(upright);
	PROCESS_BUTTON(upleft);
	PROCESS_BUTTON(downright);
	PROCESS_BUTTON(downleft);
	PROCESS_BUTTON(l2);
	PROCESS_BUTTON(r2);
	PROCESS_BUTTON(l1);
	PROCESS_BUTTON(r1);
	PROCESS_BUTTON(triangle);
	PROCESS_BUTTON(circle);
	PROCESS_BUTTON(cross);
	PROCESS_BUTTON(square);
	PROCESS_BUTTON(ps);
	PROCESS_BUTTON(touchpad);

	#undef PROCESS_BUTTON

	#define PROCESS_AXIS(which) \
	{ \
    	axis_change.which = ((int16_t)axis.which) - ((int16_t)old_axis.which); \
	} 

	PROCESS_AXIS(button.l2);
	PROCESS_AXIS(button.r2);
	PROCESS_AXIS(stick.lx);
	PROCESS_AXIS(stick.ly);
	PROCESS_AXIS(stick.rx);
	PROCESS_AXIS(stick.ry);

	#undef PROCESS_AXIS

	return true;
}

bool Joystick::CheckDevice()
{
	return next_ping_time > millis();
}

int16_t Joystick::ApplyDeadzone(int16_t value) const
{
	if (abs(value) < AXIS_DEADZONE)
	{
		return 0;
	}

	return value;
}

} // namespace TankController