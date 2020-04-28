#include <array>

#include "PS4Controller.h"

#include "Joystick.hpp"

namespace TankController
{

Joystick::Joystick() :
	button_state{}, button_pressed{}, button_released{},
	axis_default{ 0, 0, AXIS_MIN, 0, 0, AXIS_MIN },
	axis{ axis_default }, axis_pressed{}, axis_released{}
{ }

void Joystick::Update()
{
	button_pressed = std::array<bool, (size_t)Button::MAX>{};
	button_released = std::array<bool, (size_t)Button::MAX>{};

	std::array<bool, (size_t)Button::MAX> temp_button_state = button_state;
	std::array<int, (size_t)Axis::MAX> temp_axis = axis;

	if(!CheckDevice())
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

	temp_button_state[(size_t)Button::DPAD_UP] = PS4.data.button.up;
	temp_button_state[(size_t)Button::DPAD_DOWN] = PS4.data.button.down;
	temp_button_state[(size_t)Button::DPAD_LEFT] = PS4.data.button.left;
	temp_button_state[(size_t)Button::DPAD_RIGHT] = PS4.data.button.right;

	temp_button_state[(size_t)Button::DPAD_UP_LEFT] = PS4.data.button.upleft;
	temp_button_state[(size_t)Button::DPAD_UP_RIGHT] = PS4.data.button.upright;
	temp_button_state[(size_t)Button::DPAD_DOWN_LEFT] = PS4.data.button.downleft;
	temp_button_state[(size_t)Button::DPAD_DOWN_RIGHT] = PS4.data.button.downright;

	temp_button_state[(size_t)Button::TRIANGLE] = PS4.data.button.triangle;
	temp_button_state[(size_t)Button::CIRCLE] = PS4.data.button.circle;
	temp_button_state[(size_t)Button::CROSS] = PS4.data.button.cross;
	temp_button_state[(size_t)Button::SQUARE] = PS4.data.button.square;

	temp_button_state[(size_t)Button::L1] = PS4.data.button.l1;
	temp_button_state[(size_t)Button::R1] = PS4.data.button.r1;

	temp_button_state[(size_t)Button::L3] = PS4.data.button.l3;
	temp_button_state[(size_t)Button::R3] = PS4.data.button.r3;

	temp_button_state[(size_t)Button::SHARE] = PS4.data.button.share;
	temp_button_state[(size_t)Button::OPTIONS] = PS4.data.button.options;

	temp_button_state[(size_t)Button::HOME] = PS4.data.button.ps;

	temp_button_state[(size_t)Button::TOUCHPAD] = PS4.data.button.touchpad;
		
	temp_axis[(size_t)Axis::L2] = PS4.data.analog.button.l2;
	temp_axis[(size_t)Axis::R2] = PS4.data.analog.button.r2;
	temp_axis[(size_t)Axis::LEFT_X] = PS4.event.analog_move.stick.lx;
	temp_axis[(size_t)Axis::LEFT_Y] = PS4.event.analog_move.stick.ly;
	temp_axis[(size_t)Axis::RIGHT_X] = PS4.event.analog_move.stick.rx;
	temp_axis[(size_t)Axis::RIGHT_Y] = PS4.event.analog_move.stick.ry;

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
	return PS4.isConnected();
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
	return axis[(size_t)axis_number];
}

}