#include <Arduino.h>

#include "LaserDistanceMeter.hpp"

namespace TankController
{

LaserDistanceMeter::LaserDistanceMeter(int uartnr)
	: distance{0.0f}, error{false}, keep_laser_on{false},
	  measurement_running{false}, laser_ack{false},
	  module_state{false}, has_new_distance{false},
	  last_command_time{0}, comm(uartnr),
	  buffer{}, size{0}
{
	comm.begin(baud);

	Reset();
}

void LaserDistanceMeter::Reset()
{
	buffer[0] = 0;
	size = 0;
	distance = 0.0f;
	error = false;
	keep_laser_on = false;
    measurement_running = false;
	laser_ack = false;
	module_state = false;
	has_new_distance = false;
    last_command_time = 0;

	SendCommand(Command::LASER_CLOSE);
}

LaserDistanceMeter::~LaserDistanceMeter()
{
}

void LaserDistanceMeter::SetLaser(bool on)
{
	if (IsBusy())
	{
		return;
	}

	keep_laser_on = on;
	laser_ack = true;
	SendCommand(on ? Command::LASER_OPEN : Command::LASER_CLOSE);
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
		measurement_running = true;
		SendCommand(Command::MEASUREMENT_FAST);
		break;
	case TankController::LaserDistanceMeter::Accuracy::ACC_MEDIUM:
		measurement_running = true;
		SendCommand(Command::MEASUREMENT_SLOW);
		break;
	case TankController::LaserDistanceMeter::Accuracy::ACC_HIGH:
		measurement_running = true;
		SendCommand(Command::MEASUREMENT_HIGHACCURACY);
		break;
	default:
		break;
	}
}

bool LaserDistanceMeter::IsMeasuring() const
{
	return measurement_running;
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
	bool has_error = error;
	error = false;
	return has_error;
}

bool LaserDistanceMeter::IsBusy() const
{
	return laser_ack ||
		   module_state ||
		   measurement_running;
}

void LaserDistanceMeter::Update()
{
	if(!comm)
	{
		return;
	}

	bool laser_ack_recv = false;
	bool measurement_recv = false;
	bool state_recv = false;
	bool event = false;

	while (IsBusy() && comm.available() > 0)
	{
		int c = comm.read();
		if (c != -1)
		{
			if (c == '\n')
			{
				laser_ack_recv = size >= 5 && laser_ack;
				measurement_recv = size >= 7 && measurement_running;
				state_recv = size >= 15 && module_state;
				event = laser_ack_recv || measurement_recv || state_recv;
				if (event)
				{
					break;
				}
			}
			else
			{
				if (size >= sizeof(buffer) - 1)
				{
					size = 0;
				}
				buffer[size++] = c;
				buffer[size] = 0;
			}
		}
	}

	if (event)
	{
		String line(buffer);
		if (laser_ack_recv)
		{
			laser_ack = false;
			// line probably == ",OK!"
		}
		else if (measurement_recv)
		{
			measurement_running = false;

			size_t separator = line.indexOf(':');
			size_t unit = line.indexOf('m');

			if (separator != -1 &&
				unit != -1 &&
				unit > ++separator)
			{
				has_new_distance = true;
				distance = std::atof(line.substring(separator, unit - separator).c_str());
			}
			else
			{
				error = true;
			}

			if (keep_laser_on)
			{
				SendCommand(Command::LASER_OPEN);
			}
		}
		else if (state_recv)
		{
			module_state = false;
		}
	}

	while (!IsBusy() && comm.available() > 0)
	{
		comm.read();
	}

	if (IsBusy() && millis() > 5000 && millis() - last_command_time > milliseconds(5000))
	{
		laser_ack = false;
		measurement_running = false;
		module_state = false;
	}
}

bool LaserDistanceMeter::SendCommand(Command command)
{
	if (!comm || !comm.availableForWrite())
	{
		return false;
	}

	last_command_time = millis();
	return comm.write((uint8_t *)&command, 1) == 1;
}

void LaserDistanceMeter::GetModuleState()
{
	module_state = true;
	SendCommand(Command::MODULE_STATE);
}

} // namespace TankController