#ifndef TankController_BatteryMeter_HXX
#define TankController_BatteryMeter_HXX

namespace TankController
{

class BatteryMeter
{
	/*
                           +------/\/\/\/-----+-----/\/\/\/-----+
                           |      R2=6100     |     R1=3780     |
                           |                  |                 |
                           |                  +                 |
                           |                 A1                 |
                  +   +---------+                               |
                  -      +---+                                  |
                           |                                    |
               3.3-4.2V    |                                    |
                           +------/\/\/\/-----+-----/\/\/\/-----+
                           |      R4=2316     |     R3=7720     |
                           |                  |                 |
                  +   +----+----+             +                 |
                                             A0                 |
                  -      +-+-+                                  |
                           |                                    |
               3.3-4.2V    |                                    |
                           |                                    |
                           +                                    +
                           |                                    |
                           |                                    |
                           |   GND (BATT)                       |    GND (ESP32)
                        +-----+                              +-----+
                         +---+                                +---+
                          +-+                                  +-+

	*/
public:
	BatteryMeter(
		int cell_pin_1 = 34, 
		int cell_pin_2 = 35,
		float r1_ohms = 3720.0f,
		float r2_ohms = 6100.0f,
		float r3_ohms = 7500.0f,
		float r4_ohms = 2316.0f,
		float ref_millivolts = 3300.0f,
		float cutoff_millivolts = 3300.0f,
		float resolution = 4096.0f
	);
	virtual ~BatteryMeter();

	float GetCellMillivolts(unsigned int cell);
	bool CutOff();
private:
	int pin_1;
	int pin_2;

	float R1;
	float R2;
	float R3;
	float R4;
	float ref_mv;
	float cutoff;
	float res;

   float cache_c1;
   float cache_c2;
   unsigned long cache_c1_time;
   unsigned long cache_c2_time;

   const unsigned long cache_validity = 10;
};

} // namespace TankController
#endif