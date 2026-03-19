#include "st7789.h"

static void st7789_write_bytes(uint8_t *data, uint32_t lenght)
{
    LCD_DC_1;
    DEV_SPI_Write_nByte(data, lenght);
}

static void st7789_write_byte(uint8_t data)
{
    LCD_DC_1;
    //LCD_2IN8_CS_0;
    DEV_SPI_WriteByte(data);
    //LCD_2IN8_CS_1;
}

static void st7789_write_cmd(uint8_t cmd)
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
    uint8_t color_h = color >> 8;
    uint8_t color_l = (uint8_t)(color && 0x00ff);
    uint16_t i = 0;
    uint16_t color_lenght = (1 + x_end - x_start) * (1 + y_end - y_start);
    st7789_set_windows(x_start, y_start, x_end, y_end);
    for (i = 0; i < color_lenght; i++){
        st7789_write_byte(color_h);
        st7789_write_byte(color_l);
    }
}

void st7789_flush(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t *color)
{
    uint16_t i, j;
    uint16_t width = x_end - x_start + 1;
    uint16_t height = y_end - y_start + 1;
    
    st7789_set_windows(x_start, y_start, x_end, y_end);
    
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            uint16_t pixel_color = color[i * width + j];
            uint8_t color_h = pixel_color >> 8;
            uint8_t color_l = (uint8_t)(pixel_color & 0x00ff);
            st7789_write_byte(color_h);
            st7789_write_byte(color_l);
        }
    }
}

uint16_t swap_uint16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

void st7789_clear(uint16_t color)
{
    
    uint16_t buf[ST7789_WIDTH];
    uint16_t i;
    color = swap_uint16(color);
    for (i = 0; i < ST7789_WIDTH; i++)
        buf[i] = color;

    st7789_set_windows(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);

    for (i = 0; i < ST7789_HEIGHT; i++)
        st7789_write_bytes((uint8_t *)buf ,ST7789_WIDTH * 2);
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
    
    // 显示 gImage_up
    SEGGER_RTT_WriteString(0, "Displaying gImage_up...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 直接发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            // 从 gImage_up 数组中获取像素数据（数组是 8 位颜色格式，每个像素占 2 字节）
            int index = (i * image_width + j) * 2;
            uint8_t color_h = gImage_up[index];
            uint8_t color_l = gImage_up[index + 1];
            st7789_write_byte(color_h);
            st7789_write_byte(color_l);
        }
    }
    
    DEV_Delay_ms(2000); // 显示2秒
    
    // 显示 gImage_down
    SEGGER_RTT_WriteString(0, "Displaying gImage_down...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 直接发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            // 从 gImage_down 数组中获取像素数据（数组是 8 位颜色格式，每个像素占 2 字节）
            int index = (i * image_width + j) * 2;
            uint8_t color_h = gImage_down[index];
            uint8_t color_l = gImage_down[index + 1];
            st7789_write_byte(color_h);
            st7789_write_byte(color_l);
        }
    }
    
    DEV_Delay_ms(2000); // 显示2秒
    
    // 显示 gImage_right
    SEGGER_RTT_WriteString(0, "Displaying gImage_right...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 直接发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            // 从 gImage_right 数组中获取像素数据（数组是 8 位颜色格式，每个像素占 2 字节）
            int index = (i * image_width + j) * 2;
            uint8_t color_h = gImage_right[index];
            uint8_t color_l = gImage_right[index + 1];
            st7789_write_byte(color_h);
            st7789_write_byte(color_l);
        }
    }
    
    DEV_Delay_ms(2000); // 显示2秒
    
    // 显示 gImage_left
    SEGGER_RTT_WriteString(0, "Displaying gImage_left...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 直接发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            // 从 gImage_left 数组中获取像素数据（数组是 8 位颜色格式，每个像素占 2 字节）
            int index = (i * image_width + j) * 2;
            uint8_t color_h = gImage_left[index];
            uint8_t color_l = gImage_left[index + 1];
            st7789_write_byte(color_h);
            st7789_write_byte(color_l);
        }
    }
    
    DEV_Delay_ms(2000); // 显示2秒
    
    // 显示 gImage_long_press
    SEGGER_RTT_WriteString(0, "Displaying gImage_long_press...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 直接发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            // 从 gImage_long_press 数组中获取像素数据（数组是 8 位颜色格式，每个像素占 2 字节）
            int index = (i * image_width + j) * 2;
            uint8_t color_h = gImage_long_press[index];
            uint8_t color_l = gImage_long_press[index + 1];
            st7789_write_byte(color_h);
            st7789_write_byte(color_l);
        }
    }
    
    DEV_Delay_ms(2000); // 显示2秒
    
    // 显示 gImage_double_click
    SEGGER_RTT_WriteString(0, "Displaying gImage_double_click...\r\n");
    st7789_set_windows(x_start, y_start, x_start + image_width - 1, y_start + image_height - 1);
    
    // 直接发送图像数据到屏幕
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; j++) {
            // 从 gImage_double_click 数组中获取像素数据（数组是 8 位颜色格式，每个像素占 2 字节）
            int index = (i * image_width + j) * 2;
            uint8_t color_h = gImage_double_click[index];
            uint8_t color_l = gImage_double_click[index + 1];
            st7789_write_byte(color_h);
            st7789_write_byte(color_l);
        }
    }
    
    DEV_Delay_ms(2000); // 显示2秒
    
    return 0;
}