#include "esp_bt_main.h"
#include "Esp.h"
#include "PS4Controller.h"
#include "Joystick.hpp"
#include "MotorControl.hpp"

static bool init_bluetooth()
{
  if (!btStart()) {
    Serial.println("Failed to initialize controller");
    return false;
  }

  if (esp_bluedroid_init()!= ESP_OK) {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }

  if (esp_bluedroid_enable()!= ESP_OK) {
    Serial.println("Failed to enable bluedroid");
    return false;
  }

  return true;
}

void printDeviceAddress()
{
    uint64_t chipid = ESP.getEfuseMac();
    const uint8_t* point = (uint8_t*)&chipid;
    
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
    while(!init_bluetooth()){}
    printDeviceAddress();
    PS4.begin("70:20:84:6d:3d:4c");    
}

bool controller_connected = false;
void loop()
{
    if (PS4.isConnected())
    {
        if(!controller_connected)
        {
            controller_connected = true;
            PS4.setLed(0, 200, 0);
            // PS4.setFlashRate(100, 200); // 100ms on, 200ms off [0,2550], {0,0} => ON
            // PS4.setRumble(100, 200); // weak, strong rumble [0,255]
            PS4.sendToController();
        }
        if ( PS4.data.button.up )
            Serial.println("Up Button");
        if ( PS4.data.button.down )
            Serial.println("Down Button");
        if ( PS4.data.button.left )
            Serial.println("Left Button");
        if ( PS4.data.button.right )
            Serial.println("Right Button");
          
        if ( PS4.data.button.upright )
            Serial.println("Up Right");
        if ( PS4.data.button.upleft )
            Serial.println("Up Left");
        if ( PS4.data.button.downleft )
            Serial.println("Down Left");
        if ( PS4.data.button.downright )
            Serial.println("Down Right");
          
        if ( PS4.data.button.triangle )
            Serial.println("Triangle Button");
        if ( PS4.data.button.circle )
            Serial.println("Circle Button");
        if ( PS4.data.button.cross )
            Serial.println("Cross Button");
        if ( PS4.data.button.square )
            Serial.println("Square Button");
          
        if ( PS4.data.button.l1 )
            Serial.println("l1 Button");
        if ( PS4.data.button.r1 )
            Serial.println("r1 Button");
          
        if ( PS4.data.button.l3 )
            Serial.println("l3 Button");
        if ( PS4.data.button.r3 )
            Serial.println("r3 Button");
          
        if ( PS4.data.button.share )
            Serial.println("Share Button");
        if ( PS4.data.button.options )
            Serial.println("Options Button");
          
        if ( PS4.data.button.ps )
            Serial.println("PS Button");
        if ( PS4.data.button.touchpad )
            Serial.println("Touch Pad Button");
          
        if ( PS4.data.button.l2 ) {
            Serial.print("l2 button at ");
            Serial.println(PS4.data.analog.button.l2, DEC);
        }
        if ( PS4.data.button.r2 ) {
            Serial.print("r2 button at ");
            Serial.println(PS4.data.analog.button.r2, DEC);
        }
        
        if ( PS4.event.analog_move.stick.lx ) {
            Serial.print("Left Stick x at ");
            Serial.println(PS4.data.analog.stick.lx, DEC);
        }
        if ( PS4.event.analog_move.stick.ly ) {
            Serial.print("Left Stick y at ");
            Serial.println(PS4.data.analog.stick.ly, DEC);
        }
        if ( PS4.event.analog_move.stick.rx ) {
            Serial.print("Right Stick x at ");
            Serial.println(PS4.data.analog.stick.rx, DEC);
        }
        if ( PS4.event.analog_move.stick.ry ) {
            Serial.print("Right Stick y at ");
            Serial.println(PS4.data.analog.stick.ry, DEC);
        }
        
        if (PS4.data.status.charging)
          Serial.println("The controller is charging");
        if (PS4.data.status.audio)
          Serial.println("The controller has headphones attached");
        if (PS4.data.status.mic)
          Serial.println("The controller has a mic attached");
        
        Serial.print("Battery = ");
        Serial.print(PS4.data.status.battery, DEC);
        Serial.println(" / 16");
        
        Serial.println();
        delay(100);
    } else {
        controller_connected = false;
        printDeviceAddress();
        delay(1000);
    }
}
