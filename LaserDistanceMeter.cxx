#include <string>
#include <thread>

#include <serial/serial.h>

#include <TankController/LaserDistanceMeter.hxx>

namespace TankController
{
	LaserDistanceMeter::LaserDistanceMeter(const std::string& device)
		: comm{ nullptr }, device{ device }, worker{ nullptr },
		running{ true }, distance{ 0.0f }, error{ false }, command{ (char)Command::NONE },
		keep_laser_on{ false }, measurement_running{ false }, laser_ack{ false },
		module_state{ false }, has_new_distance{ false },
		last_command_time{ std::chrono::high_resolution_clock::now() - milliseconds(5000) }
	{ 
		comm = new serial::Serial(device, baud, serial::Timeout(), data_bits, parity);
		worker = new std::thread(&LaserDistanceMeter::WorkerThread, this);		
	}

	LaserDistanceMeter::~LaserDistanceMeter()
	{ 
		running = false;

		if (worker != nullptr)
		{
			worker->join();
			delete worker;
			worker = nullptr;
		}

		if (comm != nullptr)
		{
			comm->close();
			delete comm;
			comm = nullptr;
		}
	}

	void LaserDistanceMeter::SetLaser(bool on)
	{
		if (IsBusy())
		{
			return;
		}

		keep_laser_on = on;
		command = (char)(on ? Command::LASER_OPEN : Command::LASER_CLOSE);
	}

	bool LaserDistanceMeter::GetLaser() const
	{
		return keep_laser_on;
	}

	void LaserDistanceMeter::StartMeasurement(Accuracy accuracy)
	{
		if (IsBusy())
		{
			return;
		}

		switch (accuracy)
		{
		case TankController::LaserDistanceMeter::Accuracy::ACC_LOW:
			command = (char)Command::MEASUREMENT_FAST;
			break;
		case TankController::LaserDistanceMeter::Accuracy::ACC_MEDIUM:
			command = (char)Command::MEASUREMENT_SLOW;
			break;
		case TankController::LaserDistanceMeter::Accuracy::ACC_HIGH:
			command = (char)Command::MEASUREMENT_HIGHACCURACY;
			break;
		default:
			break;
		}
	}

	bool LaserDistanceMeter::IsMeasuring() const
	{
		return
			measurement_running ||
			command == (char)Command::MEASUREMENT_FAST ||
			command == (char)Command::MEASUREMENT_SLOW ||
			command == (char)Command::MEASUREMENT_HIGHACCURACY;
	}

	bool LaserDistanceMeter::HasNewDistanceResult() const
	{
		return has_new_distance;
	}

	float LaserDistanceMeter::GetDistance()
	{
		has_new_distance = false;
		return distance;
	}

	bool LaserDistanceMeter::HasError()
	{
		return error.exchange(false);
	}

	bool LaserDistanceMeter::IsBusy() const
	{
		return 
			laser_ack || 
			module_state || 
			measurement_running || 
			command != (char)Command::NONE;
	}

	void LaserDistanceMeter::WorkerThread()
	{
		while (running)
		{
			volatile char local_command = (char)Command::NONE;
			local_command = command.exchange(local_command);

			switch (local_command)
			{
			case (char)Command::LASER_OPEN:
			case (char)Command::LASER_CLOSE:

				laser_ack = true;
				keep_laser_on = local_command == (char)Command::LASER_OPEN;

				last_command_time = std::chrono::high_resolution_clock::now();

				comm->write((uint8_t*)&local_command,1);
				break;
				
			case (char)Command::MEASUREMENT_FAST:
			case (char)Command::MEASUREMENT_SLOW:
			case (char)Command::MEASUREMENT_HIGHACCURACY:

				measurement_running = true;
				error = false;

				last_command_time = std::chrono::high_resolution_clock::now();

				comm->write((uint8_t*)&local_command, 1);
				break;

			case (char)Command::MODULE_STATE:
				module_state = true;

				last_command_time = std::chrono::high_resolution_clock::now();

				comm->write((uint8_t*)&local_command, 1);
				break;
			}

			if ((comm->available() >= 5 && laser_ack) ||
				(comm->available() >= 7 && measurement_running) ||
				(comm->available() >= 15 && module_state))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(20)); // takes ~6ms to send 15 chars
				
				std::string line;
				if (comm->readline(line) >= 5)
				{
					if (laser_ack)
					{
						laser_ack = false;
						// line probably == ",OK!"
					}
					else if (measurement_running)
					{
						measurement_running = false;

						size_t separator = line.find(':');
						size_t unit = line.find('m');

						if (separator != std::string::npos &&
							unit != std::string::npos &&
							unit > ++separator)
						{
							has_new_distance = true;
							distance = std::atof(line.substr(separator, unit - separator).c_str());
						}
						else
						{
							error = true;
						}

						//has_new_distance = true;

						if (keep_laser_on)
						{
							command = (char)Command::LASER_OPEN;
						}
					}
					else if (module_state)
					{
						module_state = false;
					}
				}
			}

			while(!IsBusy() && comm->available() > 0)
			{
				uint8_t c = 0;
				comm->read(&c, 1);
			}

			if (IsBusy() && std::chrono::high_resolution_clock::now() - last_command_time > milliseconds(5000))
			{
				laser_ack = false;
				measurement_running = false;
				module_state = false;
				command = (char)Command::NONE;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
}