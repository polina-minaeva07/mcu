#include "hardware/adc.h"
#include "pico/stdlib.h"

#include <stdio.h>

const uint GPIO_PIN = 26;
const uint ADC_CHANNEL = 0;

void adc_task_init()
{
    adc_init();
    adc_gpio_init(GPIO_PIN);
    adc_set_temp_sensor_enabled(true);
}

float read_voltage()
{
    adc_select_input(ADC_CHANNEL);
    uint16_t voltage_counts = adc_read();
    float voltage_V = (voltage_counts / 4096.0) * 3.3;
    return voltage_V;
}