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
#include "src/misc/lv_palette.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
  .stack_size = 128 * 4,
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
  /* USER CODE END RTOS_THREADS */
  lvglTaskHandle = osThreadNew(StartLvglTask, NULL, &lvglTask_attributes);
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
  st7789_init();
  cst816d_init();

  st7789_clear(0XF800);
  DEV_Delay_ms(500);
  st7789_clear(0X400);
  DEV_Delay_ms(500);
  st7789_clear(0xFFFF);
  
  // lvgl init
  lv_init();
  lv_tick_set_cb(HAL_GetTick);
  lv_port_display_init();



  // lv_port_indev_init();



// 获取当前屏幕
  lv_obj_t * scr = lv_scr_act();
  
  // 设置屏幕背景（可选）
  lv_obj_set_style_bg_color(scr, lv_palette_main(LV_PALETTE_NONE), 0);
  
  // 创建标签
  lv_obj_t * label = lv_label_create(scr);
  lv_label_set_text(label, "HELLO STM32");
  lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0); 
  lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0); 
  lv_obj_center(label);
  
  // 创建第二个标签
  lv_obj_t * label1 = lv_label_create(scr);
  lv_label_set_text(label1, "HELLO LVGL");
  lv_obj_set_style_text_font(label1, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(label1, lv_palette_main(LV_PALETTE_BLUE), 0); 
  lv_obj_set_pos(label1, 100, 100);
  
  // 创建红色方块
  lv_obj_t * test_rect = lv_obj_create(lv_scr_act());
  lv_obj_set_size(test_rect, 50, 50);
  lv_obj_set_style_border_color(test_rect, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_set_style_bg_opa(test_rect, LV_OPA_100, 0);  // 明确设置不透明度
  lv_obj_set_pos(test_rect, 0, 0);
  lv_obj_align(test_rect, LV_ALIGN_TOP_LEFT, 0, 0);
  
  // 创建黄色方块
  lv_obj_t * test_rect1 = lv_obj_create(lv_scr_act());
  lv_obj_set_size(test_rect1, 50, 50);
  lv_obj_set_style_border_color(test_rect1, lv_palette_main(LV_PALETTE_YELLOW), 0);
  lv_obj_set_style_bg_color(test_rect1, lv_palette_main(LV_PALETTE_YELLOW), 0);
  lv_obj_set_style_bg_opa(test_rect1, LV_OPA_100, 0);  // 明确设置不透明度
  lv_obj_align(test_rect1, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  


  



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

osStatus_t osThreadDetach (osThreadId_t thread_id)
{
    (void)thread_id;
    return osOK;
}
/* USER CODE END Application */

