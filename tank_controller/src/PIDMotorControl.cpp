#include <array>
#include <stdexcept>
#include <string>

#include "servo/ESP32Servo.h"
#include "PID_v1.h"

#include "PIDMotorControl.hpp"

namespace TankController
{

void IRAM_ATTR Left_Tick()
{
	left_tacho.ticks += 1;
}

void IRAM_ATTR Right_Tick()
{
	right_tacho.ticks += 1;
}

PIDMotorControl::PIDMotorControl() : 
	MotorControl()
{
	pinMode(LEFT_TACHO_PIN, INPUT);
	pinMode(RIGHT_TACHO_PIN, INPUT);

	left_tacho.max_ticks_ps = LEFT_PULSE_MAX;
	right_tacho.max_ticks_ps = RIGHT_PULSE_MAX;

	attachInterrupt(digitalPinToInterrupt(RIGHT_TACHO_PIN),Right_Tick,RISING);
	attachInterrupt(digitalPinToInterrupt(LEFT_TACHO_PIN),Left_Tick,RISING);
}

PIDMotorControl::~PIDMotorControl()
{

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

PIDMotorControlTacho::PIDMotorControlTacho():
	pid(&calculated_ticks_ps, &output, &target_ticks_ps, 1.0, 1.0, 0.0, DIRECT)
{
    update_delta = MAX_UPDATE_DELTA; // micros
	ticks = 0;	
	last_update_time = millis();

	max_ticks_ps = 1000;
	max_speed_ps = 2000;
	max_speed_abs = 1000;

	direction = 0;
	target_speed = 0;

	target_ticks_ps = 0.0;
	calculated_ticks_ps = 0.0;
	output = 0.0;

	pid.SetSampleTime(25);
	pid.SetOutputLimits(0.0, 1000.0);
	pid.SetMode(AUTOMATIC);
}

void PIDMotorControlTacho::Update()
{
	unsigned long time = millis();
	unsigned long threshold = last_update_time + update_delta;

	if(time < threshold)
	{
		return;
	}

	double delta = ((double)time - (double)last_update_time) / 1000.0;
	double local_ticks = (double)ticks;

	ticks = 0;
	last_update_time = time;

	calculated_ticks_ps = local_ticks / delta;

	bool update = pid.Compute();

	double max_delta_speed = (double)max_speed_ps * delta;
	double delta_speed =  output;
	
	if(delta_speed > max_delta_speed)
	{
		delta_speed = max_delta_speed;
	}

	if(target_ticks_ps < 1.0 || direction == 0)
	{
		target_speed = 0;
	}
	else
	{
		target_speed += delta_speed * direction;

		if(abs(target_speed) > max_speed_abs)
		{
			target_speed = max_speed_abs * direction;
		}
	}

	Serial.println((
		String((int)this, HEX) + 
		": TT(" + String(target_ticks_ps) + 
		") CT(" + String(calculated_ticks_ps) + 
		") DT(" + String(delta) +
		") OUT(" + String(output) +
		") TS(" + String(target_speed) +
		") UPD(" + String(update) +
		")"
	).c_str());


}

void PIDMotorControlTacho::SetTargetSpeed(long speed)
{
	double abs_speed = (double)abs(speed);

	direction = ((speed < 0) ? -1 : (speed > 0 ? 1 : 0));

	update_delta = (unsigned long)((double)MAX_UPDATE_DELTA / (log10(abs_speed + 1.0) + 1.0));
	target_ticks_ps = (abs_speed * max_ticks_ps) / max_speed_abs;
	
	if(speed == 0)
	{
		target_speed = 0;
		direction = 0;
	}
}

} // namespace TankController