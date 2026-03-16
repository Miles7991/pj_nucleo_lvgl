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
#include "FreeRTOS.h"
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
    // DEV_GPIO_Mode(LCD_CS, 1);
    // DEV_GPIO_Mode(LCD_RST, 1);
    // DEV_GPIO_Mode(LCD_DC, 1);
    // DEV_GPIO_Mode(LCD_BL, 1);

    // DEV_GPIO_Mode(TP_INT, 0);
    // DEV_GPIO_Mode(TP_RST, 1);
    
    LCD_CS_1;
	// LCD_BL_1;
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}
/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_ModuleInit(void)
{

 #ifdef USE_BCM2835_LIB
    if(!bcm2835_init()) {
        printf("bcm2835 init failed  !!! \r\n");
        return 1;
    } else {
        printf("bcm2835 init success !!! \r\n");
    }
    DEV_GPIO_Init();
    bcm2835_spi_begin();                                         //Start spi interface, set spi pin for the reuse function
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //High first transmission
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                  //spi mode 0
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);  //Frequency
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                     //set CE0
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);     //enable cs0
	
	bcm2835_gpio_fsel(LCD_BL, BCM2835_GPIO_FSEL_ALT5);
    bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
    
	bcm2835_pwm_set_mode(0, 1, 1);
    bcm2835_pwm_set_range(0,1024);
	bcm2835_pwm_set_data(0,512);
	
#elif USE_WIRINGPI_LIB  
    //if(wiringPiSetup() < 0)//use wiringpi Pin number table  
    if(wiringPiSetupGpio() < 0) { //use BCM2835 Pin number table
        DEBUG("set wiringPi lib failed	!!! \r\n");
        return 1;
    } else {
        DEBUG("set wiringPi lib success  !!! \r\n");
    }
    DEV_GPIO_Init();
    wiringPiSPISetup(0,40000000);
	pinMode (LCD_BL, PWM_OUTPUT);
    pwmWrite(LCD_BL,512);
#elif SCREENSTM
	DEV_GPIO_Init();
    // LCD_BL 设置PWM亮度
    //DEV_SetBacklight(50);
#endif
    //设置从机地址，STM32不用提前设置从机地址
    // DEV_I2C_Init(0x15);
    return 0;
}

void DEV_SPI_WriteByte(uint8_t Value)
{
#ifdef USE_BCM2835_LIB
    bcm2835_spi_transfer(Value);
    
#elif USE_WIRINGPI_LIB
    wiringPiSPIDataRW(0,&Value,1);
#elif SCREENSTM
    //HAL_SPI_Transmit(&hspi3, &Value, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi2, &Value, 1, HAL_MAX_DELAY);
#endif
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
    uint8_t Data[Len];
    memcpy(Data, pData,  Len);
#ifdef USE_BCM2835_LIB
    uint8_t rData[Len];
    bcm2835_spi_transfernb((char *)Data,(char *)rData,Len);
    
#elif USE_WIRINGPI_LIB
    wiringPiSPIDataRW(0, (unsigned char *)Data, Len);
#elif SCREENSTM
    HAL_SPI_Transmit(&hspi2, Data, Len, HAL_MAX_DELAY);
#endif
}

/******************************************************************************
function:	I2C Function initialization and transfer
parameter:
Info:
******************************************************************************/
void DEV_I2C_Init(uint8_t Add)
{

#ifdef USE_BCM2835_LIB
    printf("BCM2835 I2C Device\r\n");  
    bcm2835_i2c_begin();
    bcm2835_i2c_setSlaveAddress(Add);
    
#elif USE_WIRINGPI_LIB
    printf("WIRINGPI I2C Device\r\n");       
    fd = wiringPiI2CSetup(Add);
#elif SCREENSTM
    //HAL_I2C_Master_Transmit(&hi2c3, Add, (uint8_t *)value, Length, HAL_MAX_DELAY);
#endif

}




void I2C_Write_nByte(uint16_t Reg_addr, uint8_t* value,uint32_t Length)
{
	// int ref;
    // 获取Reg_addr的高8位和低8位
    uint8_t high_byte = (Reg_addr >> 8) & 0xFF; // 获取Reg_addr的高8位
    uint8_t low_byte = Reg_addr & 0xFF; // 获取Reg_addr的低8位
#ifdef USE_BCM2835_LIB
    printf("BCM2835\r\n");
    char wbuf[2]={high_byte, low_byte};
    char Data[2+Length];
    memcpy(Data, wbuf, 2);
    memcpy(Data + 2, value, Length);
    bcm2835_i2c_write(Data, 2 + Length);
#elif USE_WIRINGPI_LIB
    wiringPiI2CWriteReg8(fd, high_byte, low_byte);
#elif SCREENSTM
    char wbuf[2]={high_byte, low_byte};
    char Data[2+Length];
    memcpy(Data, wbuf, 2);
    memcpy(Data + 2, value, Length);
    HAL_I2C_Master_Transmit(&hi2c3, ADDR_I2C_TOUCH, (uint8_t *)Data, Length+2, HAL_MAX_DELAY);
#endif

}

void I2C_Read_nByte(uint8_t Reg_addr, uint8_t* value,uint32_t Length)
{
	// int ref;
    // 获取Reg_addr的高8位和低8位
    uint8_t high_byte = (Reg_addr >> 8) & 0xFF; // 获取Reg_addr的高8位
    uint8_t low_byte = Reg_addr & 0xFF; // 获取Reg_addr的低8位
#ifdef USE_BCM2835_LIB
    printf("BCM2835\r\n");
    // char wbuf[2]={high_byte, low_byte};
    // bcm2835_i2c_read_register_rs(wbuf,(char*) value, Length);
    
#elif USE_WIRINGPI_LIB
    // printf("WIRINGPI -Read\r\n");
    wiringPiI2CWrite(fd, (int)Reg_addr); // 写入16位寄存器地址
    for (uint32_t i = 0; i < Length; i++) {
        value[i] = wiringPiI2CRead(fd);
    }
#elif SCREENSTM
    char wbuf[2]={high_byte, low_byte};
    char Data[2+Length];
    memcpy(Data, wbuf, 2);
    HAL_I2C_Master_Transmit(&hi2c3, ADDR_I2C_TOUCH, (uint8_t *)Data, 2, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c3, ADDR_I2C_TOUCH, (uint8_t *)Data+2, Length, HAL_MAX_DELAY);
    memcpy(value, Data+2, Length);
#endif
}





/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void)
{
#ifdef USE_BCM2835_LIB
    bcm2835_spi_end();
    bcm2835_close();
#elif USE_WIRINGPI_LIB
#elif SCREENSTM
    HAL_I2C_DeInit(&hi2c3);
    HAL_SPI_DeInit(&hspi2);
#endif
}
