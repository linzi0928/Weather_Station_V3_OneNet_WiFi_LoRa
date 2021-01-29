/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint8_t TempChar,TempChar2;
extern uint16_t USART_COUNTER;
unsigned char EncoderKey_down=0;
unsigned char _EncoderKey_num=0;
unsigned char EncoderKey_flag=0;
#define MaxTurningPwm    30000
#define MinTurningPwm    0
uint16_t step=15000;
 
#define EncoderA_INT_GPIO_PORT         GPIOB
#define EncoderA_INT_GPIO_PIN          GPIO_PIN_5

#define EncoderB_GPIO_PORT             GPIOB			   
#define EncoderB_GPIO_PIN		           GPIO_PIN_6

#define EncoderKey_GPIO_PORT           GPIOB			   
#define EncoderKey_GPIO_PIN		         GPIO_PIN_7

#define EncoderKey 				HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)

#define Encoder_ON	1
#define Encoder_OFF	0

void Encoder_Config(void);
int EncoderKey_Scan(void); 
void EncoderKey_cond(void);
void EncoderKey_chuli(void);
void Delay_EncoderK(__IO uint32_t nCount);
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
	if(HAL_GPIO_ReadPin(EncoderB_GPIO_PORT,EncoderB_GPIO_PIN) == Encoder_ON )  
		{	 
			if(step < MaxTurningPwm)
			{
					step+=5;
				//printf("\r\n??:%d ", step);		 
			}
			else step=15000;
		}
		else
		{	 
				if(step > MinTurningPwm)
			{
					step-=5;
			//	printf("\r\n??:%d ", step);		 
			}
				else step=15000;
		}	
  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
	//HAL_UART_Receive_IT(&huart1, &TempChar, 1);
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */
	//HAL_UART_Receive_IT(&huart3, &TempChar2, 1);
  /* USER CODE END USART3_IRQn 1 */
}

/* USER CODE BEGIN 1 */
int EncoderKey_Scan(void)
{	
/***************************?????????????????**************************************/
if(EncoderKey  == Encoder_OFF ) 
	{	   
		Delay_EncoderK(10000);		
		if(EncoderKey == Encoder_OFF )  
		{	
			while(EncoderKey == Encoder_OFF);   
			
			return 	Encoder_ON;	 
			
		}
		else
			return Encoder_OFF;
	}
	else

		return Encoder_OFF;		

}
void Delay_EncoderK(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
