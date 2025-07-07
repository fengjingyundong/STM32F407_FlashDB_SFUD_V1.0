/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"
#include "sfud.h"
#include "sfud_app.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// #define SFUD_DEMO_TEST_BUFFER_SIZE 2 * 1024
// static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for RunLED */
osThreadId_t RunLEDHandle;
const osThreadAttr_t RunLED_attributes = {
    .name = "RunLED",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for SFUD_Server */
osThreadId_t SFUD_ServerHandle;
const osThreadAttr_t SFUD_Server_attributes = {
    .name = "SFUD_Server",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void RunLEDTask(void *argument);
void SFUD_ServerTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
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
  /* creation of RunLED */
  RunLEDHandle = osThreadNew(RunLEDTask, NULL, &RunLED_attributes);

  /* creation of SFUD_Server */
  SFUD_ServerHandle = osThreadNew(SFUD_ServerTask, NULL, &SFUD_Server_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_RunLEDTask */
/**
 * @brief  Function implementing the RunLED thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_RunLEDTask */
void RunLEDTask(void *argument)
{
  /* USER CODE BEGIN RunLEDTask */

  /* Infinite loop */
  for (;;)
  {
    RED_LED(0);
    GREEN_LED(0);
    osDelay(50);
    RED_LED(1);
    GREEN_LED(1);
    osDelay(50);
    RED_LED(0);
    GREEN_LED(0);
    osDelay(50);
    RED_LED(1);
    GREEN_LED(1);
    osDelay(200);
  }
  /* USER CODE END RunLEDTask */
}

/* USER CODE BEGIN Header_SFUD_ServerTask */
/**
 * @brief Function implementing the SFUD_Server thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_SFUD_ServerTask */
void SFUD_ServerTask(void *argument)
{
  /* USER CODE BEGIN SFUD_ServerTask */
  // if (sfud_init() != SFUD_SUCCESS)
  //   printf("sfud_init False!\r\n");
  // else
  //   sfud_demo(0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);

  /* Infinite loop */
  for (;;)
  {

    osDelay(1000);
  }
  /* USER CODE END SFUD_ServerTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
