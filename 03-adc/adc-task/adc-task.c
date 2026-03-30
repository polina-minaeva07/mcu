#include "adc-task.h"

#include "hardware/adc.h"
#include "pico/stdlib.h"

#include <stdio.h>

const uint GPIO_PIN = 26;
const uint ADC_CHANNEL = 0;
const uint TEMP_SENSOR_ADC_CHANNEL = 4;

adc_task_state_t adc_state = 0;
uint64_t adc_ts;
uint64_t ADC_TASK_MEAS_PERIOD_US;

void adc_task_init()
{
    adc_init();
    adc_gpio_init(GPIO_PIN);
    adc_set_temp_sensor_enabled(true);

    ADC_TASK_MEAS_PERIOD_US = 100000;
    adc_ts = 0;
}

float adc_task_read_voltage()
{
    adc_select_input(ADC_CHANNEL);
    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts * 3.3f / 4096.0f;
    return voltage_V;
}

float adc_task_read_temperature()
{
    adc_select_input(TEMP_SENSOR_ADC_CHANNEL);
    uint16_t voltage_temp = adc_read();
    float temp_V = voltage_temp * 3.3f / 4096.0f;
    float temp_C = 27.0f - (temp_V - 0.706f) / 0.001721f;
    return temp_C;
}

void adc_task_set_state(adc_task_state_t state)
{
    adc_state = state;
    if (state == ADC_TASK_STATE_RUN) 
    {
        adc_ts = time_us_64();
    }
}

void adc_task_handle()
{
    if (adc_state != ADC_TASK_STATE_RUN)
    {
        return;
    }

    uint64_t time = time_us_64();
    if (time >= adc_ts + ADC_TASK_MEAS_PERIOD_US)
    {
        adc_ts = time;

        float voltage_V = adc_task_read_voltage();
        float temp_C = adc_task_read_temperature();
        printf("%f %f\n", voltage_V, temp_C);
    }
}