#ifndef TankController_PIDMotorControl_HXX
#define TankController_PIDMotorControl_HXX

#include <array>

#include "servo/ESP32Servo.h"

#include "MotorControl.hpp"

namespace TankController
{

class PIDMotorControl: public MotorControl
{
private:

	struct Tacho
	{
		long max_pulse_ps;
		long max_speed_ps;
		long max_speed_abs;
		unsigned long update_delta;
		unsigned long ticks;	
		unsigned long last_update_time;
		unsigned long target_ticks;
		long direction;
		long target_speed;
		
		Tacho();
		void Update();
		void SetTargetSpeed(long speed);
	};

	const static int RIGHT_TACHO_PIN = 12;
	const static int LEFT_TACHO_PIN = 14;
	// Max RPM / Gearbox Ratio / seconds in one minute * max pulses per rotation * both edges (rising+falling) = 32000 / 11 / 60 * 15 * 2
	const static int RIGHT_PULSE_MAX = 750 * 2;
	const static int LEFT_PULSE_MAX = 800 * 2;

	Tacho left_tacho;
	Tacho right_tacho;
public:
	PIDMotorControl();
	virtual ~PIDMotorControl();

	virtual void MoveTracks(int left_speed, int right_speed) override;
	virtual void Update() override;

	static IRAM_ATTR void S_Left_Tick();
	static IRAM_ATTR void S_Right_Tick();
	void IRAM_ATTR Left_Tick();
	void IRAM_ATTR Right_Tick();
};

} // namespace TankController
#endif
