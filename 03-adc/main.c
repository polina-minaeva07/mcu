#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <stdint.h>

#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
    led_task_state_set(LED_STATE_ON);
    printf("led ON\n");
}
void led_off_callback(const char* args)
{
    led_task_state_set(LED_STATE_OFF);
    printf("led OFF\n");
}
void led_blink_callback(const char* args)
{
    led_task_state_set(LED_STATE_BLINK);
    printf("led blink\n");
}

void led_blink_set_period_ms_callback(const char* args)
{
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);
    if (period_ms == 0)
    {
        printf("ERROR: period_ms = 0");
        return;
    }
    led_task_set_blink_period_ms(period_ms);
}

void help_callback(const char* args)
{
    help();
}

void mem_callback(const char* args)
{
    uint32_t address = 0;
    sscanf(args, "%x", &address);
    printf("Word at this address: '0x%x'\n", *(uint32_t*)address);
}
void wmem_callback(const char* args)
{
    uint32_t address = 0, value;
    sscanf(args, "%x %x", &address, &value);
    *(uint32_t*)address = value;
    printf("The value '0x%x' is written at the address '0x%x'\n", value, address);
}

void get_adc_callback(const char* args)
{
    float voltage_V = adc_task_read_voltage();
    printf("%f\n", voltage_V);
}
void get_temp_callback(const char* args)
{
    float temp_C = adc_task_read_temperature();
    printf("%f\n", temp_C);
}

void tm_start_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_RUN);
    printf("Measure started\n");
}
void tm_stop_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_IDLE);
    printf("Measure stopped\n");
}

api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "turn on the led"},
    {"off", led_off_callback, "turn off the led"},
    {"blink", led_blink_callback, "the led is blinking"},
    {"set_period", led_blink_set_period_ms_callback, "blinking with arguments"},
    {"mem", mem_callback, "read from memory"},
    {"wmem", wmem_callback, "write in memory"},
    {"help", help_callback, "print commands description"},
    {"get_adc", get_adc_callback, "outputs the voltage on the pin"},
    {"get_temp", get_temp_callback, "outputs the measured temperature"},
    {"tm_start", tm_start_callback, "the beginning of voltage and temperature measurements"},
    {"tm_stop", tm_stop_callback, "end of voltage and temperature measurements"},
	{NULL, NULL, NULL},
};

int main()
{
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    adc_task_init();

    while (1)
    {
        protocol_task_handle(stdio_task_handle());
        led_task_handle();
        adc_task_handle();
    }
}