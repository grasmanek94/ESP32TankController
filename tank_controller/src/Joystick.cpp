#include <array>
#include <bitset>
#include <vector>

#include "LoRaComm.hpp"
#include "Joystick.hpp"

namespace TankController
{

struct LoRaData
{
	char start;
	char buttons[(size_t)Joystick::Button::MAX];
	char axis[(size_t)Joystick::Axis::MAX];
	char end;

	static const size_t bits_in_byte = 8;
	static const size_t button_bytes = (size_t)Joystick::Button::MAX / bits_in_byte + 1;
	static const size_t axis_bytes = sizeof(axis);
	static const size_t serialized_bytes =
		sizeof(start) +
		button_bytes +
		axis_bytes +
		sizeof(end);

	LoRaData()
	{
		zero();
	}

	void zero()
	{
		memset(this, 0, sizeof(LoRaData));
	}

	std::vector<uint8_t> serialize()
	{
		std::vector<uint8_t> vec;
		std::bitset<bits_in_byte> bits;

		vec.push_back(start);

		for(int i = 0; i < button_bytes; ++i)
		{
			bits.reset();
			for(int b = 0; b < bits_in_byte; ++b)
			{
				const size_t button_index = i * bits_in_byte + b;
				if(button_index < sizeof(buttons))
				{
					bits[b] = buttons[button_index];
				}
			}
			vec.push_back(bits.to_ulong() & 0xFF);
		}

		for(int i = 0; i < sizeof(axis); ++i)
		{
			vec.push_back(*(((uint8_t*)axis) + i));
		}

		vec.push_back(end);

		return vec;
	}

	void deserialize(const uint8_t* data)
	{
		zero();
		
		std::bitset<bits_in_byte> bits;
		size_t data_index = 0;

		start = *(data + data_index);
		++data_index;

		for(int i = 0; i < button_bytes; ++i)
		{
			bits.reset();
			for(int b = 0; b < bits_in_byte; ++b)
			{
				const size_t button_index = i * bits_in_byte + b;
				if(button_index < sizeof(buttons))
				{
					buttons[button_index] = (*(data + data_index) & (1 << b)) > 0;
				}
			}
			++data_index;
		}

		for(int i = 0; i < axis_bytes; ++i)
		{
			*(((uint8_t*)axis) + i) = *(data + data_index);
			++data_index;
		}

		end = *(data + data_index);
		++data_index;
	}

} __attribute__ ((aligned (1)));

LoRaData data;
uint8_t buffer[LoRaData::serialized_bytes];

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

	const uint8_t initial_size = sizeof(buffer);
	uint8_t size = initial_size;

	if(!update_available || !comms.recv(buffer, &size))
	{
		return;
	}

	if(size != initial_size)
	{
		return;
	}

	data.deserialize(buffer);

	if(data.start != '{' || data.end != '}')
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