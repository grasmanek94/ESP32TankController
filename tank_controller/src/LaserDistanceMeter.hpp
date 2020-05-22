#ifndef TankController_LaserDistanceMeter_HXX
#define TankController_LaserDistanceMeter_HXX

#include <Arduino.h>

namespace TankController
{

class LaserDistanceMeter
{
public:
	using milliseconds = unsigned long;

	enum class Accuracy
	{
		ACC_LOW,
		ACC_MEDIUM,
		ACC_HIGH
	};

	LaserDistanceMeter(int uartnr = 2);
	virtual ~LaserDistanceMeter();

	void SetLaser(bool on);
	bool GetLaser() const;

	void StartMeasurement(Accuracy accuracy);
	bool IsMeasuring() const;
	bool HasNewDistanceResult() const;
	float GetDistance();

	bool HasError();
	bool IsBusy() const;

	void Update();
    void Reset();

private:
	float distance;
	bool error;
	bool keep_laser_on;
	bool measurement_running;
	bool laser_ack;
	bool module_state;
	bool has_new_distance;
	
	milliseconds last_command_time;
	HardwareSerial comm;
	char buffer[128];
	int size;

	const int baud = 19200;

	enum class Command : char
	{
		NONE = 0,

		LASER_OPEN = 'O',
		LASER_CLOSE = 'C',

		// returns "12.345m,0079\\n"
		// or returns " 2.345m,0079\\n"
		MEASUREMENT_FAST = 'F',
		MEASUREMENT_SLOW = 'D',
		MEASUREMENT_HIGHACCURACY = 'M',

		MODULE_STATE = 'S'
	};

	bool SendCommand(Command command);
	void GetModuleState();
};

} // namespace TankController
#endif