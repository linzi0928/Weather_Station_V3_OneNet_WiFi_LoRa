/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
extern unsigned char USART1_RX_BUF[512];
extern unsigned char USART3_RX_BUF[128];
extern uint32_t USART1_RX_STA;
uint32_t USART3_RX_STA;
uint16_t USART_COUNTER = 0;
uint8_t TempChar = 0,TempChar2 = 0;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#if 1
 
#pragma import(__use_no_semihosting)                             
 
struct __FILE
 
{
 
int handle;
 
};
 
FILE __stdout;      
 
void _sys_exit(int x)
 
{
 
   x = x;
 
}
 
int fputc(int ch, FILE *f)
 
{     
 
     while((USART1->SR&0X40)==0);  
 
    USART1->DR = (uint8_t) ch;     
 
     return ch;
 
}
 
#endif
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
			USART1_RX_BUF[USART1_RX_STA] = TempChar;
			USART1_RX_STA++;
			if(USART1_RX_STA>=512)
				USART1_RX_STA=0;
//			if((USART1_RX_STA&0x8000)==0)//接收未完成
//			{
//			if(USART1_RX_STA&0x4000)//接收到了0x0d
//				{
//				if(TempChar!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
//				else 
//					{
//						USART1_RX_STA|=0x8000;	//接收完成了 
//					}
//				}
//			else //还没收到0X0D
//				{	
//				if(TempChar==0x0d)USART1_RX_STA|=0x4000;
//				else
//					{
//					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=TempChar ;
//					USART1_RX_STA++;
//					if(USART1_RX_STA>(100-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
//					}		 
//				}
//			}  
				HAL_UART_Receive_IT(&huart1, &TempChar, 1);
    }
		if (huart->Instance == USART3) {
//        if(!USART3_RX_STA) {
//            if (TempChar2!=0x0d) {
//                USART3_RX_BUF[USART3_RX_STA++] = TempChar2;
//            }
//            else {
//                USART3_RX_BUF[USART3_RX_STA++] = TempChar2;
//                if (USART3_RX_BUF[USART3_RX_STA + 1] == 0)
//                    USART3_RX_STA|=1<<7;
//            }
//        }
//        else {
//            USART3_RX_BUF[USART3_RX_STA++] = TempChar2;
//           // USART3_RX_STA = 0;
//					
//        }
//				
			if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
				{
				if(TempChar2!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else USART3_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(TempChar2==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=TempChar2 ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(100-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		
				HAL_UART_Receive_IT(&huart3, &TempChar2, 1);
    }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
