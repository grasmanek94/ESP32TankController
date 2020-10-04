#ifndef TankController_BatteryMeter_HXX
#define TankController_BatteryMeter_HXX

namespace TankController
{

class BatteryMeter
{
public:
	BatteryMeter(int cell_pin_1 = 35);
	virtual ~BatteryMeter();

	bool CutOff();
   static void ISR();
private:
	int pin_1;
   unsigned long last_zero_time;
};

} // namespace TankController
#endif