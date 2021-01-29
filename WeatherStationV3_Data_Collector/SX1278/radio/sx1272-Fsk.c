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
#include <string.h>
#include <math.h>

#include "platform.h"

#if defined( USE_SX1272_RADIO )

#include "radio.h"

#include "sx1272-Hal.h"
#include "sx1272.h"

#include "sx1272-FskMisc.h"
#include "sx1272-Fsk.h"

// Default settings
tFskSettings FskSettings = 
{
    870000000,      // RFFrequency
    9600,           // Bitrate
    50000,          // Fdev
    20,             // Power
    100000,         // RxBw
    150000,         // RxBwAfc
    true,           // CrcOn
    true,           // AfcOn    
    255             // PayloadLength (set payload size to the maximum for variable mode, else set the exact payload length)
};

/*!
 * SX1272 FSK registers variable
 */
tSX1272* SX1272;

/*!
 * Local RF buffer for communication support
 */
static uint8_t RFBuffer[RF_BUFFER_SIZE];

/*!
 * Chunk size of data write in buffer 
 */
static uint8_t DataChunkSize = 32;


/*!
 * RF state machine variable
 */
static uint8_t RFState = RF_STATE_IDLE;

/*!
 * Rx management support variables
 */

/*!
 * PacketTimeout holds the RF packet timeout
 * SyncSize = [0..8]
 * VariableSize = [0;1]
 * AddressSize = [0;1]
 * PayloadSize = [0..RF_BUFFER_SIZE]
 * CrcSize = [0;2]
 * PacketTimeout = ( ( 8 * ( VariableSize + AddressSize + PayloadSize + CrcSize ) / BR ) * 1000.0 ) + 1
 * Computed timeout is in miliseconds
 */
static uint32_t PacketTimeout;

/*!
 * Preamble2SyncTimeout
 * Preamble2SyncTimeout = ( ( 8 * ( PremableSize + SyncSize ) / RFBitrate ) * 1000.0 ) + 1
 * Computed timeout is in miliseconds
 */
static uint32_t Preamble2SyncTimeout;

static bool PreambleDetected = false;
static bool SyncWordDetected = false;
static bool PacketDetected = false;
static uint16_t RxPacketSize = 0;
static uint8_t RxBytesRead = 0;
static uint8_t TxBytesSent = 0;
static double RxPacketRssiValue;
static uint32_t RxPacketAfcValue;
static uint8_t RxGain = 1;
static uint32_t RxTimeoutTimer = 0;
static uint32_t Preamble2SyncTimer = 0;

/*!
 * Tx management support variables
 */
static uint16_t TxPacketSize = 0;
static uint32_t TxTimeoutTimer = 0;

void SX1272FskInit( void )
{
    RFState = RF_STATE_IDLE;

    SX1272FskSetDefaults( );

    SX1272ReadBuffer( REG_OPMODE, SX1272Regs + 1, 0x70 - 1 );

    // Set the device in FSK mode and Sleep Mode
    SX1272->RegOpMode = RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_SLEEP;
    SX1272Write( REG_OPMODE, SX1272->RegOpMode );
    
#if ( PLATFORM == SX12xxEiger )
    // Then we initialize the device register structure with the value of our setup
    SX1272->RegPaConfig = ( SX1272->RegPaConfig & RF_PACONFIG_PASELECT_PABOOST ) | RF_PACONFIG_PASELECT_PABOOST;
#else
    // Then we initialize the device register structure with the value of our setup
    SX1272->RegPaConfig = ( SX1272->RegPaConfig & RF_PACONFIG_PASELECT_PABOOST ) | RF_PACONFIG_PASELECT_RFO;
#endif

    SX1272->RegLna = RF_LNA_GAIN_G1 | RF_LNA_BOOST_ON;

    if( FskSettings.AfcOn == true )
    {
        SX1272->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_ON |
                              RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }
    else
    {
        SX1272->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_OFF |
                              RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }

    SX1272->RegPreambleLsb = 8;
    
    SX1272->RegPreambleDetect = RF_PREAMBLEDETECT_DETECTOR_ON | RF_PREAMBLEDETECT_DETECTORSIZE_2 |
                                RF_PREAMBLEDETECT_DETECTORTOL_10;
    
    SX1272->RegRssiThresh = 0xFF;

    SX1272->RegSyncConfig = RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA |
                            RF_SYNCCONFIG_SYNC_ON | RF_SYNCCONFIG_FIFOFILLCONDITION_AUTO |
                            RF_SYNCCONFIG_SYNCSIZE_4;

    SX1272->RegSyncValue1 = 0x69;
    SX1272->RegSyncValue2 = 0x81;
    SX1272->RegSyncValue3 = 0x7E;
    SX1272->RegSyncValue4 = 0x96;

    SX1272->RegPacketConfig1 = RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RF_PACKETCONFIG1_DCFREE_OFF |
                               ( FskSettings.CrcOn << 4 ) | RF_PACKETCONFIG1_CRCAUTOCLEAR_ON |
                               RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT;
    SX1272FskGetPacketCrcOn( ); // Update CrcOn on FskSettings

    SX1272->RegPayloadLength = FskSettings.PayloadLength;

    // we can now update the registers with our configuration
    SX1272WriteBuffer( REG_OPMODE, SX1272Regs + 1, 0x70 - 1 );

    // then we need to set the RF settings 
    SX1272FskSetRFFrequency( FskSettings.RFFrequency );
    SX1272FskSetBitrate( FskSettings.Bitrate );
    SX1272FskSetFdev( FskSettings.Fdev );
#if ( PLATFORM == SX12xxEiger )
    SX1272FskSetPa20dBm( true );
#else
    SX1272FskSetPa20dBm( false );
#endif
    SX1272FskSetRFPower( FskSettings.Power );
    SX1272FskSetDccBw( &SX1272->RegRxBw, 0, FskSettings.RxBw );
    SX1272FskSetDccBw( &SX1272->RegAfcBw, 0, FskSettings.RxBwAfc );
    SX1272FskSetRssiOffset( -6 );

    SX1272FskSetOpMode( RF_OPMODE_STANDBY );
}

void SX1272FskSetDefaults( void )
{
    // REMARK: See SX1272 datasheet for modified default values.

    SX1272Read( REG_VERSION, &SX1272->RegVersion );
}

void SX1272FskSetOpMode( uint8_t opMode )
{
    static uint8_t opModePrev = RF_OPMODE_STANDBY;
    static bool antennaSwitchTxOnPrev = true;
    bool antennaSwitchTxOn = false;

    opModePrev = SX1272->RegOpMode & ~RF_OPMODE_MASK;

    if( opMode != opModePrev )
    {
        if( opMode == RF_OPMODE_TRANSMITTER )
        {
            antennaSwitchTxOn = true;
        }
        else
        {
            antennaSwitchTxOn = false;
        }
        if( antennaSwitchTxOn != antennaSwitchTxOnPrev )
        {
            antennaSwitchTxOnPrev = antennaSwitchTxOn;
            RXTX( antennaSwitchTxOn ); // Antenna switch control
        }
        SX1272->RegOpMode = ( SX1272->RegOpMode & RF_OPMODE_MASK ) | opMode;

        SX1272Write( REG_OPMODE, SX1272->RegOpMode );        
    }
}

uint8_t SX1272FskGetOpMode( void )
{
    SX1272Read( REG_OPMODE, &SX1272->RegOpMode );
    
    return SX1272->RegOpMode & ~RF_OPMODE_MASK;
}

int32_t SX1272FskReadFei( void )
{
    SX1272ReadBuffer( REG_FEIMSB, &SX1272->RegFeiMsb, 2 );                          // Reads the FEI value

    return ( int32_t )( double )( ( ( uint16_t )SX1272->RegFeiMsb << 8 ) | ( uint16_t )SX1272->RegFeiLsb ) * ( double )FREQ_STEP;
}

int32_t SX1272FskReadAfc( void )
{
    SX1272ReadBuffer( REG_AFCMSB, &SX1272->RegAfcMsb, 2 );                            // Reads the AFC value
    return ( int32_t )( double )( ( ( uint16_t )SX1272->RegAfcMsb << 8 ) | ( uint16_t )SX1272->RegAfcLsb ) * ( double )FREQ_STEP;
}

uint8_t SX1272FskReadRxGain( void )
{
    SX1272Read( REG_LNA, &SX1272->RegLna );
    return( SX1272->RegLna >> 5 ) & 0x07;
}

double SX1272FskReadRssi( void )
{
    SX1272Read( REG_RSSIVALUE, &SX1272->RegRssiValue );                               // Reads the RSSI value

    return -( double )( ( double )SX1272->RegRssiValue / 2.0 );
}

uint8_t SX1272FskGetPacketRxGain( void )
{
    return RxGain;
}

double SX1272FskGetPacketRssi( void )
{
    return RxPacketRssiValue;
}

uint32_t SX1272FskGetPacketAfc( void )
{
    return RxPacketAfcValue;
}

void SX1272FskStartRx( void )
{
    SX1272FskSetRFState( RF_STATE_RX_INIT );
}

void SX1272FskGetRxPacket( void *buffer, uint16_t *size )
{
    *size = RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBuffer, ( size_t )*size );
}

void SX1272FskSetTxPacket( const void *buffer, uint16_t size )
{
    TxPacketSize = size;
    memcpy( ( void * )RFBuffer, buffer, ( size_t )TxPacketSize ); 

    RFState = RF_STATE_TX_INIT;
}

// Remark: SX1272 must be fully initialized before calling this function
uint16_t SX1272FskGetPacketPayloadSize( void )
{
    uint16_t syncSize;
    uint16_t variableSize;
    uint16_t addressSize;
    uint16_t payloadSize;
    uint16_t crcSize;

    syncSize = ( SX1272->RegSyncConfig & 0x07 ) + 1;
    variableSize = ( ( SX1272->RegPacketConfig1 & 0x80 ) == 0x80 ) ? 1 : 0;
    addressSize = ( ( SX1272->RegPacketConfig1 & 0x06 ) != 0x00 ) ? 1 : 0;
    payloadSize = SX1272->RegPayloadLength;
    crcSize = ( ( SX1272->RegPacketConfig1 & 0x10 ) == 0x10 ) ? 2 : 0;
    
    return syncSize + variableSize + addressSize + payloadSize + crcSize;
}

// Remark: SX1272 must be fully initialized before calling this function
uint16_t SX1272FskGetPacketHeaderSize( void )
{
    uint16_t preambleSize;
    uint16_t syncSize;

    preambleSize = ( ( uint16_t )SX1272->RegPreambleMsb << 8 ) | ( uint16_t )SX1272->RegPreambleLsb;
    syncSize = ( SX1272->RegSyncConfig & 0x07 ) + 1;
    
    return preambleSize + syncSize;
}

uint8_t SX1272FskGetRFState( void )
{
    return RFState;
}

void SX1272FskSetRFState( uint8_t state )
{
    RFState = state;
}

uint32_t SX1272FskProcess( void )
{
    uint32_t result = RF_BUSY;

    switch( RFState )
    {
    case RF_STATE_IDLE:
        break;
    // Rx management
    case RF_STATE_RX_INIT:
        // DIO mapping setup
        if( ( SX1272->RegPacketConfig1 & RF_PACKETCONFIG1_CRC_ON ) == RF_PACKETCONFIG1_CRC_ON )
        {
            //                           CrcOk,                   FifoLevel,               SyncAddr,               FifoEmpty
            SX1272->RegDioMapping1 = RF_DIOMAPPING1_DIO0_01 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_11 | RF_DIOMAPPING1_DIO3_00;
        }
        else
        {
            //                           PayloadReady,            FifoLevel,               SyncAddr,               FifoEmpty
            SX1272->RegDioMapping1 = RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_11 | RF_DIOMAPPING1_DIO3_00;
        }
        //                          Preamble,                   Data
        SX1272->RegDioMapping2 = RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_10 | RF_DIOMAPPING2_MAP_PREAMBLEDETECT;
        SX1272WriteBuffer( REG_DIOMAPPING1, &SX1272->RegDioMapping1, 2 );

        SX1272FskSetOpMode( RF_OPMODE_RECEIVER );
    
        memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );

        PacketTimeout = ( uint16_t )( round( ( 8.0 * ( ( double )SX1272FskGetPacketPayloadSize( ) ) / ( double )FskSettings.Bitrate ) * 1000.0 ) + 1.0 );
        PacketTimeout = PacketTimeout + ( PacketTimeout >> 1 ); // Set the Packet timeout as 1.5 times the full payload transmission time

        Preamble2SyncTimeout = PacketTimeout;

        Preamble2SyncTimer = RxTimeoutTimer = GET_TICK_COUNT( );

        SX1272->RegFifoThresh = RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY | 0x20; // 32 bytes of data
        SX1272Write( REG_FIFOTHRESH, SX1272->RegFifoThresh );

        PreambleDetected = false;
        SyncWordDetected = false;
        PacketDetected = false;
        RxBytesRead = 0;
        RxPacketSize = 0;
        RFState = RF_STATE_RX_SYNC;
        break;
    case RF_STATE_RX_SYNC:
        if( ( DIO4 == 1 ) && ( PreambleDetected == false ) )// Preamble
        {
            PreambleDetected = true;
            Preamble2SyncTimer = GET_TICK_COUNT( );
        }
        if( ( DIO2 == 1 ) && ( PreambleDetected == true ) && ( SyncWordDetected == false ) ) // SyncAddr
        {
            SyncWordDetected = true;
        
            RxPacketRssiValue = SX1272FskReadRssi( );

            RxPacketAfcValue = SX1272FskReadAfc( );
            RxGain = SX1272FskReadRxGain( );
        
            Preamble2SyncTimer = RxTimeoutTimer = GET_TICK_COUNT( );

            RFState = RF_STATE_RX_RUNNING;
        }

        // Preamble 2 SyncAddr timeout
        if( ( SyncWordDetected == false ) && ( PreambleDetected == true ) && ( ( GET_TICK_COUNT( ) - Preamble2SyncTimer ) > Preamble2SyncTimeout ) )
        {
            RFState = RF_STATE_RX_INIT;
            SX1272Write( REG_RXCONFIG, SX1272->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK );
        }
        if( ( SyncWordDetected == false ) &&
            ( PreambleDetected == false ) &&
            ( PacketDetected == false ) &&
            ( ( GET_TICK_COUNT( ) - RxTimeoutTimer ) > PacketTimeout ) )
        {
            RFState = RF_STATE_RX_TIMEOUT;
        }
        break;
    case RF_STATE_RX_RUNNING:
        if( RxPacketSize > RF_BUFFER_SIZE_MAX )
        {
            RFState = RF_STATE_RX_LEN_ERROR;
            break;
        }
#if 1
        if( DIO1 == 1 ) // FifoLevel
        {
            if( ( RxPacketSize == 0 ) && ( RxBytesRead == 0 ) ) // Read received packet size
            {
                if( ( SX1272->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE )
                {
                    SX1272ReadFifo( ( uint8_t* )&RxPacketSize, 1 );
                }
                else
                {
                    RxPacketSize = SX1272->RegPayloadLength;
                }
            }

            if( ( RxPacketSize - RxBytesRead ) > ( SX1272->RegFifoThresh & 0x3F ) )
            {
                SX1272ReadFifo( ( RFBuffer + RxBytesRead ), ( SX1272->RegFifoThresh & 0x3F ) );
                RxBytesRead += ( SX1272->RegFifoThresh & 0x3F );
            }
            else
            {
                SX1272ReadFifo( ( RFBuffer + RxBytesRead ), RxPacketSize - RxBytesRead );
                RxBytesRead += ( RxPacketSize - RxBytesRead );
            }
        }
#endif
        if( DIO0 == 1 ) // PayloadReady/CrcOk
        {
            RxTimeoutTimer = GET_TICK_COUNT( );
            if( ( RxPacketSize == 0 ) && ( RxBytesRead == 0 ) ) // Read received packet size
            {
                if( ( SX1272->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE )
                {
                    SX1272ReadFifo( ( uint8_t* )&RxPacketSize, 1 );
                }
                else
                {
                    RxPacketSize = SX1272->RegPayloadLength;
                }
                SX1272ReadFifo( RFBuffer + RxBytesRead, RxPacketSize - RxBytesRead );
                RxBytesRead += ( RxPacketSize - RxBytesRead );
                PacketDetected = true;
                RFState = RF_STATE_RX_DONE;
            }
            else
            {
                SX1272ReadFifo( RFBuffer + RxBytesRead, RxPacketSize - RxBytesRead );
                RxBytesRead += ( RxPacketSize - RxBytesRead );
                PacketDetected = true;
                RFState = RF_STATE_RX_DONE;
            }
        }
        
        // Packet timeout
        if( ( PacketDetected == false ) && ( ( GET_TICK_COUNT( ) - RxTimeoutTimer ) > PacketTimeout ) )
        {
            RFState = RF_STATE_RX_TIMEOUT;
        }
        break;
    case RF_STATE_RX_DONE:
        RxBytesRead = 0;
        RFState = RF_STATE_RX_INIT;
        result = RF_RX_DONE;
        break;
    case RF_STATE_RX_TIMEOUT:
        RxBytesRead = 0;
        RxPacketSize = 0;
        SX1272Write( REG_RXCONFIG, SX1272->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK );
        RFState = RF_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;
    case RF_STATE_RX_LEN_ERROR:
        RxBytesRead = 0;
        RxPacketSize = 0;
        SX1272Write( REG_RXCONFIG, SX1272->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK );
        RFState = RF_STATE_RX_INIT;
        result = RF_LEN_ERROR;
        break;
    // Tx management
    case RF_STATE_TX_INIT:
        // Packet DIO mapping setup
        //                           PacketSent,               FifoLevel,              FifoFull,               TxReady
        SX1272->RegDioMapping1 = RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_01;
        //                           LowBat,                   Data
        SX1272->RegDioMapping2 = RF_DIOMAPPING2_DIO4_00 | RF_DIOMAPPING2_DIO5_10;
        SX1272WriteBuffer( REG_DIOMAPPING1, &SX1272->RegDioMapping1, 2 );

        SX1272->RegFifoThresh = RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY | 0x18; // 24 bytes of data
        SX1272Write( REG_FIFOTHRESH, SX1272->RegFifoThresh );

        SX1272FskSetOpMode( RF_OPMODE_TRANSMITTER );
        RFState = RF_STATE_TX_READY_WAIT;
        TxBytesSent = 0;
        break;
    case RF_STATE_TX_READY_WAIT:
        if( DIO3 == 1 )    // TxReady
        {
            if( ( SX1272->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE )
            {
                SX1272WriteFifo( ( uint8_t* )&TxPacketSize, 1 );
            }
            
            if( ( TxPacketSize > 0 ) && ( TxPacketSize <= 64 ) )
            {
                DataChunkSize = TxPacketSize;
            }
            else
            {
                DataChunkSize = 32;
            }
            
            SX1272WriteFifo( RFBuffer, DataChunkSize );
            TxBytesSent += DataChunkSize;
            TxTimeoutTimer = GET_TICK_COUNT( );
            RFState = RF_STATE_TX_RUNNING;
        }
        break;

    case RF_STATE_TX_RUNNING:
        if( DIO1 == 0 )    // FifoLevel below thresold
        {  
            if( ( TxPacketSize - TxBytesSent ) > DataChunkSize )
            {
                SX1272WriteFifo( ( RFBuffer + TxBytesSent ), DataChunkSize );
                TxBytesSent += DataChunkSize;
            }
            else 
            {
                // we write the last chunk of data
                SX1272WriteFifo( RFBuffer + TxBytesSent, TxPacketSize - TxBytesSent );
                TxBytesSent += TxPacketSize - TxBytesSent;
            }
        }

        if( DIO0 == 1 ) // PacketSent
        {
            TxTimeoutTimer = GET_TICK_COUNT( );
            RFState = RF_STATE_TX_DONE;
            SX1272FskSetOpMode( RF_OPMODE_STANDBY );
        }
         
        // Packet timeout
        if( ( GET_TICK_COUNT( ) - TxTimeoutTimer ) > TICK_RATE_MS( 1000 ) )
        {
            RFState = RF_STATE_TX_TIMEOUT;
        }
        break;
    case RF_STATE_TX_DONE:
        RFState = RF_STATE_IDLE;
        result = RF_TX_DONE;
        break;
    case RF_STATE_TX_TIMEOUT:
        RFState = RF_STATE_IDLE;
        result = RF_TX_TIMEOUT;
        break;
    default:
        break;
    }
    return result;
}

#endif // USE_SX1272_RADIO
