#ifndef TankController_MotorControl_HXX
#define TankController_MotorControl_HXX

#include <array>

#include "ESP32Servo.h"

namespace TankController
{

class MotorControl
{
public:
	MotorControl();
	virtual ~MotorControl();

	void MoveTracks(int left_speed, int right_speed);
	void MovePitch(int pitch_speed);
	void MoveTurret(int turn_speed);
	void RelayCannon(bool enabled);
	bool RelayEnabled();

	void Reset();

	static int remap(int value, int start1, int stop1, int start2, int stop2);
private:		
	void SetServo(int channel, int speed);

private:

	struct Calibration
	{
		int MOVLMIN;
		int MOVLMAX;
		int MOVZERO;
		int MOVRMIN;
		int MOVRMAX;
		Calibration(int lmin, int lmax, int zero, int rmin, int rmax)
		 : MOVLMIN(lmin), MOVLMAX(lmax), MOVZERO(zero), MOVRMIN(rmin), MOVRMAX(rmax)
		 {}
	};

	const static int USMIN = 0;
	const static int USMAX = 1000000;

	const static int RELAY_CANNON = 11;
	const static int MOTOR_TURN_TURRET = 12;
	const static int MOTOR_PITCH_CANNON = 13;
	const static int MOTOR_MOVE_L = 14;
	const static int MOTOR_MOVE_R = 15;

	const static int CHANNEL_MIN = 11;
	const static int CHANNEL_MAX = 15;
	const static int CHANNEL_COUNT = CHANNEL_MAX - CHANNEL_MIN + 1;

	const static int ESC_ARM = 500;

	std::array<Calibration, CHANNEL_COUNT> calibration;

	std::array<ESP32Servo, CHANNEL_COUNT>  servo;
	bool relay_enabled;

	static int GetChannelIndex(int channel);

	ESP32Servo* GetServo(int channel);

};

}
#endif
