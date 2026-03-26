#include "st7789.h"

void st7789_write_bytes(uint8_t *data, uint32_t lenght)
{
    LCD_DC_1;
    DEV_SPI_Write_nByte(data, lenght);
}

void st7789_write_byte(uint8_t data)
{
    LCD_DC_1;
    //LCD_2IN8_CS_0;
    DEV_SPI_WriteByte(data);
    //LCD_2IN8_CS_1;
}

void st7789_write_cmd(uint8_t cmd)
{
    LCD_DC_0;
    //LCD_2IN8_CS_0;
    DEV_SPI_WriteByte(cmd);
    //LCD_2IN8_CS_1;
}


static void st7789_rst(void)
{
    LCD_RST_1;
    DEV_Delay_ms(100);
    LCD_RST_0;
    DEV_Delay_ms(100);
    LCD_RST_1;
    DEV_Delay_ms(100);
}



void st7789_init(void)
{
    st7789_rst();

    st7789_write_cmd(0x11);     

    DEV_Delay_ms(120);                //DEV_Delay_ms 120ms

    st7789_write_cmd(0x36);     // Memory Data Access Control MY,MX~~
    st7789_write_byte(0x48);   

    st7789_write_cmd(0x3A);     
    st7789_write_byte(0x05);   //st7789_write_byte(0x66);

    st7789_write_cmd(0xF0);     // Command Set Control
    st7789_write_byte(0xC3);   

    st7789_write_cmd(0xF0);     
    st7789_write_byte(0x96);   

    st7789_write_cmd(0xB4);     
    st7789_write_byte(0x01);   

    st7789_write_cmd(0xB7);     
    st7789_write_byte(0xC6);   

    st7789_write_cmd(0xC0);     
    st7789_write_byte(0x80);   
    st7789_write_byte(0x45);   

    st7789_write_cmd(0xC1);     
    st7789_write_byte(0x13);   //18  //00

    st7789_write_cmd(0xC2);     
    st7789_write_byte(0xA7);   

    st7789_write_cmd(0xC5);     
    st7789_write_byte(0x0A);   

    st7789_write_cmd(0xE8);     
    st7789_write_byte(0x40);
    st7789_write_byte(0x8A);
    st7789_write_byte(0x00);
    st7789_write_byte(0x00);
    st7789_write_byte(0x29);
    st7789_write_byte(0x19);
    st7789_write_byte(0xA5);
    st7789_write_byte(0x33);

    st7789_write_cmd(0xE0);
    st7789_write_byte(0xD0);
    st7789_write_byte(0x08);
    st7789_write_byte(0x0F);
    st7789_write_byte(0x06);
    st7789_write_byte(0x06);
    st7789_write_byte(0x33);
    st7789_write_byte(0x30);
    st7789_write_byte(0x33);
    st7789_write_byte(0x47);
    st7789_write_byte(0x17);
    st7789_write_byte(0x13);
    st7789_write_byte(0x13);
    st7789_write_byte(0x2B);
    st7789_write_byte(0x31);

    st7789_write_cmd(0xE1);
    st7789_write_byte(0xD0);
    st7789_write_byte(0x0A);
    st7789_write_byte(0x11);
    st7789_write_byte(0x0B);
    st7789_write_byte(0x09);
    st7789_write_byte(0x07);
    st7789_write_byte(0x2F);
    st7789_write_byte(0x33);
    st7789_write_byte(0x47);
    st7789_write_byte(0x38);
    st7789_write_byte(0x15);
    st7789_write_byte(0x16);
    st7789_write_byte(0x2C);
    st7789_write_byte(0x32);


    st7789_write_cmd(0xF0);     
    st7789_write_byte(0x3C);   

    st7789_write_cmd(0xF0);     
    st7789_write_byte(0x69);   

    DEV_Delay_ms(120);                

    st7789_write_cmd(0x21);     

    st7789_write_cmd(0x29); 

}


void st7789_set_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    uint8_t buffer[4];

    buffer[0] = x_start >> 8;
    buffer[1] = (uint8_t)(x_start & 0x00ff);
    buffer[2] = x_end >> 8;
    buffer[3] = (uint8_t)(x_end & 0x00ff);
    st7789_write_cmd(0x2a);
    st7789_write_bytes(buffer, 4);

    buffer[0] = y_start >> 8;
    buffer[1] = (uint8_t)(y_start & 0x00ff);
    buffer[2] = y_end >> 8;
    buffer[3] = (uint8_t)(y_end & 0x00ff);
    st7789_write_cmd(0x2b);
    st7789_write_bytes(buffer, 4);
    st7789_write_cmd(0x2c);
}


void st7789_draw_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t width = x_end - x_start + 1;
    uint16_t height = y_end - y_start + 1;
    uint16_t buf[width * 2]; // 每个像素2字节
    uint16_t i, j;
    
    // 填充缓冲区
    for (i = 0; i < width; i++) {
        buf[i * 2] = color >> 8;     // 高8位
        buf[i * 2 + 1] = color & 0xFF; // 低8位
    }
    
    st7789_set_windows(x_start, y_start, x_end, y_end);
    
    // 逐行发送数据
    for (i = 0; i < height; i++) {
        st7789_write_bytes((uint8_t *)buf, width * 2);
    }
}

void st7789_flush(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t *color)
{
    // uint16_t i, j;
    // uint16_t width = x_end - x_start + 1;
    // uint16_t height = y_end - y_start + 1;
    
    // st7789_set_windows(x_start, y_start, x_end, y_end);
    
    // for (i = 0; i < height; i++) {
    //     for (j = 0; j < width; j++) {
    //         uint16_t pixel_color = color[i * width + j];
    //         uint8_t color_h = pixel_color >> 8;
    //         uint8_t color_l = (uint8_t)(pixel_color & 0x00ff);
    //         st7789_write_byte(color_h);
    //         st7789_write_byte(color_l);
    //     }
    // }
}

uint16_t swap_uint16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

void st7789_clear(uint16_t color)
{
    
    uint16_t buf[ST7789_WIDTH * BATCH_LINES];
    uint16_t i, j;
    color = swap_uint16(color);
    
    // 填充缓冲区
    for (i = 0; i < ST7789_WIDTH * BATCH_LINES; i++)
        buf[i] = color;

    st7789_set_windows(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);

    // 批量写入数据
    uint16_t total_batches = ST7789_HEIGHT / BATCH_LINES;
    uint16_t remaining_lines = ST7789_HEIGHT % BATCH_LINES;
    
    // 写入完整的批次
    for (i = 0; i < total_batches; i++) {
        st7789_write_bytes((uint8_t *)buf, ST7789_WIDTH * 2 * BATCH_LINES);
    }
    
    // 写入剩余的行数
    if (remaining_lines > 0) {
        st7789_write_bytes((uint8_t *)buf, ST7789_WIDTH * 2 * remaining_lines);
    }
}


#include "../../Core/Src/image.h"
int testPic(void)
{
    st7789_clear(0xFFFF); // 清屏为白色
    
    // 计算图像显示位置（居中显示）
    int image_width = 150; // 从数组大小推断，150x150像素
    int image_height = 150;
    int x_start = (ST7789_WIDTH - image_width) / 2;
    int y_start = (ST7789_HEIGHT - image_height) / 2;
    int line_size = image_width * 2; // 每行字节数
    
    // 显示 gImage_up
    SEGGER_RTT_WriteString(0, "Displaying gImage_up...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 逐行发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        int index = i * line_size;
        st7789_write_bytes(&gImage_up[index], line_size);
    }
    
    DEV_Delay_ms(500); // 显示2秒
    
    // 显示 gImage_down
    SEGGER_RTT_WriteString(0, "Displaying gImage_down...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 逐行发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        int index = i * line_size;
        st7789_write_bytes(&gImage_down[index], line_size);
    }
    
    DEV_Delay_ms(500); // 显示2秒
    
    // 显示 gImage_right
    SEGGER_RTT_WriteString(0, "Displaying gImage_right...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 逐行发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        int index = i * line_size;
        st7789_write_bytes(&gImage_right[index], line_size);
    }
    
    DEV_Delay_ms(500); // 显示2秒
    
    // 显示 gImage_left
    SEGGER_RTT_WriteString(0, "Displaying gImage_left...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 逐行发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        int index = i * line_size;
        st7789_write_bytes(&gImage_left[index], line_size);
    }
    
    DEV_Delay_ms(500); // 显示2秒
    
    // 显示 gImage_long_press
    SEGGER_RTT_WriteString(0, "Displaying gImage_long_press...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 逐行发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        int index = i * line_size;
        st7789_write_bytes(&gImage_long_press[index], line_size);
    }
    
    DEV_Delay_ms(500); // 显示2秒
    
    // 显示 gImage_double_click
    SEGGER_RTT_WriteString(0, "Displaying gImage_double_click...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 逐行发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        int index = i * line_size;
        st7789_write_bytes(&gImage_double_click[index], line_size);
    }
    
    DEV_Delay_ms(500); // 显示2秒
    
    return 0;
}