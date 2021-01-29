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
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
//#include <stdint.h>
//#include <stdbool.h> 

#include "platform.h"

#if defined( USE_SX1276_RADIO )

//#include "ioe.h"
#include "spi_1278.h"
#include "../../radio/sx1276-Hal.h"

/*!
 * SX1276 RESET I/O definitions
 */
#define RESET_IOPORT                                GPIOB
#define RESET_PIN                                   GPIO_PIN_1

/*!
 * SX1276 SPI NSS I/O definitions
 */
#define NSS_IOPORT                                  GPIOB
#define NSS_PIN                                     GPIO_PIN_9    //ԭGPIO_PIN_15

/*!
 * SX1276 DIO pins  I/O definitions
 */
#define DIO0_IOPORT                                 GPIOB
#define DIO0_PIN                                    GPIO_PIN_2

#define DIO1_IOPORT                                 GPIOC
#define DIO1_PIN                                    GPIO_PIN_6

#define DIO2_IOPORT                                 GPIOB
#define DIO2_PIN                                    GPIO_PIN_8



#define DIO3_IOPORT                                 GPIOC
#define DIO3_PIN                                    GPIO_PIN_3

#define DIO4_IOPORT                                 GPIOC
#define DIO4_PIN                                    GPIO_PIN_4

#define DIO5_IOPORT                                 GPIOC
#define DIO5_PIN                                    GPIO_PIN_5

#define RXTX_IOPORT                                 
#define RXTX_PIN                                    FEM_CTX_PIN


void Set_RF_Switch_RX(void)
{
//	RXE_HIGH();
//	TXE_LOW();
}

void Set_RF_Switch_TX(void)
{
//	RXE_LOW();
//	TXE_HIGH();
}


void SX1276InitIo( void )
{
//    GPIO_InitTypeDef GPIO_InitStructure;


//    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
//                            RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );

//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     // Configure SPI-->NSS as output
//    GPIO_InitStructure.GPIO_PIN = NSS_PIN;
//    GPIO_Init( NSS_IOPORT, &GPIO_InitStructure );
//	HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_SET );

//	
//	//配置射频开关芯片控制管脚   RXE-->CTRL   TXE--> /CTRL
//	GPIO_InitStructure.GPIO_PIN = RXE_PIN;
//	GPIO_Init(RXE_PORT, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_PIN = TXE_PIN;
//	GPIO_Init(TXE_PORT, &GPIO_InitStructure);	
//	//默认设设置为接收状态
//	Set_RF_Switch_RX();	
//	
//    // Configure radio DIO as inputs
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

//    // Configure DIO0
//    GPIO_InitStructure.GPIO_PIN =  DIO0_PIN;
//    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );
//    
//    // Configure DIO1
//    GPIO_InitStructure.GPIO_PIN =  DIO1_PIN;
//    GPIO_Init( DIO1_IOPORT, &GPIO_InitStructure );
//    
//    // Configure DIO2
//    GPIO_InitStructure.GPIO_PIN =  DIO2_PIN;
//    GPIO_Init( DIO2_IOPORT, &GPIO_InitStructure );
//    
//    // REAMARK: DIO3/4/5 configured are connected to IO expander

//    // Configure DIO3 as input
//    GPIO_InitStructure.GPIO_PIN =  DIO3_PIN;
//    GPIO_Init( DIO3_IOPORT, &GPIO_InitStructure );
//    // Configure DIO4 as input
//    GPIO_InitStructure.GPIO_PIN =  DIO4_PIN;
//    GPIO_Init( DIO4_IOPORT, &GPIO_InitStructure );
//    // Configure DIO5 as input
//	GPIO_InitStructure.GPIO_PIN =  DIO5_PIN;
//    GPIO_Init( DIO5_IOPORT, &GPIO_InitStructure );
}

void SX1276SetReset( uint8_t state )
{

    if( state == RADIO_RESET_ON )
    {
        // Configure RESET as output
		
		// Set RESET pin to 0
        HAL_GPIO_WritePin( RESET_IOPORT, RESET_PIN, GPIO_PIN_RESET );
    }
    else
    {
		
		// Set RESET pin to 1
        HAL_GPIO_WritePin( RESET_IOPORT, RESET_PIN, GPIO_PIN_SET );

    }
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t temp_addr[1];
	temp_addr[0]=addr | 0x80;
    //NSS = 0;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_RESET );

//    SpiInOut( addr | 0x80 );
//    for( i = 0; i < size; i++ )
//    {
//        SpiInOut( buffer[i] );
//    }
		HAL_SPI_Transmit( &hspi2, temp_addr,1,0XFFFF);
		HAL_SPI_Transmit( &hspi2, buffer,size,0XFFFF);
    //NSS = 1;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_SET );
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t temp_addr[1];
		temp_addr[0]=addr & 0x7F ;
    //NSS = 0;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_RESET );

		HAL_SPI_Transmit( &hspi2, temp_addr,1,0XFFFF);
		HAL_SPI_Receive(&hspi2,buffer,size,0XFFFF);
//    SpiInOut( addr & 0x7F );

//    for( i = 0; i < size; i++ )
//    {
//        buffer[i] = SpiInOut( 0 );
//    }

    //NSS = 1;
    HAL_GPIO_WritePin( NSS_IOPORT, NSS_PIN, GPIO_PIN_SET );
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

inline uint8_t SX1276ReadDio0( void )
{
    return HAL_GPIO_ReadPin( DIO0_IOPORT, DIO0_PIN );
}

inline uint8_t SX1276ReadDio1( void )
{
    return HAL_GPIO_ReadPin( DIO1_IOPORT, DIO1_PIN );
}

inline uint8_t SX1276ReadDio2( void )
{
    return HAL_GPIO_ReadPin( DIO2_IOPORT, DIO2_PIN );
}

inline uint8_t SX1276ReadDio3( void )
{
    return HAL_GPIO_ReadPin( DIO3_IOPORT, DIO3_PIN );
}

inline uint8_t SX1276ReadDio4( void )
{
    return HAL_GPIO_ReadPin( DIO4_IOPORT, DIO4_PIN );
}

inline uint8_t SX1276ReadDio5( void )
{
    return HAL_GPIO_ReadPin( DIO5_IOPORT, DIO5_PIN );
}


//射频芯片收发切换
inline void SX1276WriteRxTx( uint8_t txEnable )
{
    if( txEnable != 0 )
    {
		Set_RF_Switch_TX(); //单片机将射频开关芯片切换成发射状态
//        IoePinOn( FEM_CTX_PIN );
//        IoePinOff( FEM_CPS_PIN );
    }
    else
    {
		Set_RF_Switch_RX();  //单片机将射频开关芯片切换成接收状态
//        IoePinOff( FEM_CTX_PIN );
//        IoePinOn( FEM_CPS_PIN );
    }
}

#endif // USE_SX1276_RADIO
