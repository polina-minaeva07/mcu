#pragma once

#include <stdint.h>
#include "ili9341-driver.h"

typedef struct ili9341_font_t {
    const unsigned char* data; 
    uint8_t        width;      
    uint8_t        height;
    uint8_t        first_char; 
    uint8_t        char_count;
} ili9341_font_t;

extern const ili9341_font_t jetbrains_font;

void ili9341_draw_char(const ili9341_display_t *dev,
                       uint16_t x, uint16_t y, char c,
                       const ili9341_font_t *font,
                       uint16_t color, uint16_t bg_color);