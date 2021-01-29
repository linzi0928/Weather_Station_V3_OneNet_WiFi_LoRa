/**
  ******************************************************************************
  * @file    stm32fxxx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides all exceptions handler and peripherals interrupt
  *          service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32fxxx_it.h"

//#if defined (STM32F2XX) || defined(STM32F4XX)
//    #include "usb_core.h"
//    #include "usbd_core.h"
//    #include "usb_conf.h"
//#elif defined (STM32F10X_HD) || (STM32F10X_XL)
//    #include "usb_lib.h"
//    #include "usb_istr.h"
//#else
//    #error "Missing define: Evaluation board (ie. USE_STM322xG_EVAL)"
//#endif

#include "sx12xxEiger.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

//#if defined( STM32F4XX ) || defined( STM32F2XX )

//extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
//extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

//#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
//extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
//extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
//#endif

//#else

//#endif

/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
//void HardFault_Handler(void)
//{
//  __asm volatile( ".global HardFault_Handler" );
//  __asm volatile( ".extern HardFault_Handler_C" );
//  __asm volatile( "TST LR, #4" );
//  __asm volatile( "ITE EQ" );
//  __asm volatile( "MRSEQ R0, MSP" );
//  __asm volatile( "MRSNE R0, PSP" );
//  __asm volatile( "B HardFault_Handler_C" );
//  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
//}

#include <stdio.h>
// From Joseph Yiu, minor edits by FVH
// hard fault handler in C,
// with stack frame location as input parameter
// called from HardFault_Handler in file xxx.s
void HardFault_Handler_C( unsigned int *args )
{
#if 0
    int8_t s[100];
    unsigned int stacked_r0;
    unsigned int stacked_r1;
    unsigned int stacked_r2;
    unsigned int stacked_r3;
    unsigned int stacked_r12;
    unsigned int stacked_lr;
    unsigned int stacked_pc;
    unsigned int stacked_psr;

    stacked_r0 = ( ( unsigned long) args[0] );
    stacked_r1 = ( ( unsigned long) args[1] );
    stacked_r2 = ( ( unsigned long) args[2] );
    stacked_r3 = ( ( unsigned long) args[3] );

    stacked_r12 = ( ( unsigned long) args[4] );
    stacked_lr = ( ( unsigned long) args[5] );
    stacked_pc = ( ( unsigned long) args[6] );
    stacked_psr = ( ( unsigned long) args[7] );

    sprintf( s, "\n\n[Hard fault handler - all numbers in hex]\n" );
    sprintf( s, "R0 = %x\n", stacked_r0 );
    sprintf( s, "R1 = %x\n", stacked_r1 );
    sprintf( s, "R2 = %x\n", stacked_r2 );
    sprintf( s, "R3 = %x\n", stacked_r3 );
    sprintf( s, "R12 = %x\n", stacked_r12 );
    sprintf( s, "LR [R14] = %x  subroutine call return address\n", stacked_lr );
    sprintf( s, "PC [R15] = %x  program counter\n", stacked_pc );
    sprintf( s, "PSR = %x\n", stacked_psr );
    sprintf( s, "BFAR = %x\n", ( *( ( volatile unsigned long * )( 0xE000ED38 ) ) ) );
    sprintf( s, "CFSR = %x\n", ( *( ( volatile unsigned long * )( 0xE000ED28 ) ) ) );
    sprintf( s, "HFSR = %x\n", ( *( ( volatile unsigned long * )( 0xE000ED2C ) ) ) );
    sprintf( s, "DFSR = %x\n", ( *( ( volatile unsigned long * )( 0xE000ED30 ) ) ) );
    sprintf( s, "AFSR = %x\n", ( *( ( volatile unsigned long * )( 0xE000ED3C ) ) ) );
    sprintf( s, "SCB_SHCSR = %x\n", SCB->SHCSR );
#endif
    /* Go to infinite loop when Hard Fault exception occurs */
    while( 1 )
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
//void BusFault_Handler(void)
//{
//  /* Go to infinite loop when Bus Fault exception occurs */
//  while (1)
//  {
//  }
//}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
__weak void SysTick_Handler(void)
{
    TickCounter++;
}

//#if defined( STM32F4XX ) || defined( STM32F2XX )
/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
//#ifdef USE_USB_OTG_FS  
//void OTG_FS_WKUP_IRQHandler(void)
//{
//  if(USB_OTG_dev.cfg.low_power)
//  {
//    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
//    SystemInit();
//    USB_OTG_UngateClock(&USB_OTG_dev);
//  }
//  EXTI_ClearITPendingBit(EXTI_Line18);
//}
//#endif

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
//#ifdef USE_USB_OTG_HS  
//void OTG_HS_WKUP_IRQHandler(void)
//{
//  if(USB_OTG_dev.cfg.low_power)
//  {
//    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
//    SystemInit();
//    USB_OTG_UngateClock(&USB_OTG_dev);
//  }
//  EXTI_ClearITPendingBit(EXTI_Line20);
//}
//#endif

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
//#ifdef USE_USB_OTG_HS  
//void OTG_HS_IRQHandler(void)
//#else
//void OTG_FS_IRQHandler(void)
//#endif
//{
//  USBD_OTG_ISR_Handler (&USB_OTG_dev);
//}

//#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
///**
//  * @brief  This function handles EP1_IN Handler.
//  * @param  None
//  * @retval None
//  */
//void OTG_HS_EP1_IN_IRQHandler(void)
//{
//  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
//}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
//void OTG_HS_EP1_OUT_IRQHandler(void)
//{
//  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
//}
//#endif

//#else

/**
  * @brief This function handles USB Low Priority interrupts requests.
  * @param  None
  * @retval None
 */
//#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS) || defined (STM32F37X)
//void USB_LP_IRQHandler(void)
//#else
//void USB_LP_CAN1_RX0_IRQHandler(void)
//#endif
//{
//    USB_Istr( );
//}

/**
  * @brief This function handles USB WakeUp interrupt request.
  * @param  None
  * @retval None
 */

//#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)
//void USB_FS_WKUP_IRQHandler(void)
//#else
//void USBWakeUp_IRQHandler(void)
//#endif
//{
//  EXTI_ClearITPendingBit(EXTI_Line18);
//}

//#endif

/******************************************************************************/
/*                 STM32Fxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32fxxx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
