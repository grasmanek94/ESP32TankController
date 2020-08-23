#ifndef TankController_PIDMotorControl_HXX
#define TankController_PIDMotorControl_HXX

#include <array>

#include "servo/ESP32Servo.h"

#include "MotorControl.hpp"

namespace TankController
{

class PIDMotorControl: MotorControl
{
private:
	const static int RIGHT_TACHO = 12;
	const static int LEFT_TACHO = 14;
	// Max RPM / Gearbox Ratio / seconds in one minute * max pulses per rotation
	const static int PULSE_MAX = 32000 / 10 / 60 * 15;

public:
	PIDMotorControl();
	virtual ~PIDMotorControl();

	virtual void MoveTracks(int left_speed, int right_speed) override;
	virtual void Update();
};

} // namespace TankController
#endif
