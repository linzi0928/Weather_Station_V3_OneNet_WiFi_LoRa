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
 * \file       sx1232-Hal.h
 * \brief      SX1232 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __SX1232_HAL_H__
#define __SX1232_HAL_H__

#include "ioe.h"
/*!
 * DIO state read functions mapping
 */
#define DIO0                                        SX1232ReadDio0( )
#define DIO1                                        SX1232ReadDio1( )
#define DIO2                                        SX1232ReadDio2( )
#define DIO3                                        SX1232ReadDio3( )
#define DIO4                                        SX1232ReadDio4( )
#define DIO5                                        SX1232ReadDio5( )


#define GET_TICK_COUNT( )                           ( TickCounter )
#define TICK_RATE_MS( ms )                          ( ms )

typedef enum
{
    RADIO_RESET_OFF,
    RADIO_RESET_ON,
}tRadioResetState;

/*!
 * \brief Initializes the radio interface I/Os
 */
void SX1232InitIo( void );

/*!
 * \brief Set the radio reset pin state
 * 
 * \param state New reset pin state
 */
void SX1232SetReset( uint8_t state );

/*!
 * \brief Writes the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \param [IN]: data New register value
 */
void SX1232Write( uint8_t addr, uint8_t data );

/*!
 * \brief Reads the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \param [OUT]: data Register value
 */
void SX1232Read( uint8_t addr, uint8_t *data );

/*!
 * \brief Writes multiple radio registers starting at address
 *
 * \param [IN] addr   First Radio register address
 * \param [IN] buffer Buffer containing the new register's values
 * \param [IN] size   Number of registers to be written
 */
void SX1232WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads multiple radio registers starting at address
 *
 * \param [IN] addr First Radio register address
 * \param [OUT] buffer Buffer where to copy the registers data
 * \param [IN] size Number of registers to be read
 */
void SX1232ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Writes the buffer contents to the radio FIFO
 *
 * \param [IN] buffer Buffer containing data to be put on the FIFO.
 * \param [IN] size Number of bytes to be written to the FIFO
 */
void SX1232WriteFifo( uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads the contents of the radio FIFO
 *
 * \param [OUT] buffer Buffer where to copy the FIFO read data.
 * \param [IN] size Number of bytes to be read from the FIFO
 */
void SX1232ReadFifo( uint8_t *buffer, uint8_t size );

/*!
 * \brief Gets the SX1232 DIO0 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1232ReadDio0( void );

/*!
 * \brief Gets the SX1232 DIO1 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1232ReadDio1( void );

/*!
 * \brief Gets the SX1232 DIO2 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1232ReadDio2( void );

/*!
 * \brief Gets the SX1232 DIO3 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1232ReadDio3( void );

/*!
 * \brief Gets the SX1232 DIO4 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1232ReadDio4( void );

/*!
 * \brief Gets the SX1232 DIO5 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1232ReadDio5( void );

#endif //__SX1232_HAL_H__