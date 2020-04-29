#ifndef TankController_BatteryMeter_HXX
#define TankController_BatteryMeter_HXX

#include "Arduino.h"

namespace TankController
{

class BatteryMeter
{
public:
	BatteryMeter(int cell_pin_1 = 12, int cell_pin_2 = 13);
	virtual ~BatteryMeter();

	bool Cell1Okay();
	bool Cell2Okay();
	bool ControllerOkay();
	bool AllOkay();
private:
	int pin_1;
	int pin_2;
};

} // namespace TankController
#endif