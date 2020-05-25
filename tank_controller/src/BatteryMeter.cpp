#include <Arduino.h>

#include "BatteryMeter.hpp"

namespace TankController
{

BatteryMeter::BatteryMeter(
	int cell_pin_1, int cell_pin_2,
	float r1_ohms, float r2_ohms, float r3_ohms,
	float r4_ohms, float ref_millivolts,
	float cutoff_millivolts, float resolution)
	: pin_1{cell_pin_1}, pin_2{cell_pin_2},
	R1{r1_ohms}, R2{r2_ohms}, R3{r3_ohms},
	R4{r4_ohms}, ref_mv{ref_millivolts}, 
	cutoff{cutoff_millivolts}, res{resolution},
	cache_c1{0.0f}, cache_c2{0.0f},
    cache_c1_time{0}, cache_c2_time{0}
{
	pinMode(pin_1, INPUT);
	pinMode(pin_2, INPUT);

	analogSetPinAttenuation(pin_1, ADC_11db);
	analogSetPinAttenuation(pin_2, ADC_11db);
}

BatteryMeter::~BatteryMeter()
{
}


bool BatteryMeter::CutOff()
{
	Serial.printf("%.2f V / %.2f V\r\n", GetCellMillivolts(0) / 1000.0f, GetCellMillivolts(1) / 1000.0f);
	return 
		GetCellMillivolts(0) < cutoff || 
		GetCellMillivolts(1) < cutoff;
}

float BatteryMeter::GetCellMillivolts(unsigned int cell)
{
	unsigned long now = millis();
	unsigned long& local_cache_validity = (cell == 0) ? cache_c1_time : cache_c2_time;
	float& cache_value = (cell == 0) ? cache_c1 : cache_c2;
	if(local_cache_validity >= now)
	{
		return cache_value;
	}

	float r_measure = (cell == 0) ? R3 : R1;
	float r_passive = (cell == 0) ? R4 : R2;

	unsigned int buffer = analogRead((cell == 0) ? pin_1 : pin_2);

	if(buffer >= 4095)
	{
		return 0.0f;
	}

	float diff = (cell == 0) ? 0.0f : GetCellMillivolts(0);

	float r_total = r_measure + r_passive;

	float pin_voltage = (float)buffer * ref_mv / res;

	float v_total = pin_voltage * r_total / r_measure;

	local_cache_validity = now + cache_validity;
	cache_value = v_total - diff;

	return cache_value;
}

} // namespace TruckController