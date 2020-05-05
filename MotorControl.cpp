#include <array>
#include <stdexcept>

#include "ESP32Servo.h"

#include "MotorControl.hpp"

namespace TankController
{

/*

Calibration @ 50Hz / 12-bits [uint16_t]
ServoNum	LMIN	LMAX	RMIN	RMAX	MID
15			282		292		312		322		302
14			281		291		311		321		301
13			281		291		311		321		301
12			284		294		313		323		303.5

*/

MotorControl::MotorControl() : relay_cannon{14, Calibration{USMAX, USMAX, 0, USMAX, USMAX}, {}},	  // Cannon
							   motor_turn_turret{15, Calibration{1386, 1435, 1480, 1528, 1577}, {}},  // Yaw Turret
							   motor_pitch_cannon{27, Calibration{1372, 1420, 1470, 1518, 1567}, {}}, // Pitch Turret
							   motor_move_l{32, Calibration{800, 1450, 1500, 1550, 2100}, {}},		  // Left Engine
							   motor_move_r{33, Calibration{800, 1450, 1500, 1550, 2100}, {}},		  // Right Engine
							   relay_enabled{false}
{
	pinMode(relay_cannon.pin, OUTPUT);
	gpio_set_drive_capability((gpio_num_t)relay_cannon.pin, GPIO_DRIVE_CAP_MAX);
	
	Serial.printf("RELAY PIN: %d\r\n", relay_cannon.pin);

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

int MotorControl::remap(int value, int start1, int stop1, int start2, int stop2)
{
	float fvalue = value;
	float fstart1 = start1;
	float fstop1 = stop1;
	float fstart2 = start2;
	float fstop2 = stop2;

	return (int)(fstart2 + (fstop2 - fstart2) * ((fvalue - fstart1) / (fstop1 - fstart1)));
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