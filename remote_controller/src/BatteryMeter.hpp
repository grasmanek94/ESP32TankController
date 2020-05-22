#ifndef TankController_BatteryMeter_HXX
#define TankController_BatteryMeter_HXX

namespace TankController
{

class BatteryMeter
{
	/*
                           +------/\/\/\/-----+-----/\/\/\/-----+
                           |      R2=6070     |     R1=3750     |
                           |                  |                 |
                           |                  +                 |
                           |                 A1                 |
                  +   +---------+                               |
                  -      +---+                                  |
                           |                                    |
               3.3-4.2V    |                                    |
                           +------/\/\/\/-----+-----/\/\/\/-----+
                           |      R4=2165     |     R3=7240     |
                           |                  |                 |
                  +   +----+----+             +                 |
                                             A0                 |
                  -      +-+-+                                  |
                           |                                    |
               3.3-4.2V    |                                    |
                           |                                    |
                           +--------------/\/\/\/---------------+
                           |              R5=10M                |
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
		float r1_ohms = 3750,
		float r2_ohms = 6070,
		float r3_ohms = 7240,
		float r4_ohms = 2165,
		float ref_millivolts = 3300,
		float cutoff_millivolts = 3300,
		float resolution = 4096
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