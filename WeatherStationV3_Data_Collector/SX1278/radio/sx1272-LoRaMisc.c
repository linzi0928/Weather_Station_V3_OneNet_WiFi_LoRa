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
 * \file       sx1272-LoRaMisc.c
 * \brief      SX1272 RF chip high level functions driver
 *
 * \remark     Optional support functions.
 *             These functions are defined only to easy the change of the
 *             parameters.
 *             For a final firmware the radio parameters will be known so
 *             there is no need to support all possible parameters.
 *             Removing these functions will greatly reduce the final firmware
 *             size.
 *
 * \version    2.0.0 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include "platform.h"

#if defined( USE_SX1272_RADIO )

#include "sx1272-Hal.h"
#include "sx1272.h"

#include "sx1272-LoRa.h"
#include "sx1272-LoRaMisc.h"

/*!
 * SX1272 definitions
 */
#define XTAL_FREQ                                   32000000
#define FREQ_STEP                                   61.03515625

extern tLoRaSettings LoRaSettings;

void SX1272LoRaSetRFFrequency( uint32_t freq )
{
    LoRaSettings.RFFrequency = freq;

    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    SX1272LR->RegFrfMsb = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    SX1272LR->RegFrfMid = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    SX1272LR->RegFrfLsb = ( uint8_t )( freq & 0xFF );
    SX1272WriteBuffer( REG_LR_FRFMSB, &SX1272LR->RegFrfMsb, 3 );
}

uint32_t SX1272LoRaGetRFFrequency( void )
{
    SX1272ReadBuffer( REG_LR_FRFMSB, &SX1272LR->RegFrfMsb, 3 );
    LoRaSettings.RFFrequency = ( ( uint32_t )SX1272LR->RegFrfMsb << 16 ) | ( ( uint32_t )SX1272LR->RegFrfMid << 8 ) | ( ( uint32_t )SX1272LR->RegFrfLsb );
    LoRaSettings.RFFrequency = ( uint32_t )( ( double )LoRaSettings.RFFrequency * ( double )FREQ_STEP );

    return LoRaSettings.RFFrequency;
}

void SX1272LoRaSetRFPower( int8_t power )
{
    SX1272Read( REG_LR_PACONFIG, &SX1272LR->RegPaConfig );
    SX1272Read( REG_LR_PADAC, &SX1272LR->RegPaDac );
    
    if( ( SX1272LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {
        if( ( SX1272LR->RegPaDac & 0x07 ) == 0x07 )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            SX1272LR->RegPaConfig = ( SX1272LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            SX1272LR->RegPaConfig = ( SX1272LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        SX1272LR->RegPaConfig = ( SX1272LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1272Write( REG_LR_PACONFIG, SX1272LR->RegPaConfig );
    LoRaSettings.Power = power;
}

int8_t SX1272LoRaGetRFPower( void )
{
    SX1272Read( REG_LR_PACONFIG, &SX1272LR->RegPaConfig );
    SX1272Read( REG_LR_PADAC, &SX1272LR->RegPaDac );

    if( ( SX1272LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {
        if( ( SX1272LR->RegPaDac & 0x07 ) == 0x07 )
        {
            LoRaSettings.Power = 5 + ( SX1272LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
        }
        else
        {
            LoRaSettings.Power = 2 + ( SX1272LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
        }
    }
    else
    {
        LoRaSettings.Power = -1 + ( SX1272LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
    }
    return LoRaSettings.Power;
}

void SX1272LoRaSetSignalBandwidth( uint8_t bw )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    SX1272LR->RegModemConfig1 = ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 6 );
    SX1272Write( REG_LR_MODEMCONFIG1, SX1272LR->RegModemConfig1 );
    LoRaSettings.SignalBw = bw;
}

uint8_t SX1272LoRaGetSignalBandwidth( void )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    LoRaSettings.SignalBw = ( SX1272LR->RegModemConfig1 & ~RFLR_MODEMCONFIG1_BW_MASK ) >> 6;
    return LoRaSettings.SignalBw;
}

void SX1272LoRaSetSpreadingFactor( uint8_t factor )
{

    if( factor > 12 )
    {
        factor = 12;
    }
    else if( factor < 6 )
    {
        factor = 6;
    }
    
    if( factor == 6 )
    {
        SX1272LoRaSetNbTrigPeaks( 5 );
    }
    else
    {
        SX1272LoRaSetNbTrigPeaks( 3 );
    }

    SX1272Read( REG_LR_MODEMCONFIG2, &SX1272LR->RegModemConfig2 );    
    SX1272LR->RegModemConfig2 = ( SX1272LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );
    SX1272Write( REG_LR_MODEMCONFIG2, SX1272LR->RegModemConfig2 );    
    LoRaSettings.SpreadingFactor = factor;
}

uint8_t SX1272LoRaGetSpreadingFactor( void )
{
    SX1272Read( REG_LR_MODEMCONFIG2, &SX1272LR->RegModemConfig2 );   
    LoRaSettings.SpreadingFactor = ( SX1272LR->RegModemConfig2 & ~RFLR_MODEMCONFIG2_SF_MASK ) >> 4;
    return LoRaSettings.SpreadingFactor;
}

void SX1272LoRaSetErrorCoding( uint8_t value )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    SX1272LR->RegModemConfig1 = ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 3 );
    SX1272Write( REG_LR_MODEMCONFIG1, SX1272LR->RegModemConfig1 );
    LoRaSettings.ErrorCoding = value;
}

uint8_t SX1272LoRaGetErrorCoding( void )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    LoRaSettings.ErrorCoding = ( SX1272LR->RegModemConfig1 & ~RFLR_MODEMCONFIG1_CODINGRATE_MASK ) >> 3;
    return LoRaSettings.ErrorCoding;
}

void SX1272LoRaSetPacketCrcOn( bool enable )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    SX1272LR->RegModemConfig1 = ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_RXPAYLOADCRC_MASK ) | ( enable << 1 );
    SX1272Write( REG_LR_MODEMCONFIG1, SX1272LR->RegModemConfig1 );
    LoRaSettings.CrcOn = enable;
}

void SX1272LoRaSetPreambleLength( uint16_t value )
{
    SX1272ReadBuffer( REG_LR_PREAMBLEMSB, &SX1272LR->RegPreambleMsb, 2 );

    SX1272LR->RegPreambleMsb = ( value >> 8 ) & 0x00FF;
    SX1272LR->RegPreambleLsb = value & 0xFF;
    SX1272WriteBuffer( REG_LR_PREAMBLEMSB, &SX1272LR->RegPreambleMsb, 2 );
}

uint16_t SX1272LoRaGetPreambleLength( void )
{
    SX1272ReadBuffer( REG_LR_PREAMBLEMSB, &SX1272LR->RegPreambleMsb, 2 );
    return ( ( SX1272LR->RegPreambleMsb & 0x00FF ) << 8 ) | SX1272LR->RegPreambleLsb;
}

bool SX1272LoRaGetPacketCrcOn( void )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    LoRaSettings.CrcOn = ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_RXPAYLOADCRC_ON ) >> 1;
    return LoRaSettings.CrcOn;
}

void SX1272LoRaSetImplicitHeaderOn( bool enable )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    SX1272LR->RegModemConfig1 = ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable << 2 );
    SX1272Write( REG_LR_MODEMCONFIG1, SX1272LR->RegModemConfig1 );
    LoRaSettings.ImplicitHeaderOn = enable;
}

bool SX1272LoRaGetImplicitHeaderOn( void )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    LoRaSettings.ImplicitHeaderOn = ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_ON ) >> 2;
    return LoRaSettings.ImplicitHeaderOn;
}

void SX1272LoRaSetRxSingleOn( bool enable )
{
    LoRaSettings.RxSingleOn = enable;
}

bool SX1272LoRaGetRxSingleOn( void )
{
    return LoRaSettings.RxSingleOn;
}

void SX1272LoRaSetFreqHopOn( bool enable )
{
    LoRaSettings.FreqHopOn = enable;
}

bool SX1272LoRaGetFreqHopOn( void )
{
    return LoRaSettings.FreqHopOn;
}

void SX1272LoRaSetHopPeriod( uint8_t value )
{
    SX1272LR->RegHopPeriod = value;
    SX1272Write( REG_LR_HOPPERIOD, SX1272LR->RegHopPeriod );
    LoRaSettings.HopPeriod = value;
}

uint8_t SX1272LoRaGetHopPeriod( void )
{
    SX1272Read( REG_LR_HOPPERIOD, &SX1272LR->RegHopPeriod );
    LoRaSettings.HopPeriod = SX1272LR->RegHopPeriod;
    return LoRaSettings.HopPeriod;
}

void SX1272LoRaSetTxPacketTimeout( uint32_t value )
{
    LoRaSettings.TxPacketTimeout = value;
}

uint32_t SX1272LoRaGetTxPacketTimeout( void )
{
    return LoRaSettings.TxPacketTimeout;
}

void SX1272LoRaSetRxPacketTimeout( uint32_t value )
{
    LoRaSettings.RxPacketTimeout = value;
}

uint32_t SX1272LoRaGetRxPacketTimeout( void )
{
    return LoRaSettings.RxPacketTimeout;
}

void SX1272LoRaSetPayloadLength( uint8_t value )
{
    SX1272LR->RegPayloadLength = value;
    SX1272Write( REG_LR_PAYLOADLENGTH, SX1272LR->RegPayloadLength );
    LoRaSettings.PayloadLength = value;
}

uint8_t SX1272LoRaGetPayloadLength( void )
{
    SX1272Read( REG_LR_PAYLOADLENGTH, &SX1272LR->RegPayloadLength );
    LoRaSettings.PayloadLength = SX1272LR->RegPayloadLength;
    return LoRaSettings.PayloadLength;
}

void SX1272LoRaSetPa20dBm( bool enale )
{
    SX1272Read( REG_LR_PADAC, &SX1272LR->RegPaDac );
    
    if( enale == true )
    {
        SX1272LR->RegPaDac = 0x87;
    }
    else
    {
        SX1272LR->RegPaDac = 0x84;
    }
    SX1272Write( REG_LR_PADAC, SX1272LR->RegPaDac );
}

bool SX1272LoRaGetPa20dBm( void )
{
    SX1272Read( REG_LR_PADAC, &SX1272LR->RegPaDac );
    
    return ( ( SX1272LR->RegPaDac & 0x07 ) == 0x07 ) ? true : false;
}

void SX1272LoRaSetPaRamp( uint8_t value )
{
    SX1272Read( REG_LR_PARAMP, &SX1272LR->RegPaRamp );
    SX1272LR->RegPaRamp = ( SX1272LR->RegPaRamp & RFLR_PARAMP_MASK ) | ( value & ~RFLR_PARAMP_MASK );
    SX1272Write( REG_LR_PARAMP, SX1272LR->RegPaRamp );
}

uint8_t SX1272LoRaGetPaRamp( void )
{
    SX1272Read( REG_LR_PARAMP, &SX1272LR->RegPaRamp );
    return SX1272LR->RegPaRamp & ~RFLR_PARAMP_MASK;
}

void SX1272LoRaSetSymbTimeout( uint16_t value )
{
    SX1272ReadBuffer( REG_LR_MODEMCONFIG2, &SX1272LR->RegModemConfig2, 2 );

    SX1272LR->RegModemConfig2 = ( SX1272LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
    SX1272LR->RegSymbTimeoutLsb = value & 0xFF;
    SX1272WriteBuffer( REG_LR_MODEMCONFIG2, &SX1272LR->RegModemConfig2, 2 );
}

uint16_t SX1272LoRaGetSymbTimeout( void )
{
    SX1272ReadBuffer( REG_LR_MODEMCONFIG2, &SX1272LR->RegModemConfig2, 2 );
    return ( ( SX1272LR->RegModemConfig2 & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) << 8 ) | SX1272LR->RegSymbTimeoutLsb;
}

void SX1272LoRaSetLowDatarateOptimize( bool enable )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    SX1272LR->RegModemConfig1 = ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_LOWDATARATEOPTIMIZE_MASK ) | enable;
    SX1272Write( REG_LR_MODEMCONFIG1, SX1272LR->RegModemConfig1 );
}

bool SX1272LoRaGetLowDatarateOptimize( void )
{
    SX1272Read( REG_LR_MODEMCONFIG1, &SX1272LR->RegModemConfig1 );
    return ( SX1272LR->RegModemConfig1 & RFLR_MODEMCONFIG1_LOWDATARATEOPTIMIZE_ON );
}

void SX1272LoRaSetNbTrigPeaks( uint8_t value )
{
    SX1272Read( 0x31, &SX1272LR->RegTestReserved31 );
    SX1272LR->RegTestReserved31 = ( SX1272LR->RegTestReserved31 & 0xF8 ) | value;
    SX1272Write( 0x31, SX1272LR->RegTestReserved31 );
}

uint8_t SX1272LoRaGetNbTrigPeaks( void )
{
    SX1272Read( 0x31, &SX1272LR->RegTestReserved31 );
    return ( SX1272LR->RegTestReserved31 & 0x07 );
}

#endif // USE_SX1272_RADIO
