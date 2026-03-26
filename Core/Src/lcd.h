#ifndef __LCD_H__
#define __LCD_H__

#include "lvgl.h"
#include "st7789.h"

void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);
void lv_port_display_init(void);
#endif