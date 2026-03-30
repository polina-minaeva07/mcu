#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdint.h>

#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "led-task/led-task.h"
#include "hardware/i2c.h"
#include "bme280-driver.h"

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

void read_regs_callback(const char* args)
{
    uint32_t addr, N;
    sscanf(args, "%x %x", &addr, &N);

    if (addr > 0xFF)
    {
        printf("Error: addr must be <= 0xFF");
        return;
    }
    if (N > 0xFF)
    {
        printf("Error: N must be <= 0xFF");
        return;
    }
    if (addr + N > 0x100)
    {
        printf("Error: addr + N must be <= 0x100");
        return;
    }

    uint8_t buffer[256] = {0};
    bme280_read_regs(addr, buffer, N);

    for (int i = 0; i < N; i++)
    {
        printf("bme280 register [0x%X] = 0x%X\n", addr + i, buffer[i]);
    }
}

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}
void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}

void write_regs_callback(const char* args)
{
    uint32_t addr, value;
    sscanf(args, "%x %x", &addr, &value);

    if (addr > 0xFF)
    {
        printf("Error: addr must be <= 0xFF\n");
        return;
    }
    if (value > 0xFF)
    {
        printf("Error: N must be <= 0xFF\n");
        return;
    }

    bme280_write_reg(addr, value);
    printf("Written 0x%02X to register 0x%02X\n", value, addr);
}

void temp_raw_callback(const char* args) 
{
    uint16_t raw = bme280_read_temp_raw();
    printf("temp_raw: %u (0x%04X)\n", raw, raw);
}
void pres_raw_callback(const char* args) 
{
    uint16_t raw = bme280_read_pres_raw();
    printf("pres_raw: %u (0x%04X)\n", raw, raw);
}
void hum_raw_callback(const char* args) 
{
    uint16_t raw = bme280_read_hum_raw();
    printf("hum_raw: %u (0x%04X)\n", raw, raw);
}

void temp_callback(const char* args) 
{
    float temp = bme280_read_temperature();
    printf("%.2f °C\n", temp);
}
void pres_callback(const char* args) 
{
    float pres = bme280_read_pressure();
    printf("%.2f Pa\n", pres);
}
void hum_callback(const char* args) 
{
    float hum = bme280_read_humidity();
    printf("%.2f %%\n", hum);
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
    {"read_reg", read_regs_callback, "read BME280 registers: read_regs <addr> <count>"},
    {"write_reg", write_regs_callback, "write BME280 register: write_reg <addr> <value>"},
    {"temp_raw", temp_raw_callback, "read raw temperature value from BME280"},
    {"pres_raw", pres_raw_callback, "read raw pressure value from BME280"},
    {"hum_raw", hum_raw_callback, "read raw humidity value from BME280"},
    {"temp", temp_callback, "read temperature in °C"},
    {"pres", pres_callback, "read pressure in hPa"},
    {"hum", hum_callback, "read humidity in %"},
	{NULL, NULL, NULL},
};

int main()
{
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    i2c_init(i2c1, 100000);

    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);

    bme280_init(rp2040_i2c_read, rp2040_i2c_write);

    while (1)
    {
        protocol_task_handle(stdio_task_handle());
        led_task_handle();
    }
}