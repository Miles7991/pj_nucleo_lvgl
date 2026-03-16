/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2019-07-08
* | Info        :   Basic version
*
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "Debug.h"

// hal库默认在main.h中包含了spi、i2c、gpio、tim的头文件
// #include "stm32f4xx_hal_spi.h"
// #include "stm32f4xx_hal_i2c.h"
// #include "stm32f4xx_hal_gpio.h"
// #include "stm32f4xx_hal_tim.h"
#include "main.h"

#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>



/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t


#define true   1  
#define false  0

// #define LCD_CS   8
// #define LCD_RST  27
// #define LCD_DC   25
// #define LCD_BL   18

// #define TP_INT   4
// #define TP_RST   17

//LCD
#define LCD_CS_0		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_1		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)

#define LCD_RST_0		HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)
#define LCD_RST_1		HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)

#define LCD_DC_0		HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_1		HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)

// #define LCD_BL_0		HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)
// #define LCD_BL_1		HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)

#define TP_RST_0		HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_RESET)
#define TP_RST_1		HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_SET)

#define LCD_SetBacklight(Value) DEV_SetBacklight(Value)

/*------------------------------------------------------------------------------------------------------*/
UBYTE DEV_ModuleInit(void);
void DEV_ModuleExit(void);

// void DEV_GPIO_Mode(UWORD Pin, UWORD Mode); use HAL_GPIO_Mode
// void DEV_Digital_Write(UWORD Pin, UBYTE Value);  use HAL_GPIO_WritePin
// UBYTE DEV_Digital_Read(UWORD Pin);  use HAL_GPIO_ReadPin

void DEV_Delay_ms(UDOUBLE xms);

void DEV_I2C_Init(uint8_t Add);
void I2C_Write_nByte(uint16_t Reg_addr, uint8_t* value,uint32_t Length);
void I2C_Read_nByte(uint8_t Reg_addr, uint8_t* value,uint32_t Length);

void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);
void DEV_SetBacklight(UWORD Value);
#endif
