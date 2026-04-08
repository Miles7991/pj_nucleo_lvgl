/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"
#include "lcd.h"
#include "touchpad.h"
#include "fatfs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void my_print(lv_log_level_t level, const char * buf);
static void btn_event_cb(lv_event_t * e);
static void slider_event_cb(lv_event_t * e);
static lv_obj_t * sldlabel;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t lvglTaskHandle;
const osThreadAttr_t lvglTask_attributes = {
  .name = "lvglTask",
  .stack_size = 512 * 10,
  .priority = (osPriority_t) osPriorityHigh,
};
lv_display_t * my_disp;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 10,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartLvglTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  lvglTaskHandle = osThreadNew(StartLvglTask, NULL, &lvglTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  // FRESULT res;                                          /* FatFs function common result code */
  // uint32_t byteswritten, bytesread;                     /* File write/read counts */
  // uint8_t wtext[] = "Hello from STM32 Nucleo + FATFS + SPI!"; /* File write buffer */
  // uint8_t rtext[_MAX_SS];                               /* File read buffer */

  // SEGGER_RTT_printf(0, "\r\n--- FATFS SPI SD Card Example ---\r\n");

  // /* 1. Mount SD Card */
  // res = f_mount(&USERFatFS, (TCHAR const*)USERPath, 1);
  // if (res != FR_OK) {
  //   SEGGER_RTT_printf(0, "f_mount error: %d\r\n", res);
  // } else {
  //   SEGGER_RTT_printf(0, "f_mount success!\r\n");

  //   /* 2. Create and Open a new text file object with write access */
  //   res = f_open(&USERFile, "NUCLEO.TXT", FA_CREATE_ALWAYS | FA_WRITE);
  //   if (res != FR_OK) {
  //     SEGGER_RTT_printf(0, "f_open (write) error: %d\r\n", res);
  //   } else {
  //     SEGGER_RTT_printf(0, "f_open (write) success!\r\n");

  //     /* 3. Write data to the text file */
  //     res = f_write(&USERFile, wtext, sizeof(wtext), (void *)&byteswritten);
  //     if ((byteswritten == 0) || (res != FR_OK)) {
  //       SEGGER_RTT_printf(0, "f_write error: %d\r\n", res);
  //     } else {
  //       SEGGER_RTT_printf(0, "f_write success! written: %d bytes\r\n", byteswritten);
  //     }

  //     /* 4. Close the open text file */
  //     f_close(&USERFile);
  //   }

  //   /* 5. Open the text file object with read access */
  //   res = f_open(&USERFile, "NUCLEO.TXT", FA_READ);
  //   if (res != FR_OK) {
  //     SEGGER_RTT_printf(0, "f_open (read) error: %d\r\n", res);
  //   } else {
  //     SEGGER_RTT_printf(0, "f_open (read) success!\r\n");

  //     /* 6. Read data from the text file */
  //     res = f_read(&USERFile, rtext, sizeof(rtext) - 1, (UINT*)&bytesread);
  //     if ((bytesread == 0) || (res != FR_OK)) {
  //       SEGGER_RTT_printf(0, "f_read error: %d\r\n", res);
  //     } else {
  //       rtext[bytesread] = '\0'; /* Ensure null-terminated string */
  //       SEGGER_RTT_printf(0, "f_read success! read content: %s\r\n", rtext);
  //     }

  //     /* 7. Close the open text file */
  //     f_close(&USERFile);
  //   }
    
  //   /* 8. Unmount */
  //   // f_mount(NULL, (TCHAR const*)USERPath, 0);
  // }

  /* Infinite loop */
  while(1)
  {
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void StartLvglTask(void *argument)
{
  // 外设初始化
  // touch_data_t touch_data;
  if(DEV_ModuleInit() != 0)
  {
    SEGGER_RTT_printf(0, "DEV_ModuleInit failed.\r\n");
    DEV_ModuleExit();
  }
  
  // lvgl init
#if LV_USE_LOG
  lv_log_register_print_cb(my_print);
#endif
  lv_init();
#if LV_USE_LOG
  // lv_init 会清除 lv_log_register_print_cb 的 cb 需要再次注册 my_print
  lv_log_register_print_cb(my_print);
#endif
  lv_tick_set_cb(HAL_GetTick);
  lv_port_display_init();

  st7789_clear(0XF800);
  DEV_Delay_ms(500);
  st7789_clear(0X400);
  DEV_Delay_ms(500);
  st7789_clear(0xFFFF);

  lv_port_indev_init();
  


// 获取当前屏幕
  lv_obj_t * scr = lv_scr_act();
  // 设置屏幕背景（可选）
  lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
  
  
  // 创建红色方块
  lv_obj_t * test_rect = lv_obj_create(scr);
  lv_obj_set_size(test_rect, 50, 50);
  lv_obj_set_style_bg_color(test_rect, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_set_style_bg_opa(test_rect, LV_OPA_COVER, 0);
  lv_obj_align(test_rect, LV_ALIGN_TOP_LEFT, 0, 0);
  // 创建标签
  lv_obj_t * label = lv_label_create(scr);
  lv_label_set_text(label, "HELLO LVGL");
  lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0); 
  lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0); 
  lv_obj_align_to(label, test_rect, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

  // 创建黄色方块
  lv_obj_t * test_rect1 = lv_obj_create(scr);
  lv_obj_set_size(test_rect1, 50, 50);
  lv_obj_set_style_bg_color(test_rect1, lv_palette_main(LV_PALETTE_YELLOW), 0);
  lv_obj_set_style_bg_opa(test_rect1, LV_OPA_COVER, 0);
  lv_obj_align(test_rect1, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  
  // 创建按钮
  lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
  // lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
  lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 10,   -10);
  lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);           /*Assign a callback to the button*/

  lv_obj_t * bntlabel = lv_label_create(btn);          /*Add a label to the button*/
  lv_label_set_text(bntlabel, "Button");                     /*Set the labels text*/
  lv_obj_center(bntlabel);

/**
 * Create a slider and write its value on a label. 
 */
  lv_obj_t * slider = lv_slider_create(lv_screen_active());
  lv_obj_set_width(slider, 200);                          /*Set the width*/
  lv_obj_center(slider);                                  /*Align to the center of the parent (screen)*/
  lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /*Assign an event function*/

  /*Create a label above the slider*/
  sldlabel = lv_label_create(lv_screen_active());
  lv_slider_set_value(slider, 50, LV_ANIM_OFF);
  lv_label_set_text(sldlabel, "bright:50");
  lv_obj_align_to(sldlabel, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/

  /* Infinite loop */
  for(;;)
  {
    // if (get_touch_data(&touch_data))
    // {
    //   SEGGER_RTT_printf(0, "Touch Screen Detected\r\n");
    //   // printf("x: %d, y: %d \r\n", touch_data.coords[0].x, touch_data.coords[0].y);
    //   st7789_draw_rectangle(touch_data.coords[0].x, touch_data.coords[0].y, touch_data.coords[0].x + 10, touch_data.coords[0].y + 10, 0XF800);
    // }
    // DEV_Delay_ms(10);

    // LVGL MAIN JOB
    lv_timer_handler();
    osDelay(2);

  }
}

void my_print(lv_log_level_t level, const char * buf)
{
    if(__get_IPSR() != 0) return;  // 防止ISR调用

    const char * color;
    const char * tag;

    switch(level) {
        case LV_LOG_LEVEL_TRACE: color = "\x1B[90m"; tag = "[TRACE]"; break;
        case LV_LOG_LEVEL_INFO:  color = "\x1B[32m"; tag = "[INFO]";  break;
        case LV_LOG_LEVEL_WARN:  color = "\x1B[33m"; tag = "[WARN]";  break;
        case LV_LOG_LEVEL_ERROR: color = "\x1B[31m"; tag = "[ERROR]"; break;
        case LV_LOG_LEVEL_USER:  color = "\x1B[36m"; tag = "[USER]";  break;
        default: color = ""; tag = "[UNK]"; break;
    }

    SEGGER_RTT_printf(0, "%s%s\x1B[0m %s", color, tag, buf);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    SEGGER_RTT_printf(0, "Stack overflow: %s\n", pcTaskName);
    while(1);
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target_obj(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}
// 滑动条事件回调函数
static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);

    /*Refresh the text*/
    uint8_t brightness = lv_slider_get_value(slider);
    lv_label_set_text_fmt(sldlabel, "bright:%d", brightness);
    lv_obj_align_to(sldlabel, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
    
    DEV_SetBacklight(brightness);
}

/* USER CODE END Application */

