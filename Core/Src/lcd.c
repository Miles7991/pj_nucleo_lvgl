#include "lcd.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "SEGGER_RTT.h"

static lv_display_t * lcd_disp;

/* Send short command to the LCD. This function shall wait until the transaction finishes. */
void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    //send cmd
    for(int i = 0; i < cmd_size; i++)
    {
        st7789_write_cmd(cmd[i]);
    }
    //send data
    st7789_write_bytes(param, param_size);
}

/* Send large array of pixel data to the LCD. If necessary, this function has to
 * do the byte-swapping. This function can do the transfer in the background. */
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    for(int i = 0; i < cmd_size; i++)
    {
        st7789_write_cmd(cmd[i]);
    }
    st7789_write_bytes(param, param_size);
    lv_display_flush_ready(disp);
}

void lv_port_display_init(void)
{

    /* Create the LVGL display object and the ST7789 LCD display driver */
    lcd_disp = lv_st7789_create(ST7789_WIDTH, ST7789_HEIGHT, LV_LCD_FLAG_NONE, my_lcd_send_cmd, my_lcd_send_color);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_0);     /* set landscape orientation */

    /* Example: two dynamically allocated buffers for partial rendering */
    uint8_t * buf1 = NULL;
    uint8_t * buf2 = NULL;

    uint32_t buf_size = ST7789_WIDTH * ST7789_HEIGHT / 10 * lv_color_format_get_size(lv_display_get_color_format(
                                                                                              lcd_disp));

    buf1 = lv_malloc(buf_size);
    if(buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    buf2 = lv_malloc(buf_size);
    if(buf2 == NULL) {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(buf1);
        return;
    }
    lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

}