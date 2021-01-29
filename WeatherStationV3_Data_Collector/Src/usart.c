/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2021 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
//#include "nbiot.h"
#include "stdio.h"
//#define USART1_REC_LEN 256
#define USART2_REC_LEN 9
int USART_RX_STA=0;
//uint8_t aRxBuffer1[1];
uint8_t aRxBuffer2[1];
//uint8_t USART1_RX_BUF[USART1_REC_LEN];	//??1????
uint8_t USART2_RX_BUF[USART2_REC_LEN];
//volatile uint16_t USART1_RX_LEN=0;		//??1???????
volatile uint16_t USART2_RX_LEN=0;		//??1???????
/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT|UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  huart2.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
  huart2.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA15     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA15     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_15);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	int ret=HAL_BUSY;
//	if(huart->Instance==USART1)															//???????3
//	{
////		if((USART_RX_STA&0x8000)==0)								//接收未完成
////		{
////			if(USART_RX_STA&0x4000)									//接收到了0x0d
////			{
////				if(aRxBuffer1[0]!=0x0a)USART_RX_STA=0;	//接收错误，重新开始接收
////				else USART_RX_STA|=0x8000;							//接收到了0x0a，接收完成
////			}
////			else																			//还未接收到0x0d
////			{
////				if(aRxBuffer1[0]==0x0d)USART_RX_STA|=0x4000;
////				else
////				{
////					USART1_RX_BUF[USART_RX_STA&0X3FFF]=aRxBuffer1[0];
////					USART_RX_STA++;
////					if(USART_RX_STA>(USART1_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
////				}
////			}
////		}
////		HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer1, 1);
//		if(USART1_RX_LEN>=USART1_REC_LEN-1)
//			USART1_RX_LEN=0;
//		else
//			USART1_RX_BUF[USART1_RX_LEN++]=aRxBuffer1[0];			//??????
//		while(ret!=HAL_OK)
//		{
//		ret=HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer1, 1);	//????
//		}
//	}
	if(huart->Instance==USART2)															//???????3
	{
		if(USART2_RX_LEN==9)
		{
			USART2_RX_LEN=0;
		}
		USART2_RX_BUF[USART2_RX_LEN]=aRxBuffer2[0];			//??????
		USART2_RX_LEN++;
		if(aRxBuffer2[0]==0x18&&USART2_RX_BUF[USART2_RX_LEN-2]==0xff&&USART2_RX_LEN>1)
		{
			USART2_RX_BUF[0]=0xff;
			USART2_RX_LEN=2;
		}
		else if(aRxBuffer2[0]==0x18&&USART2_RX_LEN==1&&USART2_RX_BUF[8]==0xff)
		{
			USART2_RX_LEN=2;
		}
		while(ret!=HAL_OK)
		{
		ret=HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer2, 1);	//????
		}
	}

	
}
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 慪es?) calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* ?????????????? */
 // HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

//int _write(int32_t file, char *ptr, int len)
//{
//	int DataIdx;
//	for (DataIdx = 0; DataIdx < len;DataIdx++)
//	{
//	   __io_putchar(*ptr++);
//	}
//	return len;
//}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
