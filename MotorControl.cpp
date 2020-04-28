#include <array>
#include <stdexcept>

#include "ESP32Servo.h"

#include "MotorControl.hpp"

namespace TankController
{

/*

Calibration
ServoNum	LMIN	LMAX	RMIN	RMAX	MID
15			282		292		312		322		302
14			281		291		311		321		301
13			281		291		311		321		301
12			284		294		313		323		303.5

*/

MotorControl::MotorControl()
	: 
	calibration{
		Calibration{1000000,1000000,0,1000000,1000000}, // Cannon
		Calibration{1386,1435,1480,1528,1577}, // Yaw Turret
		Calibration{1372,1420,1470,1518,1567}, // Pitch Turret
		Calibration{800,1450,1500,1550,2100}, // Left Engine
		Calibration{800,1450,1500,1550,2100} // Right Engine
	},
	servo{}, relay_enabled{ false }
{
	GetServo(RELAY_CANNON)->attach(RELAY_CANNON, -1, USMIN, USMAX, USMIN, USMAX);
	GetServo(MOTOR_TURN_TURRET)->attach(MOTOR_TURN_TURRET, -1, USMIN, USMAX, USMIN, USMAX);
	GetServo(MOTOR_PITCH_CANNON)->attach(MOTOR_PITCH_CANNON, -1, USMIN, USMAX, USMIN, USMAX);
	GetServo(MOTOR_MOVE_L)->attach(MOTOR_MOVE_L, -1, USMIN, USMAX, USMIN, USMAX);
	GetServo(MOTOR_MOVE_R)->attach(MOTOR_MOVE_R, -1, USMIN, USMAX, USMIN, USMAX);

	Reset();
}

void MotorControl::Reset()
{
	SetServo(MOTOR_MOVE_L, 0);
	SetServo(MOTOR_MOVE_R, 0);
	SetServo(MOTOR_PITCH_CANNON, 0);
	SetServo(MOTOR_TURN_TURRET, 0);

	GetServo(MOTOR_MOVE_L)->write(ESC_ARM);
	GetServo(MOTOR_MOVE_R)->write(ESC_ARM);
}

MotorControl::~MotorControl()
{
	Reset();
}

void MotorControl::MoveTracks(int speed_l, int speed_r)
{
	SetServo(MOTOR_MOVE_L, -speed_l);
	SetServo(MOTOR_MOVE_R, speed_r);
}

void MotorControl::MovePitch(int pitch_speed)
{
	SetServo(MOTOR_PITCH_CANNON, pitch_speed);
}

void MotorControl::MoveTurret(int turn_speed)
{
	SetServo(MOTOR_TURN_TURRET, turn_speed);
}

void MotorControl::RelayCannon(bool enabled)
{
	SetServo(RELAY_CANNON, (enabled ? 1000 : 0));
	relay_enabled = enabled;
}

bool MotorControl::RelayEnabled()
{
	if (relay_enabled)
	{
		return true;
	}

	return GetServo(RELAY_CANNON)->read() != 0;
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

void MotorControl::SetServo(int channel, int speed)
{
	if (channel > CHANNEL_MAX || channel < CHANNEL_MIN)
	{
		return;
	}

	const Calibration& local_calibration = calibration[GetChannelIndex(channel)];

	int value = local_calibration.MOVZERO;

	if (speed < 0)
	{
		value = remap(speed, -1000, 0, local_calibration.MOVLMIN, local_calibration.MOVLMAX);
	}
	else if (speed > 0)
	{
		value = remap(speed, 0, 1000, local_calibration.MOVRMIN, local_calibration.MOVRMAX);
	}

	GetServo(channel)->write(value);
}

int MotorControl::GetChannelIndex(int channel)
{
	return channel - CHANNEL_MIN;
}

ESP32Servo* MotorControl::GetServo(int channel)
{
	return &servo[GetChannelIndex(channel)];
}

}