#include <array>
#include <vector>
#include <bitset>

#include "PS4Lib/PS4Controller.h"

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

Joystick::Joystick() : next_ping_time{0}
{}

void Joystick::Update()
{
	unsigned long now = millis();

	if(now < next_ping_time)
	{
		return;
	}

	next_ping_time = now + PING_TIME_MS;

	if(!comms.initialized())
	{
		comms.setup();
	}

	if (!CheckDevice())
	{
		return;
	}

	data.start = '{';

	data.buttons[(size_t)Button::DPAD_UP] = PS4.data.button.up;
	data.buttons[(size_t)Button::DPAD_DOWN] = PS4.data.button.down;
	data.buttons[(size_t)Button::DPAD_LEFT] = PS4.data.button.left;
	data.buttons[(size_t)Button::DPAD_RIGHT] = PS4.data.button.right;

	data.buttons[(size_t)Button::DPAD_UP_LEFT] = PS4.data.button.upleft;
	data.buttons[(size_t)Button::DPAD_UP_RIGHT] = PS4.data.button.upright;
	data.buttons[(size_t)Button::DPAD_DOWN_LEFT] = PS4.data.button.downleft;
	data.buttons[(size_t)Button::DPAD_DOWN_RIGHT] = PS4.data.button.downright;

	data.buttons[(size_t)Button::TRIANGLE] = PS4.data.button.triangle;
	data.buttons[(size_t)Button::CIRCLE] = PS4.data.button.circle;
	data.buttons[(size_t)Button::CROSS] = PS4.data.button.cross;
	data.buttons[(size_t)Button::SQUARE] = PS4.data.button.square;

	data.buttons[(size_t)Button::L1] = PS4.data.button.l1;
	data.buttons[(size_t)Button::R1] = PS4.data.button.r1;

	data.buttons[(size_t)Button::L3] = PS4.data.button.l3;
	data.buttons[(size_t)Button::R3] = PS4.data.button.r3;

	data.buttons[(size_t)Button::SHARE] = PS4.data.button.share;
	data.buttons[(size_t)Button::OPTIONS] = PS4.data.button.options;

	data.buttons[(size_t)Button::HOME] = PS4.data.button.ps;

	data.buttons[(size_t)Button::TOUCHPAD] = PS4.data.button.touchpad;

	data.axis[(size_t)Axis::L2] = PS4.data.analog.button.l2;
	data.axis[(size_t)Axis::R2] = PS4.data.analog.button.r2;
	data.axis[(size_t)Axis::LEFT_X] = PS4.data.analog.stick.lx;
	data.axis[(size_t)Axis::LEFT_Y] = PS4.data.analog.stick.ly;
	data.axis[(size_t)Axis::RIGHT_X] = PS4.data.analog.stick.rx;
	data.axis[(size_t)Axis::RIGHT_Y] = PS4.data.analog.stick.ry;

	data.end = '}';

	std::vector<uint8_t> vec = data.serialize();
	comms.send(vec.data(), vec.size());
	comms.waitPacketSent();
}

bool Joystick::CheckDevice()
{
	return PS4.isConnected();
}

} // namespace TankController