#include "cst816d.h"


static bool cst816d_read_flag = true;
static bool cst816d_int_enable = true;


static void cst816d_read(uint8_t reg_addr, uint8_t *data, uint32_t length) {
    I2C_Read_nByte(reg_addr, data, length);
}

bool get_touch_data(touch_data_t *touch_data) {
  uint8_t buff[4] = {0};
  uint8_t touch_num = 0;

  if (cst816d_int_enable) {
    if (false == cst816d_read_flag) {
      return false;
    }
    // Note: We'll clear the flag later to ensure we don't miss a touch if read fails
  }

  cst816d_read(CST816D_TOUCH_NUM_REG, &touch_num, 1);
  
  if (touch_num == 0 || touch_num > CST816D_TOUCH_MAX_NUM) {
    if (cst816d_int_enable) cst816d_read_flag = false;
    touch_data->touch_num = 0;
    return false;
  }

  touch_data->touch_num = touch_num;

  // Read all 4 bytes of coordinates in one go
  cst816d_read(CST816D_TOUCH_XH_REG, buff, 4);

  for (uint16_t i = 0; i < touch_num; i++) {
    // For CST816D, each touch point is 6 bytes but we only need the first 4 for X/Y
    // Actually, if we only read 4 bytes into buff, i > 0 would be out of bounds.
    // But since CST816D_TOUCH_MAX_NUM is 1, i will always be 0.
    touch_data->coords[i].x = (((uint16_t)buff[0] & 0x0f) << 8) | buff[1];
    touch_data->coords[i].y = (((uint16_t)buff[2] & 0x0f) << 8) | buff[3];
  }

  if (cst816d_int_enable) cst816d_read_flag = false;

  return true;
}


// 中断服务函数
void cst816d_touch_int_cb(void) {
  cst816d_read_flag = true;
}


void cst816d_touch_rst(void)
{
    TP_RST_0;
    DEV_Delay_ms(100);
    TP_RST_1;
    DEV_Delay_ms(200);
}

void cst816d_init(void)
{
    uint8_t id = 0;
    cst816d_touch_rst();

    cst816d_read(CST816D_ID_REG, &id, 1);
    if (0 != id)
    {
        LCD_DEBUG("cst816d id:%x\r\n", id);
    }
    //wiringPiISR(TP_INT,INT_EDGE_FALLING,&cst816d_touch_int_cb);

    cst816d_int_enable = true;
}

