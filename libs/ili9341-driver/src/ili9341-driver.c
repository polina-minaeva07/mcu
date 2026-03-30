#include "ili9341-driver.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void ili9341_write_cmd(const ili9341_display_t *dev, uint8_t cmd)
{
    const ili9341_hal_t *hal = dev->hal;
    hal->gpio_cs_write(false);
    hal->gpio_dc_write(false); 
    hal->spi_write(&cmd, 1);
    hal->gpio_cs_write(true); 
}

void ili9341_write_data(const ili9341_display_t *dev, const uint8_t *data, uint32_t len)
{
    const ili9341_hal_t *hal = dev->hal;
    hal->gpio_cs_write(false); 
    hal->gpio_dc_write(true);
    hal->spi_write(data, len);
    hal->gpio_cs_write(true); 
}

void ili9341_write_data_byte(const ili9341_display_t *dev, uint8_t byte)
{
    ili9341_write_data(dev, &byte, 1);
}


void ili9341_set_address_window(const ili9341_display_t *dev, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t caset[4] = {
        (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFFu),
        (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFFu)
    };
    ili9341_write_cmd(dev, ILI9341_CMD_CASET);
    ili9341_write_data(dev, caset, 4);

    uint8_t paset[4] = {
        (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFFu),
        (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFFu)
    };
    ili9341_write_cmd(dev, ILI9341_CMD_PASET);
    ili9341_write_data(dev, paset, 4);

    ili9341_write_cmd(dev, ILI9341_CMD_MEMWRITE);
}


bool ili9341_init(ili9341_display_t *dev, const ili9341_hal_t *hal)
{
    if (!dev || !hal)                   return false;
    if (!hal->spi_write)                return false;
    if (!hal->gpio_cs_write)            return false;
    if (!hal->gpio_dc_write)            return false;
    if (!hal->gpio_reset_write)         return false;
    if (!hal->delay_ms)                 return false;

    dev->hal      = hal;
    dev->width    = ILI9341_NATIVE_WIDTH;
    dev->height   = ILI9341_NATIVE_HEIGHT;
    dev->rotation = ILI9341_ROTATION_0;

    hal->gpio_reset_write(false); 
    hal->delay_ms(10);
    hal->gpio_reset_write(true); 
    hal->delay_ms(100);

    ili9341_write_cmd(dev, ILI9341_CMD_SLEEP_OUT);
    hal->delay_ms(5);

    ili9341_write_cmd(dev, ILI9341_CMD_COLMOD);
    ili9341_write_data_byte(dev, ILI9341_COLMOD_16BIT);

    ili9341_write_cmd(dev, ILI9341_CMD_INVOFF);

    ili9341_write_cmd(dev, ILI9341_CMD_DISPON);
    hal->delay_ms(5);

    return true;
}
