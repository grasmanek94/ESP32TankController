#include <array>
#include <bitset>

#include "LoRaComm.hpp"
#include "Joystick.hpp"

namespace TankController
{

struct LoRaData
{
	char start;
	std::bitset<(size_t)Joystick::Button::MAX> buttons;
	int axis[(size_t)Joystick::Axis::MAX];
	char end;

	LoRaData()
	{
		zero();
	}

	void zero()
	{
		memset(this, 0, sizeof(LoRaData));
	}
} __attribute__ ((aligned (1)));

static_assert(sizeof(LoRaData) <= LoRaComm::MAX_MESSAGE_LEN, "");
LoRaData data;

Joystick::Joystick() : button_state{}, button_pressed{}, button_released{},
					   axis_default{0, 0, AXIS_MIN, 0, 0, AXIS_MIN},
					   axis{axis_default}, axis_pressed{}, axis_released{},
					   next_ping_time{0}
{}

void Joystick::Update()
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

	button_pressed = std::array<bool, (size_t)Button::MAX>{};
	button_released = std::array<bool, (size_t)Button::MAX>{};

	std::array<bool, (size_t)Button::MAX> temp_button_state = button_state;
	std::array<int, (size_t)Axis::MAX> temp_axis = axis;

	if (!CheckDevice())
	{
		for (size_t i = 0; i < (size_t)Button::MAX; ++i)
		{
			button_released[i] = button_pressed[i] || temp_button_state[i] || button_state[i];
		}

		for (size_t i = 0; i < (size_t)Button::MAX; ++i)
		{
			axis_released[i] = axis_pressed[i] || (temp_axis[i] != axis_default[i]) || (axis[i] != axis_default[i]);
		}

		button_pressed = std::array<bool, (size_t)Button::MAX>{};
		button_state = std::array<bool, (size_t)Button::MAX>{};

		axis_pressed = std::array<bool, (size_t)Axis::MAX>{};
		axis = axis_default;
		return;
	}

	const uint8_t initial_size = sizeof(data);
	uint8_t size = initial_size;

	if(!update_available || !comms.recv(reinterpret_cast<uint8_t*>(&data), &size))
	{
		return;
	}

	if(size != initial_size || data.start != '{' || data.end != '}')
	{
		return;
	}

	for(size_t btn = 0; btn < (size_t)Button::MAX; ++btn)
	{
		temp_button_state[btn] = data.buttons[btn];
	}

	for(size_t axis = 0; axis < (size_t)Button::MAX; ++axis)
	{
		temp_axis[axis] = data.axis[axis];
	}	

	for (size_t i = 0; i < (size_t)Button::MAX; ++i)
	{
		button_released[i] = (button_state[i] && !temp_button_state[i]);
		button_pressed[i] = (!button_state[i] && temp_button_state[i]);
		button_state[i] = temp_button_state[i];
	}

	for (size_t i = 0; i < (size_t)Axis::MAX; ++i)
	{
		axis_released[i] = ((axis[i] != axis_default[i]) && (temp_axis[i] == axis_default[i]));
		axis_pressed[i] = ((axis[i] == axis_default[i]) && (temp_axis[i] != axis_default[i]));
		axis[i] = temp_axis[i];
	}
}

bool Joystick::CheckDevice()
{
	return next_ping_time > millis();
}

bool Joystick::GetPressed(Button button_number) const
{
	return button_pressed[(size_t)button_number];
}

bool Joystick::GetReleased(Button button_number) const
{
	return button_released[(size_t)button_number];
}

bool Joystick::GetState(Button button_number) const
{
	return button_state[(size_t)button_number];
}

bool Joystick::GetPressed(Axis axis_number) const
{
	return axis_pressed[(size_t)axis_number];
}

bool Joystick::GetReleased(Axis axis_number) const
{
	return axis_released[(size_t)axis_number];
}

bool Joystick::GetState(Axis axis_number) const
{
	return axis[(size_t)axis_number] != axis_default[(size_t)axis_number];
}

int Joystick::GetValue(Axis axis_number) const
{
	if (abs(axis[(size_t)axis_number]) < AXIS_DEADZONE)
	{
		return 0;
	}

	return axis[(size_t)axis_number];
}

} // namespace TankController