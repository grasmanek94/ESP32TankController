#include "esp_bt_main.h"
#include "Esp.h"
#include "PS4Controller.h"
#include "Joystick.hpp"
#include "MotorControl.hpp"
#include "LaserDistanceMeter.hpp"

using namespace TankController;

static bool init_bluetooth()
{
    if (!btStart())
    {
        Serial.println("Failed to initialize controller");
        return false;
    }

    if (esp_bluedroid_init() != ESP_OK)
    {
        Serial.println("Failed to initialize bluedroid");
        return false;
    }

    if (esp_bluedroid_enable() != ESP_OK)
    {
        Serial.println("Failed to enable bluedroid");
        return false;
    }

    return true;
}

void printDeviceAddress()
{
    uint64_t chipid = ESP.getEfuseMac();
    const uint8_t *point = (uint8_t *)&chipid;

    Serial.println("");

    for (int i = 0; i < 6; i++)
    {
        Serial.printf("%02X", (int)point[i]);
        if (i < 5)
        {
            Serial.print(":");
        }
    }

    Serial.println("");
}

const int BATTERY_STATUS_LED = 2;

bool controller_connected;
bool batteries_charged;
MotorControl control;
Joystick joystick;
LaserDistanceMeter distance_meter(&Serial1);

using milliseconds = unsigned long;

milliseconds last_control_update;
milliseconds relay_disable_time;

int max_speed;

void Reset()
{
    batteries_charged = false;
    controller_connected = false;
    last_control_update = millis();
    relay_disable_time = millis();
    max_speed = 1000;

    control.Reset();
    joystick.Update();
    distance_meter.Reset();
}

void setup()
{
    Serial.begin(115200);

    while (!init_bluetooth())
    {
    }

    printDeviceAddress();
    PS4.begin("70:20:84:6d:3d:4c");

    pinMode(BATTERY_STATUS_LED, OUTPUT);

    Reset();
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

void ControlLoop()
{
    const float SPEED_COEFFICIENT = 1.0f;
    const float STEER_COEFFICIENT = 1.0f;

    milliseconds now = millis();

    if (now - last_control_update >= milliseconds(10))
    {
        last_control_update = now;

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

        if (enabled && relay_disable_time < now)
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
            relay_disable_time = now + milliseconds(75);
            control.RelayCannon(true);
            if (Serial)
            {
                Serial.printf("RelayCannon(true), now: %d, disable: %d\r\n", now, relay_disable_time);
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
    bool batteries_charged_now = false;
    if (batteries_charged && !batteries_charged_now)
    {
        Reset();
    }

    if (!batteries_charged)
    {
        delay(250);
        digitalWrite(BATTERY_STATUS_LED, HIGH);
        delay(250);
        digitalWrite(BATTERY_STATUS_LED, LOW);
        return;
    }

    if (PS4.isConnected())
    {
        if (!controller_connected)
        {
            // init routines here
            controller_connected = true;
            PS4.setLed(0, 200, 0);
            // PS4.setFlashRate(100, 200); // 100ms on, 200ms off [0,2550], {0,0} => ON
            // PS4.setRumble(100, 200); // weak, strong rumble [0,255]
            PS4.sendToController();
        }

        ControlLoop();

        // if (PS4.data.status.charging)
        //   Serial.println("The controller is charging");
        // Serial.print("Battery = ");
        // Serial.print(PS4.data.status.battery, DEC);
        // Serial.println(" / 16");
    }
    else
    {
        if (controller_connected)
        {
            Reset();
            controller_connected = false;
        }

        printDeviceAddress();
        delay(100);
    }
}
