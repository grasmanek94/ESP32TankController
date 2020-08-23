#include <array>
#include <stdexcept>
#include <string>

#include "servo/ESP32Servo.h"

#include "PIDMotorControl.hpp"

namespace TankController
{

static PIDMotorControl* PIDMotorControl_ptr = nullptr;

PIDMotorControl::PIDMotorControl() : MotorControl(), left_tacho{}, right_tacho{}
{
	PIDMotorControl_ptr = this;

	pinMode(LEFT_TACHO_PIN, INPUT);
	pinMode(RIGHT_TACHO_PIN, INPUT);

	left_tacho.max_pulse_ps = LEFT_PULSE_MAX;
	right_tacho.max_pulse_ps = RIGHT_PULSE_MAX;

	attachInterrupt(RIGHT_TACHO_PIN,S_Right_Tick,CHANGE);
	attachInterrupt(LEFT_TACHO_PIN,S_Left_Tick,CHANGE);
}

PIDMotorControl::~PIDMotorControl()
{
	PIDMotorControl_ptr = nullptr;
}

void PIDMotorControl::MoveTracks(int speed_l, int speed_r)
{
	left_tacho.SetTargetSpeed(speed_l);
	right_tacho.SetTargetSpeed(speed_r);
}

void PIDMotorControl::Update()
{
	left_tacho.Update();
	right_tacho.Update();
	MotorControl::MoveTracks(left_tacho.target_speed, right_tacho.target_speed);
}

void IRAM_ATTR PIDMotorControl::S_Left_Tick()
{
	if(PIDMotorControl_ptr != nullptr)
	{
		PIDMotorControl_ptr->Left_Tick();
	}
}

void IRAM_ATTR PIDMotorControl::S_Right_Tick()
{
	if(PIDMotorControl_ptr != nullptr)
	{
		PIDMotorControl_ptr->Right_Tick();
	}
}

PIDMotorControl::Tacho::Tacho()
{
    update_delta = 200;
	ticks = 0;	
	last_update_time = millis();
	target_ticks = 0;
	target_speed = 0;
	direction = 0;
	max_speed_abs = 1000;
	max_speed_ps = 2000;
}

void PIDMotorControl::Tacho::Update()
{
	unsigned long time = millis();
	unsigned long threshold = last_update_time + update_delta;

	if(time < threshold)
	{
		return;
	}

	unsigned long delta = time - last_update_time;
	unsigned long local_ticks = ticks;

	ticks = 0;
	last_update_time = time;

	//unsigned long expected_max_ticks = max_pulse_ps * delta;
	//unsigned long expected_ticks = target_ticks * delta;
	//unsigned long actual_ticks = local_ticks * 1000;

	//long error = (expected_ticks - actual_ticks) / 1000;
	long error = target_ticks - local_ticks;

	long max_speed_in_update = max_speed_ps * delta / 1000;

	if(error == 0 || target_ticks == 0)
	{
		return;
	}

	long directional_error_percent = direction * error;

	Serial.println((
		String((int)this, HEX) + 
		": TT(" + String(target_ticks) + 
		") LT(" + String(local_ticks) + 
		") ER(" + String(error) + 
		") DT(" + String(delta) +
		") MSIU(" + String(max_speed_in_update) +
		") DEP(" + String(directional_error_percent) +
		")"
	).c_str());

	target_speed += directional_error_percent;
	if(target_speed > max_speed_abs || target_speed < -max_speed_abs)
	{
		target_speed = max_speed_abs * direction;
	}
}

void IRAM_ATTR PIDMotorControl::Left_Tick()
{
	left_tacho.ticks += 1;
}

void IRAM_ATTR PIDMotorControl::Right_Tick()
{
	right_tacho.ticks += 1;
}

void PIDMotorControl::Tacho::SetTargetSpeed(long speed)
{
	double abs_speed = (double)abs(speed);

	direction = ((speed < 0) ? -1 : (speed > 0 ? 1 : 0));

	update_delta = (unsigned long)(200.0 / (log10(abs_speed + 1.0) + 1.0));
	target_ticks = (abs_speed * max_pulse_ps) / 1000;
	
	if(speed == 0)
	{
		target_speed = 0;
		direction = 0;
	}
}

} // namespace TankController