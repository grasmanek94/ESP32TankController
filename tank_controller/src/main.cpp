#include <Arduino.h>

#include <Esp.h>

#include "Joystick.hpp"
#include "MotorControl.hpp"
#include "LaserDistanceMeter.hpp"
#include "BatteryMeter.hpp"

using namespace TankController;

const int BATTERY_STATUS_LED = 2;

MotorControl control;
Joystick joystick;
LaserDistanceMeter distance_meter;
BatteryMeter battery_meter;

bool controller_connected = false;

using milliseconds = unsigned long;

milliseconds next_control_update = 0;
const milliseconds control_update_time = 5;

milliseconds relay_disable_time = 0;
const milliseconds relay_disable_timeout = 75;

milliseconds next_battery_update = 0;
const int max_battery_charge_failures = 4;
const milliseconds battery_update_time = 125;
int battery_charge_failures = max_battery_charge_failures;

int max_speed = 1000;

void Reset()
{
    max_speed = 1000;

    control.Reset();
    joystick.Update();
    distance_meter.Reset();
}

void setup()
{
    Serial.begin(115200);

    pinMode(BATTERY_STATUS_LED, OUTPUT);

    Reset();

    Serial.println("Done");
}

#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

void AdjustSpeed(int amount)
{
    max_speed = CLAMP(max_speed + amount, 10, 1000);
    if (Serial)
    {
        Serial.printf("Speed: %d\r\n", max_speed);
    }
}

void PerformMotorControls(unsigned long time_now)
{
    const float SPEED_COEFFICIENT = 1.0f;
    const float STEER_COEFFICIENT = 1.0f;

    if (next_control_update < time_now)
    {
        next_control_update = time_now + control_update_time;

        joystick.Update();

        int acceleration = joystick.GetValue(Joystick::Axis::R2) + Joystick::AXIS_MIN;
        int deceleration = joystick.GetValue(Joystick::Axis::L2) + Joystick::AXIS_MIN;

        int speed = MotorControl::remap(acceleration - deceleration, -Joystick::AXIS_RANGE, Joystick::AXIS_RANGE, -max_speed, max_speed);
        int steer = MotorControl::remap(joystick.GetValue(Joystick::Axis::LEFT_X), Joystick::AXIS_MIN, Joystick::AXIS_MAX, max_speed, -max_speed);

        int speedR = CLAMP(speed * SPEED_COEFFICIENT - steer * STEER_COEFFICIENT, -max_speed, max_speed);
        int speedL = CLAMP(speed * SPEED_COEFFICIENT + steer * STEER_COEFFICIENT, -max_speed, max_speed);

        int turret_yaw = MotorControl::remap(joystick.GetValue(Joystick::Axis::RIGHT_X), Joystick::AXIS_MIN, Joystick::AXIS_MAX, 1000, -1000);
        int turret_pitch = MotorControl::remap(joystick.GetValue(Joystick::Axis::RIGHT_Y), Joystick::AXIS_MIN, Joystick::AXIS_MAX, -1000, 1000);

        control.MoveTracks(speedL, speedR);
        control.MoveTurret(turret_yaw);
        control.MovePitch(turret_pitch);

        bool enabled = control.RelayEnabled();

        if (enabled && relay_disable_time < time_now)
        {
            control.RelayCannon(false);
            if (Serial)
            {
                Serial.println("RelayCannon(false);");
            }
        }
        else if (!enabled &&
                 ((joystick.GetPressed(Joystick::Button::L1) && joystick.GetState(Joystick::Button::R1)) ||
                  (joystick.GetPressed(Joystick::Button::R1) && joystick.GetState(Joystick::Button::L1))))
        {
            relay_disable_time = time_now + relay_disable_timeout;
            control.RelayCannon(true);
            if (Serial)
            {
                Serial.printf("RelayCannon(true), now: %d, disable: %d\r\n", time_now, relay_disable_time);
            }
        }

        if (joystick.GetPressed(Joystick::Button::CIRCLE))
        {
            distance_meter.SetLaser(!distance_meter.GetLaser());
            if (Serial)
            {
                Serial.println("SetLaser(!GetLaser());");
            }
        }
        else if (joystick.GetPressed(Joystick::Button::TRIANGLE))
        {
            distance_meter.StartMeasurement(LaserDistanceMeter::Accuracy::ACC_LOW);
            if (Serial)
            {
                Serial.println("StartMeasurement(LaserDistanceMeter::Accuracy::ACC_LOW);");
            }
        }
        else if (joystick.GetPressed(Joystick::Button::SQUARE))
        {
            distance_meter.StartMeasurement(LaserDistanceMeter::Accuracy::ACC_MEDIUM);
            if (Serial)
            {
                Serial.println("StartMeasurement(LaserDistanceMeter::Accuracy::ACC_MEDIUM);");
            }
        }
        else if (joystick.GetPressed(Joystick::Button::SQUARE))
        {
            distance_meter.StartMeasurement(LaserDistanceMeter::Accuracy::ACC_HIGH);
            if (Serial)
            {
                Serial.println("StartMeasurement(LaserDistanceMeter::Accuracy::ACC_HIGH);");
            }
        }
        else if (joystick.GetPressed(Joystick::Button::DPAD_DOWN))
        {
            AdjustSpeed(-100);
        }
        else if (joystick.GetPressed(Joystick::Button::DPAD_UP))
        {
            AdjustSpeed(100);
        }
        else if (joystick.GetPressed(Joystick::Button::DPAD_LEFT))
        {
            AdjustSpeed(-10);
        }
        else if (joystick.GetPressed(Joystick::Button::DPAD_RIGHT))
        {
            AdjustSpeed(10);
        }
        else if (distance_meter.HasNewDistanceResult())
        {
            if (Serial)
            {
                Serial.printf("Distance: %f\r\n", distance_meter.GetDistance());
            }
        }
        else if (distance_meter.HasError())
        {
            if (Serial)
            {
                Serial.println("Distance: Measurement Error");
            }
        }
    }
}

#undef CLAMP

void loop()
{
    milliseconds now = millis();

    bool batteries_charged = battery_charge_failures < max_battery_charge_failures;

    if (next_battery_update < now)
    {
        next_battery_update = now + battery_update_time;

        if (battery_meter.CutOff())
        {
            ++battery_charge_failures;
            digitalWrite(BATTERY_STATUS_LED, battery_charge_failures % 2);

            if (battery_charge_failures >= max_battery_charge_failures)
            {
                control.MoveTracks(0, 0);
                control.MoveTurret(0);
                control.MovePitch(0);
                control.RelayCannon(false);

                if (batteries_charged)
                {
                    batteries_charged = false;
                    Reset();
                }
            }
        }
        else
        {
            battery_charge_failures = 0;
            batteries_charged = true;
            digitalWrite(BATTERY_STATUS_LED, LOW);
        }
    }

    bool actual_connected_status = joystick.CheckDevice();
    if (!actual_connected_status && controller_connected)
    {
        // exit routines here
        Reset();
        controller_connected = false;
        digitalWrite(BATTERY_STATUS_LED, LOW);
    }
    else if (actual_connected_status && !controller_connected)
    {
        controller_connected = true;
        Reset();
    }

    if (controller_connected && batteries_charged)
    {
        digitalWrite(BATTERY_STATUS_LED, HIGH);
        PerformMotorControls(now);
    }
}
