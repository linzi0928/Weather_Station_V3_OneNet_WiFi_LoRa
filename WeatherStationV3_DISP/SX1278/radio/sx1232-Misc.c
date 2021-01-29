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
 * \file       sx1232-Misc.c
 * \brief      SX1232 RF chip high level functions driver
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
#include <math.h>

#include "platform.h"

#if defined( USE_SX1232_RADIO )

#include "sx1232-Hal.h"
#include "sx1232.h"

#include "sx1232-Misc.h"

extern tFskSettings FskSettings;

void SX1232SetRFFrequency( uint32_t freq )
{
    FskSettings.RFFrequency = freq;

    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    SX1232->RegFrfMsb = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    SX1232->RegFrfMid = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    SX1232->RegFrfLsb = ( uint8_t )( freq & 0xFF );
    SX1232WriteBuffer( REG_FRFMSB, &SX1232->RegFrfMsb, 3 );
}

uint32_t SX1232GetRFFrequency( void )
{
    SX1232ReadBuffer( REG_FRFMSB, &SX1232->RegFrfMsb, 3 );
    FskSettings.RFFrequency = ( ( uint32_t )SX1232->RegFrfMsb << 16 ) | ( ( uint32_t )SX1232->RegFrfMid << 8 ) | ( ( uint32_t )SX1232->RegFrfLsb );
    FskSettings.RFFrequency = ( uint32_t )( ( double )FskSettings.RFFrequency * ( double )FREQ_STEP );

    return FskSettings.RFFrequency;
}

void SX1232RxCalibrate( void )
{
    // Rx chain re-calibration workaround
    SX1232Write( REG_RSSICONFIG, RF_RSSICONFIG_OFFSET_P_00_DB |
                                 RF_RSSICONFIG_SMOOTHING_256 );
    
    SX1232Write( REG_IMAGECAL, RF_IMAGECAL_AUTOIMAGECAL_OFF |
                               RF_IMAGECAL_IMAGECAL_START |
                               RF_IMAGECAL_TEMPTHRESHOLD_10 |
                               RF_IMAGECAL_TEMPMONITOR_OFF );

    // Wait 8ms
    uint32_t startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 8 ) );    

    SX1232Write( REG_RSSICONFIG, RF_RSSICONFIG_OFFSET_P_00_DB |
                                 RF_RSSICONFIG_SMOOTHING_16 );
}

void SX1232SetBitrate( uint32_t bitrate )
{
    FskSettings.Bitrate = bitrate;
    
    bitrate = ( uint16_t )( ( double )XTAL_FREQ / ( double )bitrate );
    SX1232->RegBitrateMsb    = ( uint8_t )( bitrate >> 8 );
    SX1232->RegBitrateLsb    = ( uint8_t )( bitrate & 0xFF );
    SX1232WriteBuffer( REG_BITRATEMSB, &SX1232->RegBitrateMsb, 2 );    
}

uint32_t SX1232GetBitrate( void )
{
    SX1232ReadBuffer( REG_BITRATEMSB, &SX1232->RegBitrateMsb, 2 );
    FskSettings.Bitrate = ( ( ( uint32_t )SX1232->RegBitrateMsb << 8 ) | ( ( uint32_t )SX1232->RegBitrateLsb ) );
    FskSettings.Bitrate = ( uint16_t )( ( double )XTAL_FREQ / ( double )FskSettings.Bitrate );

    return FskSettings.Bitrate;
}

void SX1232SetFdev( uint32_t fdev )
{
    FskSettings.Fdev = fdev;

    fdev = ( uint16_t )( ( double )fdev / ( double )FREQ_STEP );
    SX1232->RegFdevMsb    = ( uint8_t )( fdev >> 8 );
    SX1232->RegFdevLsb    = ( uint8_t )( fdev & 0xFF );
    SX1232WriteBuffer( REG_FDEVMSB, &SX1232->RegFdevMsb, 2 );    
}

uint32_t SX1232GetFdev( void )
{
    SX1232ReadBuffer( REG_FDEVMSB, &SX1232->RegFdevMsb, 2 );
    FskSettings.Fdev = ( ( ( uint32_t )SX1232->RegFdevMsb << 8 ) | ( ( uint32_t )SX1232->RegFdevLsb ) );
    FskSettings.Fdev = ( uint16_t )( ( double )FskSettings.Fdev * ( double )FREQ_STEP );

    return FskSettings.Fdev;
}

void SX1232SetRFPower( int8_t power )
{
    SX1232Read( REG_PACONFIG, &SX1232->RegPaConfig );
    SX1232Read( REG_PADAC, &SX1232->RegPaDac );
    
    if( ( SX1232->RegPaConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( ( SX1232->RegPaDac & 0x07 ) == 0x07 )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            SX1232->RegPaConfig = ( SX1232->RegPaConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
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
            SX1232->RegPaConfig = ( SX1232->RegPaConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
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
        SX1232->RegPaConfig = ( SX1232->RegPaConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1232Write( REG_PACONFIG, SX1232->RegPaConfig );
    FskSettings.Power = power;
}

int8_t SX1232GetRFPower( void )
{
    SX1232Read( REG_PACONFIG, &SX1232->RegPaConfig );
    SX1232Read( REG_PADAC, &SX1232->RegPaDac );

    if( ( SX1232->RegPaConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( ( SX1232->RegPaDac & 0x07 ) == 0x07 )
        {
            FskSettings.Power = 5 + ( SX1232->RegPaConfig & ~RF_PACONFIG_OUTPUTPOWER_MASK );
        }
        else
        {
            FskSettings.Power = 2 + ( SX1232->RegPaConfig & ~RF_PACONFIG_OUTPUTPOWER_MASK );
        }
    }
    else
    {
        FskSettings.Power = -1 + ( SX1232->RegPaConfig & ~RF_PACONFIG_OUTPUTPOWER_MASK );
    }
    return FskSettings.Power;
}

/*!
 * \brief Computes the Rx bandwidth with the mantisse and exponent
 *
 * \param [IN] mantisse Mantisse of the bandwidth value
 * \param [IN] exponent Exponent of the bandwidth value
 * \retval bandwidth Computed bandwidth
 */
static uint32_t SX1232ComputeRxBw( uint8_t mantisse, uint8_t exponent )
{
    // rxBw
    if( ( SX1232->RegOpMode & RF_OPMODE_MODULATIONTYPE_FSK ) == RF_OPMODE_MODULATIONTYPE_FSK )
    {
        return ( uint32_t )( ( double )XTAL_FREQ / ( mantisse * ( double )pow( 2, exponent + 2 ) ) );
    }
    else
    {
        return ( uint32_t )( ( double )XTAL_FREQ / ( mantisse * ( double )pow( 2, exponent + 3 ) ) );
    }
}

/*!
 * \brief Computes the mantisse and exponent from the bandwitdh value
 *
 * \param [IN] rxBwValue Bandwidth value
 * \param [OUT] mantisse Mantisse of the bandwidth value
 * \param [OUT] exponent Exponent of the bandwidth value
 */
static void SX1232ComputeRxBwMantExp( uint32_t rxBwValue, uint8_t* mantisse, uint8_t* exponent )
{
    uint8_t tmpExp = 0;
    uint8_t tmpMant = 0;

    double tmpRxBw = 0;
    double rxBwMin = 10e6;

    for( tmpExp = 0; tmpExp < 8; tmpExp++ )
    {
        for( tmpMant = 16; tmpMant <= 24; tmpMant += 4 )
        {
            if( ( SX1232->RegOpMode & RF_OPMODE_MODULATIONTYPE_FSK ) == RF_OPMODE_MODULATIONTYPE_FSK )
            {
                tmpRxBw = ( double )XTAL_FREQ / ( tmpMant * ( double )pow( 2, tmpExp + 2 ) );
            }
            else
            {
                tmpRxBw = ( double )XTAL_FREQ / ( tmpMant * ( double )pow( 2, tmpExp + 3 ) );
            }
            if( fabs( tmpRxBw - rxBwValue ) < rxBwMin )
            {
                rxBwMin = fabs( tmpRxBw - rxBwValue );
                *mantisse = tmpMant;
                *exponent = tmpExp;
            }
        }
    }
}

void SX1232SetDccBw( uint8_t* reg, uint32_t dccValue, uint32_t rxBwValue )
{
    uint8_t mantisse = 0;
    uint8_t exponent = 0;
    
    if( reg == &SX1232->RegRxBw )
    {
        *reg = ( uint8_t )dccValue & 0x60;
    }
    else
    {
        *reg = 0;
    }

    SX1232ComputeRxBwMantExp( rxBwValue, &mantisse, &exponent );

    switch( mantisse )
    {
        case 16:
            *reg |= ( uint8_t )( 0x00 | ( exponent & 0x07 ) );
            break;
        case 20:
            *reg |= ( uint8_t )( 0x08 | ( exponent & 0x07 ) );
            break;
        case 24:
            *reg |= ( uint8_t )( 0x10 | ( exponent & 0x07 ) );
            break;
        default:
            // Something went terribely wrong
            break;
    }

    if( reg == &SX1232->RegRxBw )
    {
        SX1232Write( REG_RXBW, *reg );
        FskSettings.RxBw = rxBwValue;
    }
    else
    {
        SX1232Write( REG_AFCBW, *reg );
        FskSettings.RxBwAfc = rxBwValue;
    }
}

uint32_t SX1232GetBw( uint8_t* reg )
{
    uint32_t rxBwValue = 0;
    uint8_t mantisse = 0;
    switch( ( *reg & 0x18 ) >> 3 )
    {
        case 0:
            mantisse = 16;
            break;
        case 1:
            mantisse = 20;
            break;
        case 2:
            mantisse = 24;
            break;
        default:
            break;
    }
    rxBwValue = SX1232ComputeRxBw( mantisse, ( uint8_t )*reg & 0x07 );
    if( reg == &SX1232->RegRxBw )
    {
        return FskSettings.RxBw = rxBwValue;
    }
    else
    {
        return FskSettings.RxBwAfc = rxBwValue;
    }
}

void SX1232SetPacketCrcOn( bool enable )
{
    SX1232Read( REG_PACKETCONFIG1, &SX1232->RegPacketConfig1 );
    SX1232->RegPacketConfig1 = ( SX1232->RegPacketConfig1 & RF_PACKETCONFIG1_CRC_MASK ) | ( enable << 4 );
    SX1232Write( REG_PACKETCONFIG1, SX1232->RegPacketConfig1 );
    FskSettings.CrcOn = enable;
}

bool SX1232GetPacketCrcOn( void )
{
    SX1232Read( REG_PACKETCONFIG1, &SX1232->RegPacketConfig1 );
    FskSettings.CrcOn = ( SX1232->RegPacketConfig1 & RF_PACKETCONFIG1_CRC_ON ) >> 4;
    return FskSettings.CrcOn;
}

void SX1232SetAfcOn( bool enable )
{
    SX1232Read( REG_RXCONFIG, &SX1232->RegRxConfig );
    SX1232->RegRxConfig = ( SX1232->RegRxConfig & RF_RXCONFIG_AFCAUTO_MASK ) | ( enable << 4 );
    SX1232Write( REG_RXCONFIG, SX1232->RegRxConfig );
    FskSettings.AfcOn = enable;
}

bool SX1232GetAfcOn( void )
{
    SX1232Read( REG_RXCONFIG, &SX1232->RegRxConfig );
    FskSettings.AfcOn = ( SX1232->RegRxConfig & RF_RXCONFIG_AFCAUTO_ON ) >> 4;
    return FskSettings.AfcOn;
}

void SX1232SetPayloadLength( uint8_t value )
{
    SX1232->RegPayloadLength = value;
    SX1232Write( REG_PAYLOADLENGTH, SX1232->RegPayloadLength );
    FskSettings.PayloadLength = value;
}

uint8_t SX1232GetPayloadLength( void )
{
    SX122Read( REG_PAYLOADLENGTH, &SX1232->RegPayloadLength );
    FskSettings.PayloadLength = SX1232->RegPayloadLength;
    return FskSettings.PayloadLength;
}

void SX1232SetPa20dBm( bool enale )
{
    SX1232Read( REG_PADAC, &SX1232->RegPaDac );
    
    if( enale == true )
    {
        SX1232->RegPaDac = 0x87;
    }
    else
    {
        SX1232->RegPaDac = 0x84;
    }
    SX1232Write( REG_PADAC, SX1232->RegPaDac );
}

bool SX1232GetPa20dBm( void )
{
    SX1232Read( REG_PADAC, &SX1232->RegPaDac );
    
    return ( ( SX1232->RegPaDac & 0x07 ) == 0x07 ) ? true : false;
}

void SX1232SetPaRamp( uint8_t value )
{
    SX1232Read( REG_PARAMP, &SX1232->RegPaRamp );
    SX1232->RegPaRamp = ( SX1232->RegPaRamp & RF_PARAMP_MASK ) | ( value & ~RF_PARAMP_MASK );
    SX1232Write( REG_PARAMP, SX1232->RegPaRamp );
}

uint8_t SX1232GetPaRamp( void )
{
    SX1232Read( REG_PARAMP, &SX1232->RegPaRamp );
    return SX1232->RegPaRamp & ~RF_PARAMP_MASK;
}

void SX1232SetRssiOffset( int8_t offset )
{
    SX1232Read( REG_RSSICONFIG, &SX1232->RegRssiConfig );
    if( offset < 0 )
    {
        offset = ( ~offset & 0x1F );
        offset += 1;
        offset = -offset;
    }
    SX1232->RegRssiConfig |= ( uint8_t )( ( offset & 0x1F ) << 3 );
    SX1232Write( REG_RSSICONFIG, SX1232->RegRssiConfig );
}

int8_t SX1232GetRssiOffset( void )
{
    SX1232Read( REG_RSSICONFIG, &SX1232->RegRssiConfig );
    int8_t offset = SX1232->RegRssiConfig >> 3;
    if( ( offset & 0x10 ) == 0x10 )
    {
        offset = ( ~offset & 0x1F );
        offset += 1;
        offset = -offset;
    }
    return offset;
}

int8_t SX1232GetRawTemp( void )
{
    int8_t temp = 0;
    uint8_t regValue = 0;
    
    SX1232Read( REG_TEMP, &SX1232->RegTemp );
    
    temp = regValue & 0x7F;
    
    if( ( regValue & 0x80 ) == 0x80 )
    {
        temp *= -1;
    }
    return temp;
}

int8_t SX1232CalibreateTemp( int8_t actualTemp )
{
    return actualTemp - SX1232GetRawTemp( );
}

int8_t SX1232GetTemp( int8_t compensationFactor )
{
    return SX1232GetRawTemp( ) + compensationFactor;
}

#endif // USE_SX1232_RADIO
