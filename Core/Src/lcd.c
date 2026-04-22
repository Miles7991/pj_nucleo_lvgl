#include "lcd.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "SEGGER_RTT.h"

static lv_display_t * lcd_disp;
static volatile bool disp_flush_enabled = true;



// 刷新显示
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        /* Use the driver function to flush the buffer */
        st7789_flush(area->x1, area->y1, area->x2, area->y2, (uint16_t *)px_map);
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing */
    lv_display_flush_ready(disp_drv);
}

void lv_port_display_init(void)
{
    // 初始化显示
    st7789_init();
    /* Create the LVGL display object and the ST7789 LCD display driver */
    lcd_disp = lv_display_create(ST7789_WIDTH, ST7789_HEIGHT);
    lv_display_set_color_format(lcd_disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
    lv_display_set_flush_cb(lcd_disp, disp_flush);
	lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_0);

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