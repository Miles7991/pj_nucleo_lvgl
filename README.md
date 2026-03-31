# pj_nucleo_lvgl
my first try of lvgl on nucleo


## 2026-03-29

1. LVGL debug 打印问题已解决：需要在lv_init之后再调用 lv_log_register_print_cb(my_print);
2. 不显示方块的问题已解决: LV_DRAW_SW_COMPLEX 为1时才去画复杂图案。
3. 颜色异常问题：黑白颠倒，解决方案：
    - 加上 lv_display_set_color_format(lcd_disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
    - 打开 #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED         1   
    - 不使用原生的 lcd_st7789 ，而是使用自定义的 disp_flush 函数。
4. 使用FreeRTOS模式运行LVGL： 选择 #define LV_USE_OS   LV_OS_FREERTOS，不要选择 LV_OS_CMSIS_RTOS2

5. 打开触摸屏：AI帮写好了代码，并分析了之前cb失败的原因
    - 改了 cst816d read reg write reg.
    - 找到之前的问题是连续两次调用get_touch_data，但是第一次已经将标志位清零。 
    - ：将 4 个坐标字节（XH, XL, YH, YL）合并为一个 I2C 事务读取，提高了读取速度。
6. 使用SPI中断模式
7. button 加入 Event callback