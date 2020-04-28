#ifndef TankController_LaserDistanceMeter_HXX
#define TankController_LaserDistanceMeter_HXX

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#include <serial/serial.h>

namespace TankController
{
	class LaserDistanceMeter
	{
	public:
		using timer = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using milliseconds = std::chrono::duration<timer::rep, std::milli>;

		enum class Accuracy
		{
			ACC_LOW,
			ACC_MEDIUM,
			ACC_HIGH
		};

		LaserDistanceMeter(const std::string& device);
		virtual ~LaserDistanceMeter();

		void SetLaser(bool on);
		bool GetLaser() const;

		void StartMeasurement(Accuracy accuracy);
		bool IsMeasuring() const;
		bool HasNewDistanceResult() const;
		float GetDistance();

		bool HasError();
		bool IsBusy() const;

	private:

		serial::Serial* comm;
		const std::string device;
		std::thread* worker;

		std::atomic<bool> running;
		std::atomic<float> distance;
		std::atomic<bool> error;
		std::atomic<char> command;
		std::atomic<bool> keep_laser_on;
		std::atomic<bool> measurement_running;
		std::atomic<bool> laser_ack;
		std::atomic<bool> module_state;
		std::atomic<bool> has_new_distance;

		timer last_command_time;

		const int baud = 19200;
		const serial::parity_t parity = serial::parity_none;
		const serial::bytesize_t data_bits = serial::eightbits;

		void WorkerThread();

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
	};
}
#endif