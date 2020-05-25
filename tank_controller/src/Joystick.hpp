#ifndef TankController_Joystick_HXX
#define TankController_Joystick_HXX

#include "PS4Lib/ps4.h"

#include "LoRaComm.hpp"

namespace TankController
{
typedef struct {
    int16_t lx;
    int16_t ly;
    int16_t rx;
    int16_t ry;
} ps4_analog_stick_change_t;

typedef struct {
    int16_t l2;
    int16_t r2;
} ps4_analog_button_change_t;

typedef struct {
    ps4_analog_stick_change_t stick;
    ps4_analog_button_change_t button;
} ps4_analog_change_t;

class Joystick
{
public:
	static const int16_t BUTTON_MIN = 0;
	static const int16_t BUTTON_MAX = 255;
	static const int16_t BUTTON_RANGE = BUTTON_MAX - BUTTON_MIN;

	static const int16_t AXIS_MIN = -127;
	static const int16_t AXIS_MAX = 128;
	static const int16_t AXIS_RANGE = AXIS_MAX - AXIS_MIN;

	static const int16_t AXIS_DEADZONE = 5;
	static const int PING_TIME_MS = 65;

	ps4_analog_t axis;
	ps4_button_t button_state;

	ps4_analog_change_t axis_change;
	ps4_button_t button_pressed;
	ps4_button_t button_released;

	Joystick();

	bool Update();

	bool CheckDevice();
	
	int16_t ApplyDeadzone(int16_t value) const;

private:

	int next_ping_time;

	LoRaComm comms;

	ps4_analog_t old_axis;
	ps4_button_t old_button_state;
};

} // namespace TankController
#endif