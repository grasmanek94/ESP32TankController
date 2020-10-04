#include <Arduino.h>

#include "BatteryMeter.hpp"

namespace TankController
{
static BatteryMeter* last_instance;

void BatteryMeter::ISR()
{
	Serial.println("ISR");
	last_instance->last_zero_time = millis();
}

BatteryMeter::BatteryMeter(
	int cell_pin_1)
	: pin_1{cell_pin_1}, last_zero_time{0}
{
	last_instance = this;

	pinMode(pin_1, INPUT);
	attachInterrupt(pin_1, BatteryMeter::ISR, CHANGE);
}

BatteryMeter::~BatteryMeter()
{
	detachInterrupt(pin_1);
}

bool BatteryMeter::CutOff()
{
	return (digitalRead(pin_1) == 0) || ((millis() - last_zero_time) > 2000);
}

} // namespace TruckController