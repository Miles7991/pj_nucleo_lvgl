/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2019-07-08
* | Info        :   Basic version
*
******************************************************************************/
#include "DEV_Config.h"
#include "SEGGER_RTT.h"
#include "i2c.h"
#include "main.h"
#include "stm32f4xx_hal_spi.h"
#include "tim.h"
#include "spi.h"
#include "stm32f4xx_hal_tim.h"
#include "cmsis_os.h"
uint32_t fd;
int INT_PIN;

/**
 * @brief 设置背光亮度
 * 
 * 该函数通过设置PWM占空比来控制设备的背光亮度
 * 
 * @param Value 亮度值，范围为0-99，0表示最暗，99表示最亮
 * @return 无返回值
 * 
 * @note 函数使用TIM3的通道4来控制PWM输出
 * @note 亮度值超过99时会输出错误信息
 */
void DEV_SetBacklight(UWORD Value)
{
	// 设置PWM占空比，控制背光亮度
    if (Value <= 99) {
      // 计算PWM比较值（0-999）
      uint32_t compare_value = (Value * CYCLE_MAX) / 100;
      
      // 设置PWM占空比
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, compare_value);
    }
    else
    {
        SEGGER_RTT_WriteString(0,RTT_CTRL_TEXT_RED"Value out of range(0-99)\r\n"RTT_CTRL_RESET);
    }
}

/*****************************************
                GPIO
*****************************************/

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    // 暂时不用，CubeMX已经初始化了
}

/**
 * delay x ms
**/
void DEV_Delay_ms(UDOUBLE xms)
{
    osDelay(xms);
}

static void DEV_GPIO_Init(void)
{
    // 已经在 MX_GPIO_Init 中初始化了
    
    LCD_CS_1;

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}
/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_ModuleInit(void)
{
	DEV_GPIO_Init();

    return 0;
}

void DEV_SPI_WriteByte(uint8_t Value)
{
    LCD_CS_0;
    // HAL_SPI_Transmit(&hspi2, &Value, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit_DMA(&hspi2, &Value, 1);
    // 等待DMA传输完成
    while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
    LCD_CS_1;
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
    // 发送数据 记得先拉低CS
    LCD_CS_0;
    // HAL_SPI_Transmit(&hspi2, pData, Len, HAL_MAX_DELAY);
    HAL_SPI_Transmit_DMA(&hspi2, pData, Len);
    // 等待DMA传输完成
    while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
    LCD_CS_1;
}

/******************************************************************************
function:	I2C Function initialization and transfer
parameter:
Info:
******************************************************************************/
void DEV_I2C_Init(uint8_t Add)
{
    //cubeMX已经初始化了
}




void I2C_Write_nByte(uint16_t Reg_addr, uint8_t* value,uint32_t Length)
{
    HAL_I2C_Mem_Write(&hi2c3, ADDR_I2C_TOUCH_WRITE, Reg_addr, I2C_MEMADD_SIZE_8BIT, value, Length, HAL_MAX_DELAY);


}

void I2C_Read_nByte(uint8_t Reg_addr, uint8_t* value,uint32_t Length)
{
    HAL_I2C_Mem_Read(&hi2c3, ADDR_I2C_TOUCH_READ, Reg_addr, I2C_MEMADD_SIZE_8BIT, value, Length, HAL_MAX_DELAY);
}





/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void)
{
    HAL_I2C_DeInit(&hi2c3);
    HAL_SPI_DeInit(&hspi2);

}
