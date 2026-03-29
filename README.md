# pj_nucleo_lvgl
my first try of lvgl on nucleo


## 2026-03-29

1. LVGL debug 打印问题已解决：需要在lv_init之后再调用 lv_log_register_print_cb(my_print);
2. 不显示方块的问题已解决: LV_DRAW_SW_COMPLEX 为1时才去画复杂图案。

3. 颜色异常问题：黑白颠倒，解决方案：
4. 使用SPI中断模式
5. 使用FreeRTOS模式运行LVGL
6. 打开触摸屏，写按钮的Event回调函数