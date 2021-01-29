/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1232-Hal.c
 * \brief      SX1232 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <stdint.h>
#include <stdbool.h> 

#include "platform.h"

#if defined( USE_SX1232_RADIO )

#include "ioe.h"
#include "spi.h"
#include "../../radio/sx1232-Hal.h"

/*!
 * SX1232 RESET I/O definitions
 */
#if defined( STM32F4XX ) || defined( STM32F2XX )
#define RESET_IOPORT                                GPIOG
#define RESET_PIN                                   GPIO_Pin_12
#else
#define RESET_IOPORT                                GPIOA
#define RESET_PIN                                   GPIO_Pin_1
#endif

/*!
 * SX1232 SPI NSS I/O definitions
 */
#if defined( STM32F4XX ) || defined( STM32F2XX )
#define NSS_IOPORT                                  GPIOA
#define NSS_PIN                                     GPIO_Pin_15
#else
#define NSS_IOPORT                                  GPIOA
#define NSS_PIN                                     GPIO_Pin_15
#endif

/*!
 * SX1232 DIO pins  I/O definitions
 */
#if defined( STM32F4XX ) || defined( STM32F2XX )
#define DIO0_IOPORT                                 GPIOG
#define DIO0_PIN                                    GPIO_Pin_13
#else
#define DIO0_IOPORT                                 GPIOA
#define DIO0_PIN                                    GPIO_Pin_0
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_Pin_8
#else
#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_Pin_0
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
#define DIO2_IOPORT                                 GPIOA
#define DIO2_PIN                                    GPIO_Pin_2
#else
#define DIO2_IOPORT                                 GPIOC
#define DIO2_PIN                                    GPIO_Pin_5
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
#define DIO3_IOPORT                                 
#define DIO3_PIN                                    RF_DIO3_PIN
#else
#define DIO3_IOPORT                                 
#define DIO3_PIN                                    RF_DIO3_PIN
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
#define DIO4_IOPORT                                 
#define DIO4_PIN                                    RF_DIO4_PIN
#else
#define DIO4_IOPORT                                 
#define DIO4_PIN                                    RF_DIO4_PIN
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
#define DIO5_IOPORT                                 
#define DIO5_PIN                                    RF_DIO5_PIN
#else
#define DIO5_IOPORT                                 
#define DIO5_PIN                                    RF_DIO5_PIN
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
#define RXTX_IOPORT                                 
#define RXTX_PIN                                    FEM_CTX_PIN
#else
#define RXTX_IOPORT                                 
#define RXTX_PIN                                    FEM_CTX_PIN
#endif


void SX1232InitIo( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

#if defined( STM32F4XX ) || defined( STM32F2XX )
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                            RCC_AHB1Periph_GPIOG, ENABLE );
#else
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                            RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
#else
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#endif
    
    // Configure NSS as output
    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
    GPIO_InitStructure.GPIO_Pin = NSS_PIN;
    GPIO_Init( NSS_IOPORT, &GPIO_InitStructure );

    // Configure radio DIO as inputs
#if defined( STM32F4XX ) || defined( STM32F2XX )
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#else
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif

    // Configure DIO0
    GPIO_InitStructure.GPIO_Pin =  DIO0_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );
    
    // Configure DIO1
    GPIO_InitStructure.GPIO_Pin =  DIO1_PIN;
    GPIO_Init( DIO1_IOPORT, &GPIO_InitStructure );
    
    // Configure DIO2
    GPIO_InitStructure.GPIO_Pin =  DIO2_PIN;
    GPIO_Init( DIO2_IOPORT, &GPIO_InitStructure );
    
    // REAMARK: DIO3/4/5 configured are connected to IO expander

    // Configure DIO3 as input
    
    // Configure DIO4 as input
    
    // Configure DIO5 as input
}

void SX1232SetReset( uint8_t state )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if( state == RADIO_RESET_ON )
    {
        // Set RESET pin to 1
        GPIO_WriteBit( RESET_IOPORT, RESET_PIN, Bit_SET );

        // Configure RESET as output
#if defined( STM32F4XX ) || defined( STM32F2XX )
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
#else

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
#endif        
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = RESET_PIN;
        GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );
    }
    else
    {
#if FPGA == 0    
        // Configure RESET as input
#if defined( STM32F4XX ) || defined( STM32F2XX )
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
#else
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif        
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin =  RESET_PIN;
        GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );
#else
        GPIO_WriteBit( RESET_IOPORT, RESET_PIN, Bit_RESET );
#endif
    }
}

void SX1232Write( uint8_t addr, uint8_t data )
{
    SX1232WriteBuffer( addr, &data, 1 );
}

void SX1232Read( uint8_t addr, uint8_t *data )
{
    SX1232ReadBuffer( addr, data, 1 );
}

void SX1232WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );

    SpiInOut( addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        SpiInOut( buffer[i] );
    }

    //NSS = 1;
    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
}

void SX1232ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );

    SpiInOut( addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( 0 );
    }

    //NSS = 1;
    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
}

void SX1232WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1232WriteBuffer( 0, buffer, size );
}

void SX1232ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1232ReadBuffer( 0, buffer, size );
}

inline uint8_t SX1232ReadDio0( void )
{
    return GPIO_ReadInputDataBit( DIO0_IOPORT, DIO0_PIN );
}

inline uint8_t SX1232ReadDio1( void )
{
    return GPIO_ReadInputDataBit( DIO1_IOPORT, DIO1_PIN );
}

inline uint8_t SX1232ReadDio2( void )
{
    return GPIO_ReadInputDataBit( DIO2_IOPORT, DIO2_PIN );
}

inline uint8_t SX1232ReadDio3( void )
{
    return GPIO_ReadInputDataBit( DIO3_IOPORT, DIO3_PIN );
}

inline uint8_t SX1232ReadDio4( void )
{
    return GPIO_ReadInputDataBit( DIO4_IOPORT, DIO4_PIN );
}

inline uint8_t SX1232ReadDio5( void )
{
    return GPIO_ReadInputDataBit( DIO5_IOPORT, DIO5_PIN );
}

#endif // USE_SX1232_RADIO
