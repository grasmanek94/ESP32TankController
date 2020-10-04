#include <Arduino.h>

#include "BatteryMeter.hpp"

namespace TankController
{
	
void BatteryMeter::ISR()
{
	last_instance->last_zero_time = millis();
}

BatteryMeter::BatteryMeter(
	int cell_pin_1)
	: pin_1{cell_pin_1}, last_zero_time{0}
{
	last_instance = this;

	pinMode(pin_1, INPUT);
	attachInterrupt(pin_1, ISR, CHANGE);
}

BatteryMeter::~BatteryMeter()
{
}

bool BatteryMeter::CutOff()
{
	return (millis() - last_zero_time) > 2000;
}

} // namespace TruckController