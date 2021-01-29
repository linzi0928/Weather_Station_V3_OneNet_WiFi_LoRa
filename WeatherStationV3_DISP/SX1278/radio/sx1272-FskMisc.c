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
 * \file       sx1272-FskMisc.c
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
#include <math.h>

#include "platform.h"

#if defined( USE_SX1272_RADIO )

#include "sx1272-Hal.h"
#include "sx1272.h"

#include "sx1272-Fsk.h"
#include "sx1272-FskMisc.h"

extern tFskSettings FskSettings;

void SX1272FskSetRFFrequency( uint32_t freq )
{
    FskSettings.RFFrequency = freq;

    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    SX1272->RegFrfMsb = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    SX1272->RegFrfMid = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    SX1272->RegFrfLsb = ( uint8_t )( freq & 0xFF );
    SX1272WriteBuffer( REG_FRFMSB, &SX1272->RegFrfMsb, 3 );
}

uint32_t SX1272FskGetRFFrequency( void )
{
    SX1272ReadBuffer( REG_FRFMSB, &SX1272->RegFrfMsb, 3 );
    FskSettings.RFFrequency = ( ( uint32_t )SX1272->RegFrfMsb << 16 ) | ( ( uint32_t )SX1272->RegFrfMid << 8 ) | ( ( uint32_t )SX1272->RegFrfLsb );
    FskSettings.RFFrequency = ( uint32_t )( ( double )FskSettings.RFFrequency * ( double )FREQ_STEP );

    return FskSettings.RFFrequency;
}

void SX1272FskRxCalibrate( void )
{
    uint32_t startTick;
	
    SX1272Write( REG_IMAGECAL, RF_IMAGECAL_AUTOIMAGECAL_OFF |
                               RF_IMAGECAL_IMAGECAL_START |
                               RF_IMAGECAL_TEMPTHRESHOLD_10 |
                               RF_IMAGECAL_TEMPMONITOR_OFF );

    // Wait 8ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 8 ) );    
}

void SX1272FskSetBitrate( uint32_t bitrate )
{
    FskSettings.Bitrate = bitrate;
    
    bitrate = ( uint16_t )( ( double )XTAL_FREQ / ( double )bitrate );
    SX1272->RegBitrateMsb    = ( uint8_t )( bitrate >> 8 );
    SX1272->RegBitrateLsb    = ( uint8_t )( bitrate & 0xFF );
    SX1272WriteBuffer( REG_BITRATEMSB, &SX1272->RegBitrateMsb, 2 );    
}

uint32_t SX1272FskGetBitrate( void )
{
    SX1272ReadBuffer( REG_BITRATEMSB, &SX1272->RegBitrateMsb, 2 );
    FskSettings.Bitrate = ( ( ( uint32_t )SX1272->RegBitrateMsb << 8 ) | ( ( uint32_t )SX1272->RegBitrateLsb ) );
    FskSettings.Bitrate = ( uint16_t )( ( double )XTAL_FREQ / ( double )FskSettings.Bitrate );

    return FskSettings.Bitrate;
}

void SX1272FskSetFdev( uint32_t fdev )
{
    FskSettings.Fdev = fdev;

    fdev = ( uint16_t )( ( double )fdev / ( double )FREQ_STEP );
    SX1272->RegFdevMsb    = ( uint8_t )( fdev >> 8 );
    SX1272->RegFdevLsb    = ( uint8_t )( fdev & 0xFF );
    SX1272WriteBuffer( REG_FDEVMSB, &SX1272->RegFdevMsb, 2 );    
}

uint32_t SX1272FskGetFdev( void )
{
    SX1272ReadBuffer( REG_FDEVMSB, &SX1272->RegFdevMsb, 2 );
    FskSettings.Fdev = ( ( ( uint32_t )SX1272->RegFdevMsb << 8 ) | ( ( uint32_t )SX1272->RegFdevLsb ) );
    FskSettings.Fdev = ( uint16_t )( ( double )FskSettings.Fdev * ( double )FREQ_STEP );

    return FskSettings.Fdev;
}

void SX1272FskSetRFPower( int8_t power )
{
    SX1272Read( REG_PACONFIG, &SX1272->RegPaConfig );
    SX1272Read( REG_PADAC, &SX1272->RegPaDac );
    
    if( ( SX1272->RegPaConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( ( SX1272->RegPaDac & 0x07 ) == 0x07 )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            SX1272->RegPaConfig = ( SX1272->RegPaConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
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
            SX1272->RegPaConfig = ( SX1272->RegPaConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
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
        SX1272->RegPaConfig = ( SX1272->RegPaConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1272Write( REG_PACONFIG, SX1272->RegPaConfig );
    FskSettings.Power = power;
}

int8_t SX1272FskGetRFPower( void )
{
    SX1272Read( REG_PACONFIG, &SX1272->RegPaConfig );
    SX1272Read( REG_PADAC, &SX1272->RegPaDac );

    if( ( SX1272->RegPaConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( ( SX1272->RegPaDac & 0x07 ) == 0x07 )
        {
            FskSettings.Power = 5 + ( SX1272->RegPaConfig & ~RF_PACONFIG_OUTPUTPOWER_MASK );
        }
        else
        {
            FskSettings.Power = 2 + ( SX1272->RegPaConfig & ~RF_PACONFIG_OUTPUTPOWER_MASK );
        }
    }
    else
    {
        FskSettings.Power = -1 + ( SX1272->RegPaConfig & ~RF_PACONFIG_OUTPUTPOWER_MASK );
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
static uint32_t SX1272FskComputeRxBw( uint8_t mantisse, uint8_t exponent )
{
    // rxBw
    if( ( SX1272->RegOpMode & RF_OPMODE_MODULATIONTYPE_FSK ) == RF_OPMODE_MODULATIONTYPE_FSK )
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
static void SX1272FskComputeRxBwMantExp( uint32_t rxBwValue, uint8_t* mantisse, uint8_t* exponent )
{
    uint8_t tmpExp = 0;
    uint8_t tmpMant = 0;

    double tmpRxBw = 0;
    double rxBwMin = 10e6;

    for( tmpExp = 0; tmpExp < 8; tmpExp++ )
    {
        for( tmpMant = 16; tmpMant <= 24; tmpMant += 4 )
        {
            if( ( SX1272->RegOpMode & RF_OPMODE_MODULATIONTYPE_FSK ) == RF_OPMODE_MODULATIONTYPE_FSK )
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

void SX1272FskSetDccBw( uint8_t* reg, uint32_t dccValue, uint32_t rxBwValue )
{
    uint8_t mantisse = 0;
    uint8_t exponent = 0;
    
    if( reg == &SX1272->RegRxBw )
    {
        *reg = ( uint8_t )dccValue & 0x60;
    }
    else
    {
        *reg = 0;
    }

    SX1272FskComputeRxBwMantExp( rxBwValue, &mantisse, &exponent );

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

    if( reg == &SX1272->RegRxBw )
    {
        SX1272Write( REG_RXBW, *reg );
        FskSettings.RxBw = rxBwValue;
    }
    else
    {
        SX1272Write( REG_AFCBW, *reg );
        FskSettings.RxBwAfc = rxBwValue;
    }
}

uint32_t SX1272FskGetBw( uint8_t* reg )
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
    rxBwValue = SX1272FskComputeRxBw( mantisse, ( uint8_t )*reg & 0x07 );
    if( reg == &SX1272->RegRxBw )
    {
        return FskSettings.RxBw = rxBwValue;
    }
    else
    {
        return FskSettings.RxBwAfc = rxBwValue;
    }
}

void SX1272FskSetPacketCrcOn( bool enable )
{
    SX1272Read( REG_PACKETCONFIG1, &SX1272->RegPacketConfig1 );
    SX1272->RegPacketConfig1 = ( SX1272->RegPacketConfig1 & RF_PACKETCONFIG1_CRC_MASK ) | ( enable << 4 );
    SX1272Write( REG_PACKETCONFIG1, SX1272->RegPacketConfig1 );
    FskSettings.CrcOn = enable;
}

bool SX1272FskGetPacketCrcOn( void )
{
    SX1272Read( REG_PACKETCONFIG1, &SX1272->RegPacketConfig1 );
    FskSettings.CrcOn = ( SX1272->RegPacketConfig1 & RF_PACKETCONFIG1_CRC_ON ) >> 4;
    return FskSettings.CrcOn;
}

void SX1272FskSetAfcOn( bool enable )
{
    SX1272Read( REG_RXCONFIG, &SX1272->RegRxConfig );
    SX1272->RegRxConfig = ( SX1272->RegRxConfig & RF_RXCONFIG_AFCAUTO_MASK ) | ( enable << 4 );
    SX1272Write( REG_RXCONFIG, SX1272->RegRxConfig );
    FskSettings.AfcOn = enable;
}

bool SX1272FskGetAfcOn( void )
{
    SX1272Read( REG_RXCONFIG, &SX1272->RegRxConfig );
    FskSettings.AfcOn = ( SX1272->RegRxConfig & RF_RXCONFIG_AFCAUTO_ON ) >> 4;
    return FskSettings.AfcOn;
}

void SX1272FskSetPayloadLength( uint8_t value )
{
    SX1272->RegPayloadLength = value;
    SX1272Write( REG_PAYLOADLENGTH, SX1272->RegPayloadLength );
    FskSettings.PayloadLength = value;
}

uint8_t SX1272FskGetPayloadLength( void )
{
    SX1272Read( REG_PAYLOADLENGTH, &SX1272->RegPayloadLength );
    FskSettings.PayloadLength = SX1272->RegPayloadLength;
    return FskSettings.PayloadLength;
}

void SX1272FskSetPa20dBm( bool enale )
{
    SX1272Read( REG_PADAC, &SX1272->RegPaDac );
    
    if( enale == true )
    {
        SX1272->RegPaDac = 0x87;
    }
    else
    {
        SX1272->RegPaDac = 0x84;
    }
    SX1272Write( REG_PADAC, SX1272->RegPaDac );
}

bool SX1272FskGetPa20dBm( void )
{
    SX1272Read( REG_PADAC, &SX1272->RegPaDac );
    
    return ( ( SX1272->RegPaDac & 0x07 ) == 0x07 ) ? true : false;
}

void SX1272FskSetPaRamp( uint8_t value )
{
    SX1272Read( REG_PARAMP, &SX1272->RegPaRamp );
    SX1272->RegPaRamp = ( SX1272->RegPaRamp & RF_PARAMP_MASK ) | ( value & ~RF_PARAMP_MASK );
    SX1272Write( REG_PARAMP, SX1272->RegPaRamp );
}

uint8_t SX1272FskGetPaRamp( void )
{
    SX1272Read( REG_PARAMP, &SX1272->RegPaRamp );
    return SX1272->RegPaRamp & ~RF_PARAMP_MASK;
}

void SX1272FskSetRssiOffset( int8_t offset )
{
    SX1272Read( REG_RSSICONFIG, &SX1272->RegRssiConfig );
    if( offset < 0 )
    {
        offset = ( ~offset & 0x1F );
        offset += 1;
        offset = -offset;
    }
    SX1272->RegRssiConfig |= ( uint8_t )( ( offset & 0x1F ) << 3 );
    SX1272Write( REG_RSSICONFIG, SX1272->RegRssiConfig );
}

int8_t SX1272FskGetRssiOffset( void )
{
    int8_t offset;

    SX1272Read( REG_RSSICONFIG, &SX1272->RegRssiConfig );
    offset = SX1272->RegRssiConfig >> 3;
    if( ( offset & 0x10 ) == 0x10 )
    {
        offset = ( ~offset & 0x1F );
        offset += 1;
        offset = -offset;
    }
    return offset;
}

int8_t SX1272FskGetRawTemp( void )
{
    int8_t temp = 0;
    uint8_t previousOpMode;
    uint32_t startTick;
    
    // Enable Temperature reading
    SX1272Read( REG_IMAGECAL, &SX1272->RegImageCal );
    SX1272->RegImageCal = ( SX1272->RegImageCal & RF_IMAGECAL_TEMPMONITOR_MASK ) | RF_IMAGECAL_TEMPMONITOR_ON;
    SX1272Write( REG_IMAGECAL, SX1272->RegImageCal );

    // save current Op Mode
    SX1272Read( REG_OPMODE, &SX1272->RegOpMode );
    previousOpMode = SX1272->RegOpMode;

    // put device in FSK RxSynth
    SX1272->RegOpMode = RF_OPMODE_SYNTHESIZER_RX;
    SX1272Write( REG_OPMODE, SX1272->RegOpMode );

    // Wait 1ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 2 ) );  

    // Disable Temperature reading
    SX1272Read( REG_IMAGECAL, &SX1272->RegImageCal );
    SX1272->RegImageCal = ( SX1272->RegImageCal & RF_IMAGECAL_TEMPMONITOR_MASK ) | RF_IMAGECAL_TEMPMONITOR_OFF;
    SX1272Write( REG_IMAGECAL, SX1272->RegImageCal );

    // Read temperature
    SX1272Read( REG_TEMP, &SX1272->RegTemp );
    
    temp = SX1272->RegTemp & 0x7F;
    
    if( ( SX1272->RegTemp & 0x80 ) == 0x80 )
    {
        temp *= -1;
    }

    // Reload previous Op Mode
    SX1272Write( REG_OPMODE, previousOpMode );

    return temp;
}

int8_t SX1272FskCalibreateTemp( int8_t actualTemp )
{
    return actualTemp - SX1272FskGetRawTemp( );
}

int8_t SX1272FskGetTemp( int8_t compensationFactor )
{
    return SX1272FskGetRawTemp( ) + compensationFactor;
}

#endif // USE_SX1272_RADIO
