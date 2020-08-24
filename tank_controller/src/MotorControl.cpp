#include <array>
#include <stdexcept>

#include "servo/ESP32Servo.h"

#include "MotorControl.hpp"

namespace TankController
{

MotorControl::MotorControl() : relay_cannon{32, Calibration{USMAX, USMAX, 0, USMAX, USMAX}, {}},	  // Cannon
							   motor_turn_turret{33, Calibration{1386, 1435, 1480, 1528, 1577}, {}},  // Yaw Turret
							   motor_pitch_cannon{27, Calibration{1372, 1420, 1470, 1518, 1567}, {}}, // Pitch Turret
							   //motor_move_l{25, Calibration{800, 1450, 1500, 1550, 2100}, {}},		  // Left Engine
							   //motor_move_r{26, Calibration{800, 1450, 1500, 1550, 2100}, {}},		  // Right Engine
							   motor_move_l{25, Calibration{800, 1450, 1500, 1550, 2100}, {}},		  // Left Engine
							   motor_move_r{26, Calibration{800, 1450, 1500, 1575, 2100}, {}},		  // Right Engine
							   relay_enabled{false}
{
	pinMode(relay_cannon.pin, OUTPUT);

	AttachServo(motor_turn_turret);
	AttachServo(motor_pitch_cannon);
	AttachServo(motor_move_l);
	AttachServo(motor_move_r);

	Reset();
}

void MotorControl::Reset()
{
	SetServo(motor_turn_turret, 0);
	SetServo(motor_pitch_cannon, 0);
	SetServo(motor_move_r, 0);
	SetServo(motor_move_l, 0);

	RelayCannon(false);

	motor_move_l.servo.write(ESC_ARM);
	motor_move_r.servo.write(ESC_ARM);
}

void MotorControl::Update()
{

}

MotorControl::~MotorControl()
{
	Reset();
}

void MotorControl::MoveTracks(int speed_l, int speed_r)
{
	SetServo(motor_move_l, -speed_l);
	SetServo(motor_move_r, speed_r);
}

void MotorControl::MovePitch(int pitch_speed)
{
	SetServo(motor_pitch_cannon, pitch_speed);
}

void MotorControl::MoveTurret(int turn_speed)
{
	SetServo(motor_turn_turret, turn_speed);
}

void MotorControl::RelayCannon(bool enabled)
{
	digitalWrite(relay_cannon.pin, enabled);
	relay_enabled = enabled;
}

bool MotorControl::RelayEnabled()
{
	return relay_enabled;
}

int MotorControl::remap(int value, int input_min, int input_max, int output_min, int output_max)
{
	const long long factor = 1000000000;

	long long output_spread = output_max - output_min;
	long long input_spread = input_max - input_min;

	long long l_value = value;

	long long zero_value = value - input_min;
	zero_value *= factor;
	long long percentage = zero_value / input_spread;

	long long zero_output = percentage * output_spread / factor;

	long long result = output_min + zero_output;

	return (int)result;
}

void MotorControl::SetServo(function_info &info, int speed)
{
	int value = info.calibration.MOVZERO;

	if (speed < 0)
	{
		value = remap(speed, -1000, 0, info.calibration.MOVLMIN, info.calibration.MOVLMAX);
	}
	else if (speed > 0)
	{
		value = remap(speed, 0, 1000, info.calibration.MOVRMIN, info.calibration.MOVRMAX);
	}

	info.servo.write(value);
}

void MotorControl::AttachServo(function_info &info)
{
	info.servo.attach(info.pin, -1, USMIN, USMAX, USMIN, USMAX);
}

} // namespace TankController