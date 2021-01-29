#ifndef __IOE_H__
#define __IOE_H__

#include "stdint.h"

typedef enum 
{
    RF_DIO3_PIN = 0,
    RF_DIO4_PIN,
    RF_DIO5_PIN,
    RF_RXTX_PIN,
    ADC_READY_PIN,
    IOE2_NINT_PIN,
    SHDN_VSUPC_PIN,
    SHDN_VFEM_PIN,
    FEM_ANT_SEL_PIN,
    FEM_CSD_PIN,
    FEM_CPS_PIN,
    FEM_CTX_PIN,
    FEM_SPARE_PIN,
    IOE_SPARE1,
    IOE_SPARE2,
    IOE_SPARE3,
    LED_1_PIN,
    LED_2_PIN,
    LED_3_PIN,
    TEMP_ALERT_PIN,
    IOE_SPARE4,
    IOE_SPARE5,
    IOE_SPARE6,
    IOE_SPARE7,
    SX_GPIO0_PIN,
    SX_GPIO1_PIN,
    SX_GPIO2_PIN,
    SX_GPIO3_PIN,
    SX_GPIO4_PIN,
    SX_GPIO5_PIN,
    SX_GPIO6_PIN,
    SX_GPIO7_PIN,
}tIoePin;

/*
 * Pins
 */
/** 
  * @brief  IO Expander Interrupt line on EXTI  
  */ 
#if defined( STM32F4XX ) || defined( STM32F2XX )

#define IOE_IT_PIN                                  GPIO_Pin_4
#define IOE_IT_GPIO_PORT                            GPIOA
#define IOE_IT_GPIO_CLK                             RCC_AHB1Periph_GPIOA
#define IOE_IT_EXTI_PORT_SOURCE                     EXTI_PortSourceGPIOA
#define IOE_IT_EXTI_PIN_SOURCE                      EXTI_PinSource4
#define IOE_IT_EXTI_LINE                            EXTI_Line4
#define IOE_IT_EXTI_IRQn                            EXTI4_IRQn   

#else

#define IOE_IT_PIN                                  GPIO_Pin_4
#define IOE_IT_GPIO_PORT                            GPIOC
#define IOE_IT_GPIO_CLK                             RCC_APB2Periph_GPIOC
#define IOE_IT_EXTI_PORT_SOURCE                     GPIO_PortSourceGPIOC
#define IOE_IT_EXTI_PIN_SOURCE                      GPIO_PinSource4
#define IOE_IT_EXTI_LINE                            EXTI_Line4
#define IOE_IT_EXTI_IRQn                            EXTI4_IRQn   

#endif

// IOE1 Port A
#define IOE_RF_DIO3_PIN_MASK                        0xFE
#define IOE_RF_DIO4_PIN_MASK                        0xFD
#define IOE_RF_DIO5_PIN_MASK                        0xFB
#define IOE_RF_RXTX_PIN_MASK                        0xF7
#define IOE_ADC_READY_PIN_MASK                      0xEF
#define IOE_IOE2_NINT_PIN_MASK                      0xDF
#define IOE_SHDN_VSUPC_PIN_MASK                     0xBF
#define IOE_SHDN_VFEM_PIN_MASK                      0x7F

#define IOE_RF_DIO3_PIN                             0x01
#define IOE_RF_DIO4_PIN                             0x02
#define IOE_RF_DIO5_PIN                             0x04
#define IOE_RF_RXTX_PIN                             0x08
#define IOE_ADC_READY_PIN                           0x10
#define IOE_IOE2_NINT_PIN                           0x20
#define IOE_SHDN_VSUPC_PIN                          0x40
#define IOE_SHDN_VFEM_PIN                           0x80

// IOE1 Port B
#define IOE_FEM_ANT_SEL_PIN_MASK                    0xFE
#define IOE_FEM_CSD_PIN_MASK                        0xFD
#define IOE_FEM_CPS_PIN_MASK                        0xFB
#define IOE_FEM_CTX_PIN_MASK                        0xF7
#define IOE_FEM_SPARE_PIN_MASK                      0xEF

#define IOE_FEM_ANT_SEL_PIN                         0x01
#define IOE_FEM_CSD_PIN                             0x02
#define IOE_FEM_CPS_PIN                             0x04
#define IOE_FEM_CTX_PIN                             0x08
#define IOE_FEM_SPARE_PIN                           0x10

// IOE2 Port A
#define IOE_LED_1_PIN_MASK                          0xFE
#define IOE_LED_2_PIN_MASK                          0xFD
#define IOE_LED_3_PIN_MASK                          0xFB
#define IOE_TEMP_ALERT_PIN_MASK                     0xF7

#define IOE_LED_1_PIN                               0x01
#define IOE_LED_2_PIN                               0x02
#define IOE_LED_3_PIN                               0x04
#define IOE_TEMP_ALERT_PIN                          0x08

// IOE2 Port B
#define IOE_SX_GPIO0_PIN_MASK                       0xFE
#define IOE_SX_GPIO1_PIN_MASK                       0xFD
#define IOE_SX_GPIO2_PIN_MASK                       0xFB
#define IOE_SX_GPIO3_PIN_MASK                       0xF7
#define IOE_SX_GPIO4_PIN_MASK                       0xEF
#define IOE_SX_GPIO5_PIN_MASK                       0xDF
#define IOE_SX_GPIO6_PIN_MASK                       0xBF
#define IOE_SX_GPIO7_PIN_MASK                       0x7F

#define IOE_SX_GPIO0_PIN                            0x01
#define IOE_SX_GPIO1_PIN                            0x02
#define IOE_SX_GPIO2_PIN                            0x04
#define IOE_SX_GPIO3_PIN                            0x08
#define IOE_SX_GPIO4_PIN                            0x10
#define IOE_SX_GPIO5_PIN                            0x20
#define IOE_SX_GPIO6_PIN                            0x40
#define IOE_SX_GPIO7_PIN                            0x80

void IoeInit( void );

void IoePinOn( tIoePin pin );

void IoePinOff( tIoePin pin );

void IoePinToggle( tIoePin pin );

uint8_t IoePinGet( tIoePin pin );

#endif // __IOE_H__
