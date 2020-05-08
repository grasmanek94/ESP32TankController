#pragma once

#include <Arduino.h>

void esp32_adc2_setup();
uint16_t esp32_analogRead(uint8_t pin_number);