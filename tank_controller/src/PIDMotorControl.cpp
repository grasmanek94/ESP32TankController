#include <array>
#include <stdexcept>

#include "servo/ESP32Servo.h"

#include "PIDMotorControl.hpp"

namespace TankController
{

PIDMotorControl::PIDMotorControl() : MotorControl()
{

}

PIDMotorControl::~PIDMotorControl()
{
	Reset();
}

void PIDMotorControl::MoveTracks(int speed_l, int speed_r)
{
	MotorControl::MoveTracks(speed_l, speed_r);
}

} // namespace TankController