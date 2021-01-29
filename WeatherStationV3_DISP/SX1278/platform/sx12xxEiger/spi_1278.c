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
 * \file       spi.c
 * \brief      SPI hardware driver
 *
 * \version    1.0
 * \date       Feb 12 2010
 * \author     Miguel Luis
 */
 
//#include "stm32f10x_spi.h"

#include "spi_1278.h"


//修改成SPI1
//#define SPI_INTERFACE                               SPI2
//#define SPI_CLK                                     RCC_APB1Periph_SPI2

//#define SPI_PIN_SCK_PORT                            GPIOB
//#define SPI_PIN_SCK_PORT_CLK                        RCC_APB2Periph_GPIOB
//#define SPI_PIN_SCK                                 GPIO_Pin_13

//#define SPI_PIN_MISO_PORT                           GPIOB
//#define SPI_PIN_MISO_PORT_CLK                       RCC_APB2Periph_GPIOA
//#define SPI_PIN_MISO                                GPIO_Pin_14

//#define SPI_PIN_MOSI_PORT                           GPIOB
//#define SPI_PIN_MOSI_PORT_CLK                       RCC_APB2Periph_GPIOA
//#define SPI_PIN_MOSI                                GPIO_Pin_15


void SpiInit( void )
{//在CUBE中统一初始化
//    SPI_InitTypeDef SPI_InitStructure;
//    GPIO_InitTypeDef GPIO_InitStructure;

//    /* Enable peripheral clocks --------------------------------------------------*/
//    /* Enable SPIy clock and GPIO clock for SPIy */
//    RCC_APB2PeriphClockCmd( SPI_PIN_MISO_PORT_CLK | SPI_PIN_MOSI_PORT_CLK |
//                            SPI_PIN_SCK_PORT_CLK, ENABLE );
//    RCC_APB1PeriphClockCmd( SPI_CLK, ENABLE );

//    /* GPIO configuration ------------------------------------------------------*/
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

//    GPIO_InitStructure.GPIO_Pin = SPI_PIN_SCK;
//    GPIO_Init( SPI_PIN_SCK_PORT, &GPIO_InitStructure );

//    GPIO_InitStructure.GPIO_Pin = SPI_PIN_MOSI;
//    GPIO_Init( SPI_PIN_MOSI_PORT, &GPIO_InitStructure );

//    GPIO_InitStructure.GPIO_Pin = SPI_PIN_MISO;
//    GPIO_Init( SPI_PIN_MISO_PORT, &GPIO_InitStructure );

//    //禁用JTAG
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );
//    GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );

//    /* SPI_INTERFACE Config -------------------------------------------------------------*/
//    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 72/8 MHz
//    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//    SPI_InitStructure.SPI_CRCPolynomial = 7;
//    SPI_Init( SPI_INTERFACE, &SPI_InitStructure );
//    SPI_Cmd( SPI_INTERFACE, ENABLE );
}

//uint8_t SpiInOut( uint8_t outData )
//{
//    /* Send SPIy data */
//	uint8_t temp_data[1];
//	temp_data[0]=outData;
//   HAL_SPI_Transmit( &hspi2, temp_data,1,0XFFFF);
////   while( SPI_I2S_GetFlagStatus( SPI_INTERFACE, SPI_I2S_FLAG_RXNE ) == RESET );
////   return SPI_I2S_ReceiveData( SPI_INTERFACE );
//}

