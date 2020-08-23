#ifndef TankController_MotorControl_HXX
#define TankController_MotorControl_HXX

#include <array>

#include "servo/ESP32Servo.h"

namespace TankController
{

class MotorControl
{
public:
	MotorControl();
	virtual ~MotorControl();

	virtual void MoveTracks(int left_speed, int right_speed);
	virtual void MovePitch(int pitch_speed);
	virtual void MoveTurret(int turn_speed);
	virtual void RelayCannon(bool enabled);
	virtual bool RelayEnabled();

	virtual void Reset();

	static int remap(int value, int start1, int stop1, int start2, int stop2);

protected:

	const static int USMIN = 0;
	const static int USMAX = 1000000;

	const static int ESC_ARM = 500;

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
		 
		bool IsUsMax()
		{
			return MOVLMIN == USMAX && MOVLMAX == USMAX && MOVRMIN == USMAX && MOVRMAX == USMAX;
		}
	};

	struct function_info
	{
		int pin;
		Calibration calibration;
		ESP32Servo servo;
	};

	virtual void SetServo(function_info& info, int speed);
	virtual void AttachServo(function_info& info);

	function_info relay_cannon;
	function_info motor_turn_turret;
	function_info motor_pitch_cannon;
	function_info motor_move_l;
	function_info motor_move_r;

	bool relay_enabled;
};

} // namespace TankController
#endif
