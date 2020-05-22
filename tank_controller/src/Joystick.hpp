#ifndef TankController_Joystick_HXX
#define TankController_Joystick_HXX

#include <array>

#include "LoRaComm.hpp"

namespace TankController
{

class Joystick
{
public:
	static const int AXIS_MIN = -127;
	static const int AXIS_MAX = 128;
	static const int AXIS_RANGE = AXIS_MAX - AXIS_MIN;
	static const int AXIS_DEADZONE = 5;
	static const int PING_TIME_MS = 50;

	enum class Button : size_t
	{
		CROSS = 0,
		CIRCLE = 1,
		TRIANGLE = 2,
		SQUARE = 3,
		L1 = 4,
		R1 = 5,
		SHARE = 6,
		OPTIONS = 7,
		HOME = 8,
		L3 = 9,
		R3 = 10,
		DPAD_UP = 11,
		DPAD_LEFT = 12,
		DPAD_RIGHT = 13,
		DPAD_DOWN = 14,
		TOUCHPAD = 15,
		DPAD_UP_RIGHT = 16,
		DPAD_UP_LEFT = 17,
		DPAD_DOWN_RIGHT = 18,
		DPAD_DOWN_LEFT = 19,
		MAX = 20
	};

	enum class Axis : size_t
	{
		LEFT_X = 0,
		LEFT_Y = 1,
		RIGHT_X = 2,
		RIGHT_Y = 3,
		L2 = 4,
		R2 = 5,
		MAX = 6
	};

	Joystick();

	void Update();

	bool GetPressed(Button button_number) const;
	bool GetReleased(Button button_number) const;
	bool GetState(Button button_number) const;
	bool GetPressed(Axis axis_number) const;
	bool GetReleased(Axis axis_number) const;
	bool GetState(Axis axis_number) const;
	int GetValue(Axis axis_number) const;
	bool CheckDevice();
	
private:
	std::array<bool, (size_t)Button::MAX> button_state;
	std::array<bool, (size_t)Button::MAX> button_pressed;
	std::array<bool, (size_t)Button::MAX> button_released;

	const std::array<int, (size_t)Axis::MAX> axis_default;

	std::array<int, (size_t)Axis::MAX> axis;
	std::array<bool, (size_t)Axis::MAX> axis_pressed;
	std::array<bool, (size_t)Axis::MAX> axis_released;

	int next_ping_time;

	LoRaComm comms;


};

} // namespace TankController
#endif