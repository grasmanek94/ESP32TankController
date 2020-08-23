#ifndef TankController_PIDMotorControl_HXX
#define TankController_PIDMotorControl_HXX

#include <array>

#include "servo/ESP32Servo.h"
#include "PID_v1.h"

#include "MotorControl.hpp"

namespace TankController
{

struct PIDMotorControlTacho
{
	const static unsigned long MAX_UPDATE_DELTA = 200; // ms
	
	PID pid;
	
	unsigned long update_delta;
	unsigned long ticks;	
	unsigned long last_update_time;

	long max_ticks_ps;
	long max_speed_ps;
	long max_speed_abs;
	long direction;
	long target_speed;

	double target_ticks_ps;
	double calculated_ticks_ps;
	double output;

	PIDMotorControlTacho();
	void Update();
	void SetTargetSpeed(long speed);
};

static PIDMotorControlTacho left_tacho;
static PIDMotorControlTacho right_tacho;

class PIDMotorControl: public MotorControl
{
private:

	const static int RIGHT_TACHO_PIN = 15;
	const static int LEFT_TACHO_PIN = 2;
	// Max RPM / Gearbox Ratio / seconds in one minute * max pulses per rotation * both edges (rising+falling) = 32000 / 11 / 60 * 15 * 2
	const static int RIGHT_PULSE_MAX = 750 * 2;
	const static int LEFT_PULSE_MAX = 800 * 2;

public:
	PIDMotorControl();
	virtual ~PIDMotorControl();

	virtual void MoveTracks(int left_speed, int right_speed) override;
	virtual void Update() override;
};

} // namespace TankController
#endif
