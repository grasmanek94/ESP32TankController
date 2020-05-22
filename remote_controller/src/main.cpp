#include <esp_bt_main.h>
#include <esp_wifi.h>
#include <Esp.h>

#include "PS4Lib/PS4Controller.h"
#include "Joystick.hpp"
#include "BatteryMeter.hpp"

using namespace TankController;

const int BATTERY_STATUS_LED = 2;

Joystick joystick;
BatteryMeter battery_meter;

bool controller_connected = false;

using milliseconds = unsigned long;

milliseconds next_battery_update = 0;
const int max_battery_charge_failures = 4;
const milliseconds battery_update_time = 125;
int battery_charge_failures = max_battery_charge_failures;

void Reset()
{
    //joystick.Update();
}

bool init_bluetooth()
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

void setup()
{
    Serial.begin(115200);

    delay(1000);

    while (!init_bluetooth()){}

    PS4.begin("70:20:84:6D:3D:4C");

    pinMode(BATTERY_STATUS_LED, OUTPUT);

    Reset();

    printDeviceAddress();

    Serial.println("Done");
}

void loop()
{
    milliseconds now = millis();

    /*bool batteries_charged = battery_charge_failures < max_battery_charge_failures;

    if (next_battery_update < now)
    {
        next_battery_update = now + battery_update_time;

        if (battery_meter.CutOff())
        {
            ++battery_charge_failures;
            digitalWrite(BATTERY_STATUS_LED, battery_charge_failures % 2);

            if (battery_charge_failures >= max_battery_charge_failures)
            {
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
    }*/

    if (!PS4.isConnected() && controller_connected)
    {
        Reset();
        controller_connected = false;
        digitalWrite(BATTERY_STATUS_LED, LOW);
    }
    else if (PS4.isConnected() && !controller_connected)
    {
        controller_connected = true;
        PS4.setLed(0, 200, 0);
        PS4.sendToController();
    }

    if (controller_connected /*&& batteries_charged*/)
    {
        digitalWrite(BATTERY_STATUS_LED, HIGH);
        joystick.Update();
    }
    else
    {
        digitalWrite(BATTERY_STATUS_LED, millis() / 1000 % 2);
    }
}
