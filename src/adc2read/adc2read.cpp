#include <Arduino.h>

#include <soc/sens_reg.h>

uint64_t adc2_register;

void esp32_adc2_setup()
{
    adc2_register = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG);
}

uint16_t esp32_analogRead(uint8_t pin_number)
{
    WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, adc2_register);
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_DATA_INV);
    return analogRead(pin_number);
}