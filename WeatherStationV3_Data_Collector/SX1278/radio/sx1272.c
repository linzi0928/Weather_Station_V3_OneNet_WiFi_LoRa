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
 * \file       sx1272.c
 * \brief      SX1272 RF chip driver
 *
 * \version    2.0.0 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include "platform.h"
#include "radio.h"

#if defined( USE_SX1272_RADIO )

#include "sx1272.h"

#include "sx1272-Hal.h"
#include "sx1272-Fsk.h"
#include "sx1272-LoRa.h"

/*!
 * SX1272 registers variable
 */
uint8_t SX1272Regs[0x70];

static bool LoRaOn = false;
static bool LoRaOnState = false;

void SX1272Init( void )
{
    // Initialize FSK and LoRa registers structure
    SX1272 = ( tSX1272* )SX1272Regs;
    SX1272LR = ( tSX1272LR* )SX1272Regs;

    SX1272InitIo( );

    SX1272Reset( );

    // REMARK: After radio reset the default modem is FSK

#if ( LORA == 0 ) 

    LoRaOn = false;
    SX1272SetLoRaOn( LoRaOn );
    // Initialize FSK modem
    SX1272FskInit( );

#else

    LoRaOn = true;
    SX1272SetLoRaOn( LoRaOn );
    // Initialize LoRa modem
    SX1272LoRaInit( );
    
#endif

}

void SX1272Reset( void )
{
    uint32_t startTick;

    SX1272SetReset( RADIO_RESET_ON );
    
    // Wait 1ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 1 ) );    

    SX1272SetReset( RADIO_RESET_OFF );
    
    // Wait 6ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 6 ) );    
}

void SX1272SetLoRaOn( bool enable )
{
    if( LoRaOnState == enable )
    {
        return;
    }
    LoRaOnState = enable;
    LoRaOn = enable;

    if( LoRaOn == true )
    {
        SX1272LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        SX1272LR->RegOpMode = ( SX1272LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
        SX1272Write( REG_LR_OPMODE, SX1272LR->RegOpMode );
        
        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );
                                        // RxDone               RxTimeout                   FhssChangeChannel           CadDone
        SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                        // CadDetected          ModeReady
        SX1272LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );
        
        SX1272ReadBuffer( REG_LR_OPMODE, SX1272Regs + 1, 0x70 - 1 );
    }
    else
    {
        SX1272LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        SX1272LR->RegOpMode = ( SX1272LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_OFF;
        SX1272Write( REG_LR_OPMODE, SX1272LR->RegOpMode );
        
        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );
        
        SX1272ReadBuffer( REG_OPMODE, SX1272Regs + 1, 0x70 - 1 );
    }
}

bool SX1272GetLoRaOn( void )
{
    return LoRaOn;
}

void SX1272SetOpMode( uint8_t opMode )
{
    if( LoRaOn == false )
    {
        SX1272FskSetOpMode( opMode );
    }
    else
    {
        SX1272LoRaSetOpMode( opMode );
    }
}

uint8_t SX1272GetOpMode( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskGetOpMode( );
    }
    else
    {
        return SX1272LoRaGetOpMode( );
    }
}

double SX1272ReadRssi( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskReadRssi( );
    }
    else
    {
        return SX1272LoRaReadRssi( );
    }
}

uint8_t SX1272ReadRxGain( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskReadRxGain( );
    }
    else
    {
        return SX1272LoRaReadRxGain( );
    }
}

uint8_t SX1272GetPacketRxGain( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskGetPacketRxGain(  );
    }
    else
    {
        return SX1272LoRaGetPacketRxGain(  );
    }
}

int8_t SX1272GetPacketSnr( void )
{
    if( LoRaOn == false )
    {
         while( 1 )
         {
             // Useless in FSK mode
             // Block program here
         }
    }
    else
    {
        return SX1272LoRaGetPacketSnr(  );
    }
}

double SX1272GetPacketRssi( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskGetPacketRssi(  );
    }
    else
    {
        return SX1272LoRaGetPacketRssi( );
    }
}

uint32_t SX1272GetPacketAfc( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskGetPacketAfc(  );
    }
    else
    {
         while( 1 )
         {
             // Useless in LoRa mode
             // Block program here
         }
    }
}

void SX1272StartRx( void )
{
    if( LoRaOn == false )
    {
        SX1272FskSetRFState( RF_STATE_RX_INIT );
    }
    else
    {
        SX1272LoRaSetRFState( RFLR_STATE_RX_INIT );
    }
}

void SX1272GetRxPacket( void *buffer, uint16_t *size )
{
    if( LoRaOn == false )
    {
        SX1272FskGetRxPacket( buffer, size );
    }
    else
    {
        SX1272LoRaGetRxPacket( buffer, size );
    }
}

void SX1272SetTxPacket( const void *buffer, uint16_t size )
{
    if( LoRaOn == false )
    {
        SX1272FskSetTxPacket( buffer, size );
    }
    else
    {
        SX1272LoRaSetTxPacket( buffer, size );
    }
}

uint8_t SX1272GetRFState( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskGetRFState( );
    }
    else
    {
        return SX1272LoRaGetRFState( );
    }
}

void SX1272SetRFState( uint8_t state )
{
    if( LoRaOn == false )
    {
        SX1272FskSetRFState( state );
    }
    else
    {
        SX1272LoRaSetRFState( state );
    }
}

uint32_t SX1272Process( void )
{
    if( LoRaOn == false )
    {
        return SX1272FskProcess( );
    }
    else
    {
        return SX1272LoRaProcess( );
    }
}

#endif // USE_SX1272_RADIO
