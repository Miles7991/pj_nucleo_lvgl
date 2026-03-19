#ifndef __ST7789_H__
#define __ST7789_H__

#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>		//itoa()
#include <stdio.h>


#define ST7789_WIDTH        240
#define ST7789_HEIGHT       320

void st7789_init(void);
void st7789_draw_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void st7789_clear(uint16_t color);
// Add by hsy using Trae 2026-03-18
// Flush the color buffer to the display specified by the coordinates
void st7789_flush(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t *color);
void st7789_set_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
int testPic(void);
#endif  // __ST7789_H__