#include <Arduino.h>

#include "BatteryMeter.hpp"

#include "PS4Controller.h"

namespace TankController
{

BatteryMeter::BatteryMeter(int cell_pin_1, int cell_pin_2)
	: pin_1(cell_pin_1), pin_2(cell_pin_2)
{
	pinMode(pin_1, INPUT);
	pinMode(pin_2, INPUT);
}

BatteryMeter::~BatteryMeter()
{
}

bool BatteryMeter::Cell1Okay()
{
	// minimum lipo batt V is 3.2V, so lets use 3.25V / 3250 mV
	// 4.3V max total, resistors are 3.3:1 ratio (3.3+1=4.3)
	// Vref / Vmax * Vmin =
	// 3.3 / 4.3 * 3.25 = 2.494 V = 2494 mV
	// 1 mV = 4096/3300 read value = 1.241
	// 2494 * 1.241.... = 3095
	return analogRead(pin_1) > 3095;
}

bool BatteryMeter::Cell2Okay()
{
	return analogRead(pin_2) > 3095;
}

bool BatteryMeter::ControllerOkay()
{
	return PS4.isConnected() &&
		   (PS4.data.status.battery > 1 || PS4.data.status.charging);
}

bool BatteryMeter::AllOkay()
{
	return Cell1Okay() && Cell2Okay() && ControllerOkay();
}

} // namespace TankController