#ifndef __LED_H__
#define __LED_H__

#define LED_INV                                     1

typedef enum 
{
    LED_GREEN = 0,
    LED_RED = 1,
    LED_DBG1 = 2,
    LED_DBG2 = 3,
#if defined( STM32F4XX ) || defined( STM32F2XX )
    LED_DBG3 = 4,
#endif
} tLed;


#if defined( STM32F4XX ) || defined( STM32F2XX )

#define LED_NB                           5

// RED
#define LED1_PIN                         GPIO_Pin_5
#define LED1_GPIO_PORT                   GPIOC
#define LED1_GPIO_CLK                    RCC_AHB1Periph_GPIOC
// GREEN
#define LED2_PIN                         GPIO_Pin_4
#define LED2_GPIO_PORT                   GPIOC
#define LED2_GPIO_CLK                    RCC_AHB1Periph_GPIOC  
// DBG1
#define LED3_PIN                         GPIO_Pin_8
#define LED3_GPIO_PORT                   GPIOI
#define LED3_GPIO_CLK                    RCC_AHB1Periph_GPIOI  
// DBG2
#define LED4_PIN                         GPIO_Pin_9
#define LED4_GPIO_PORT                   GPIOI
#define LED4_GPIO_CLK                    RCC_AHB1Periph_GPIOI
// DBG3
#define LED5_PIN                         GPIO_Pin_10
#define LED5_GPIO_PORT                   GPIOI
#define LED5_GPIO_CLK                    RCC_AHB1Periph_GPIOI

#else

#define LED_NB                           4

// RED
#define LED1_PIN                         GPIO_Pin_1
#define LED1_GPIO_PORT                   GPIOE
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOE
// GREEN
#define LED2_PIN                         GPIO_Pin_0
#define LED2_GPIO_PORT                   GPIOE
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOE  
// DBG1
#define LED3_PIN                         GPIO_Pin_11
#define LED3_GPIO_PORT                   GPIOE
#define LED3_GPIO_CLK                    RCC_APB2Periph_GPIOE  
// DBG2
#define LED4_PIN                         GPIO_Pin_12
#define LED4_GPIO_PORT                   GPIOE
#define LED4_GPIO_CLK                    RCC_APB2Periph_GPIOE

#endif

#if LED_INV
#define LED_ON                           Bit_RESET
#define LED_OFF                          Bit_SET
#else
#define LED_ON                           Bit_SET
#define LED_OFF                          Bit_RESET
#endif

void LedInit( tLed led );

void LedOn( tLed led );

void LedOff( tLed led );

void LedToggle( tLed led );

#endif // __LED_H__
