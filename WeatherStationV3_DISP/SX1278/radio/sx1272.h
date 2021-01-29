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
 * \file       sx1272.h
 * \brief      SX1272 RF chip driver
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __SX1272_H__
#define __SX1272_H__

/*!
 * \brief SX1272 registers array
 */
extern uint8_t SX1272Regs[0x70];

/*!
 * \brief Enables LoRa modem or FSK modem
 *
 * \param [IN] opMode New operating mode
 */
void SX1272SetLoRaOn( bool enable );

/*!
 * \brief Gets the LoRa modem state
 *
 * \retval LoraOn Current LoRa modem mode
 */
bool SX1272GetLoRaOn( void );

/*!
 * \brief Initializes the SX1272
 */
void SX1272Init( void );

/*!
 * \brief Resets the SX1272
 */
void SX1272Reset( void );

/*!
 * \brief Sets the SX1272 operating mode
 *
 * \param [IN] opMode New operating mode
 */
void SX1272SetOpMode( uint8_t opMode );

/*!
 * \brief Gets the SX1272 operating mode
 *
 * \retval opMode Current operating mode
 */
uint8_t SX1272GetOpMode( void );

/*!
 * \brief Reads the current Rx gain setting
 *
 * \retval rxGain Current gain setting
 */
uint8_t SX1272ReadRxGain( void );

/*!
 * \brief Trigs and reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
double SX1272ReadRssi( void );

/*!
 * \brief Gets the Rx gain value measured while receiving the packet
 *
 * \retval rxGainValue Current Rx gain value
 */
uint8_t SX1272GetPacketRxGain( void );

/*!
 * \brief Gets the SNR value measured while receiving the packet
 *
 * \retval snrValue Current SNR value in [dB]
 */
int8_t SX1272GetPacketSnr( void );

/*!
 * \brief Gets the RSSI value measured while receiving the packet
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
double SX1272GetPacketRssi( void );

/*!
 * \brief Gets the AFC value measured while receiving the packet
 *
 * \retval afcValue Current AFC value in [Hz]
 */
uint32_t SX1272GetPacketAfc( void );

/*!
 * \brief Sets the radio in Rx mode. Waiting for a packet
 */
void SX1272StartRx( void );

/*!
 * \brief Gets a copy of the current received buffer
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
void SX1272GetRxPacket( void *buffer, uint16_t *size );

/*!
 * \brief Sets a copy of the buffer to be transmitted and starts the
 *        transmission
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
void SX1272SetTxPacket( const void *buffer, uint16_t size );

/*!
 * \brief Gets the current RFState
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
uint8_t SX1272GetRFState( void );

/*!
 * \brief Sets the new state of the RF state machine
 *
 * \param [IN]: state New RF state machine state
 */
void SX1272SetRFState( uint8_t state );

/*!
 * \brief Process the Rx and Tx state machines depending on the
 *       SX1272 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
uint32_t SX1272Process( void );

#endif //__SX1272_H__
