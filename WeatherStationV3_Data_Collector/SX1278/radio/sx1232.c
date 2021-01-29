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
 * \file       sx1232.c
 * \brief      SX1232 RF chip driver
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

#if defined( USE_SX1232_RADIO )

#include "radio.h"

#include "sx1232-Hal.h"
#include "sx1232.h"

#include "sx1232-Misc.h"

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
 * SX1232 registers variable
 */
tSX1232* SX1232;
uint8_t SX1232Regs[0x70];


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

void SX1232Init( void )
{
    RFState = RF_STATE_IDLE;

    SX1232 = ( tSX1232* )SX1232Regs;

    SX1232InitIo( );

    SX1232Reset( );

    SX1232SetDefaults( );

    SX1232ReadBuffer( REG_OPMODE, SX1232Regs + 1, 0x70 - 1 );

    // Launch Rx chain Calibration
    SX1232RxCalibrate( );

    // Set the device in FSK mode and Sleep Mode
    SX1232->RegOpMode = RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_SLEEP;
    SX1232Write( REG_OPMODE, SX1232->RegOpMode );

    // Then we initialize the device register structure with the value of our setup
    SX1232->RegPaConfig = ( SX1232->RegPaConfig & RF_PACONFIG_PASELECT_PABOOST ) | RF_PACONFIG_PASELECT_PABOOST;

    SX1232->RegLna = RF_LNA_GAIN_G1 | RF_LNA_BOOST_ON;

    if( FskSettings.AfcOn == true )
    {
        SX1232->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_ON |
                              RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }
    else
    {
        SX1232->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_OFF |
                              RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }

    SX1232->RegPreambleLsb = 8;
    
    SX1232->RegPreambleDetect = RF_PREAMBLEDETECT_DETECTOR_ON | RF_PREAMBLEDETECT_DETECTORSIZE_2 |
                                RF_PREAMBLEDETECT_DETECTORTOL_10;
    
    SX1232->RegRssiThresh = 0xFF;

    SX1232->RegSyncConfig = RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA |
                            RF_SYNCCONFIG_SYNC_ON | RF_SYNCCONFIG_FIFOFILLCONDITION_AUTO |
                            RF_SYNCCONFIG_SYNCSIZE_4;

    SX1232->RegSyncValue1 = 0x69;
    SX1232->RegSyncValue2 = 0x81;
    SX1232->RegSyncValue3 = 0x7E;
    SX1232->RegSyncValue4 = 0x96;

    SX1232->RegPacketConfig1 = RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RF_PACKETCONFIG1_DCFREE_OFF |
                               ( FskSettings.CrcOn << 4 ) | RF_PACKETCONFIG1_CRCAUTOCLEAR_ON |
                               RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT;
    SX1232GetPacketCrcOn( ); // Update CrcOn on FskSettings

    SX1232->RegPayloadLength = FskSettings.PayloadLength;

    // we can now update the registers with our configuration
    SX1232WriteBuffer( REG_OPMODE, SX1232Regs + 1, 0x70 - 1 );

    // then we need to set the RF settings 
    SX1232SetRFFrequency( FskSettings.RFFrequency );
    SX1232SetBitrate( FskSettings.Bitrate );
    SX1232SetFdev( FskSettings.Fdev );
    SX1232SetPa20dBm( true );
    SX1232SetRFPower( FskSettings.Power );
    SX1232SetDccBw( &SX1232->RegRxBw, 0, FskSettings.RxBw );
    SX1232SetDccBw( &SX1232->RegAfcBw, 0, FskSettings.RxBwAfc );
    SX1232SetRssiOffset( -6 );

    SX1232SetOpMode( RF_OPMODE_STANDBY );
}

void SX1232SetDefaults( void )
{
    // REMARK: See SX1232 datasheet for modified default values.

    uint8_t SyncWord[] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };

    SX1232Read( REG_VERSION, &SX1232->RegVersion );
    
    SX1232Write( REG_RXCONFIG, 0x0E );
    SX1232Write( REG_PREAMBLEDETECT, 0xAA );
    SX1232Write( REG_OSC, 0x07 );
    
    SX1232WriteBuffer( REG_SYNCVALUE1, SyncWord, 8 );
    
    SX1232Write( REG_FIFOTHRESH, 0x8F );
    
    SX1232Write( REG_IMAGECAL, 0x02 );
}

void SX1232Reset( void )
{
    SX1232SetReset( RADIO_RESET_ON );
    
    // Wait 1ms
    uint32_t startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 1 ) );    

    SX1232SetReset( RADIO_RESET_OFF );
    
    // Wait 6ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 6 ) );    
}

void SX1232SetOpMode( uint8_t opMode )
{
    static uint8_t opModePrev = RF_OPMODE_STANDBY;

    opModePrev = SX1232->RegOpMode & ~RF_OPMODE_MASK;

    if( opMode != opModePrev )
    {
        SX1232->RegOpMode = ( SX1232->RegOpMode & RF_OPMODE_MASK ) | opMode;

        SX1232Write( REG_OPMODE, SX1232->RegOpMode );        
    }
}

uint8_t SX1232GetOpMode( void )
{
    SX1232Read( REG_OPMODE, &SX1232->RegOpMode );
    
    return SX1232->RegOpMode & ~RF_OPMODE_MASK;
}

int32_t SX1232ReadFei( void )
{
    SX1232ReadBuffer( REG_FEIMSB, &SX1232->RegFeiMsb, 2 );                          // Reads the FEI value

    return ( int32_t )( double )( ( ( uint16_t )SX1232->RegFeiMsb << 8 ) | ( uint16_t )SX1232->RegFeiLsb ) * ( double )FREQ_STEP;
}

int32_t SX1232ReadAfc( void )
{
    SX1232ReadBuffer( REG_AFCMSB, &SX1232->RegAfcMsb, 2 );                            // Reads the AFC value
    return ( int32_t )( double )( ( ( uint16_t )SX1232->RegAfcMsb << 8 ) | ( uint16_t )SX1232->RegAfcLsb ) * ( double )FREQ_STEP;
}

uint8_t SX1232ReadRxGain( void )
{
    SX1232Read( REG_LNA, &SX1232->RegLna );
    return( SX1232->RegLna >> 5 ) & 0x07;
}

double SX1232ReadRssi( void )
{
    SX1232Read( REG_RSSIVALUE, &SX1232->RegRssiValue );                               // Reads the RSSI value

    return -( double )( ( double )SX1232->RegRssiValue / 2.0 );
}

uint8_t SX1232GetPacketRxGain( void )
{
    return RxGain;
}

double SX1232GetPacketRssi( void )
{
    return RxPacketRssiValue;
}

uint32_t SX1232GetPacketAfc( void )
{
    return RxPacketAfcValue;
}

void SX1232StartRx( void )
{
    SX1232SetRFState( RF_STATE_RX_INIT );
}

void SX1232GetRxPacket( void *buffer, uint16_t *size )
{
    *size = RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBuffer, ( size_t )*size );
}

void SX1232SetTxPacket( const void *buffer, uint16_t size )
{
    TxPacketSize = size;
    memcpy( ( void * )RFBuffer, buffer, ( size_t )TxPacketSize ); 

    RFState = RF_STATE_TX_INIT;
}

// Remark: SX1232 must be fully initialized before calling this function
uint16_t SX1232GetPacketPayloadSize( void )
{
    uint16_t syncSize;
    uint16_t variableSize;
    uint16_t addressSize;
    uint16_t payloadSize;
    uint16_t crcSize;

    syncSize = ( SX1232->RegSyncConfig & 0x07 ) + 1;
    variableSize = ( ( SX1232->RegPacketConfig1 & 0x80 ) == 0x80 ) ? 1 : 0;
    addressSize = ( ( SX1232->RegPacketConfig1 & 0x06 ) != 0x00 ) ? 1 : 0;
    payloadSize = SX1232->RegPayloadLength;
    crcSize = ( ( SX1232->RegPacketConfig1 & 0x10 ) == 0x10 ) ? 2 : 0;
    
    return syncSize + variableSize + addressSize + payloadSize + crcSize;
}

// Remark: SX1232 must be fully initialized before calling this function
uint16_t SX1232GetPacketHeaderSize( void )
{
    uint16_t preambleSize;
    uint16_t syncSize;

    preambleSize = ( ( uint16_t )SX1232->RegPreambleMsb << 8 ) | ( uint16_t )SX1232->RegPreambleLsb;
    syncSize = ( SX1232->RegSyncConfig & 0x07 ) + 1;
    
    return preambleSize + syncSize;
}

uint8_t SX1232FskGetRFState( void )
{
    return RFState;
}

void SX1232SetRFState( uint8_t state )
{
    RFState = state;
}

uint32_t SX1232Process( void )
{
    uint32_t result = RF_BUSY;

    switch( RFState )
    {
    case RF_STATE_IDLE:
        break;
    // Rx management
    case RF_STATE_RX_INIT:
        // DIO mapping setup
        if( ( SX1232->RegPacketConfig1 & RF_PACKETCONFIG1_CRC_ON ) == RF_PACKETCONFIG1_CRC_ON )
        {
            //                           CrcOk,                   FifoLevel,               SyncAddr,               FifoEmpty
            SX1232->RegDioMapping1 = RF_DIOMAPPING1_DIO0_01 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_11 | RF_DIOMAPPING1_DIO3_00;
        }
        else
        {
            //                           PayloadReady,            FifoLevel,               SyncAddr,               FifoEmpty
            SX1232->RegDioMapping1 = RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_11 | RF_DIOMAPPING1_DIO3_00;
        }
        //                          Preamble,                   Data
        SX1232->RegDioMapping2 = RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_10 | RF_DIOMAPPING2_MAP_PREAMBLEDETECT;
        SX1232WriteBuffer( REG_DIOMAPPING1, &SX1232->RegDioMapping1, 2 );

        SX1232SetOpMode( RF_OPMODE_RECEIVER );
    
        memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );

        PacketTimeout = ( uint16_t )( round( ( 8.0 * ( ( double )SX1232GetPacketPayloadSize( ) ) / ( double )FskSettings.Bitrate ) * 1000.0 ) + 1.0 );
        PacketTimeout = PacketTimeout + ( PacketTimeout >> 1 ); // Set the Packet timeout as 1.5 times the full payload transmission time

        Preamble2SyncTimeout = PacketTimeout;

        Preamble2SyncTimer = RxTimeoutTimer = GET_TICK_COUNT( );

        SX1232->RegFifoThresh = RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY | 0x20; // 32 bytes of data
        SX1232Write( REG_FIFOTHRESH, SX1232->RegFifoThresh );

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
        
            RxPacketRssiValue = SX1232ReadRssi( );

            RxPacketAfcValue = SX1232ReadAfc( );
            RxGain = SX1232ReadRxGain( );
        
            Preamble2SyncTimer = RxTimeoutTimer = GET_TICK_COUNT( );

            RFState = RF_STATE_RX_RUNNING;
        }

        // Preamble 2 SyncAddr timeout
        if( ( SyncWordDetected == false ) && ( PreambleDetected == true ) && ( ( GET_TICK_COUNT( ) - Preamble2SyncTimer ) > Preamble2SyncTimeout ) )
        {
            RFState = RF_STATE_RX_INIT;
            SX1232Write( REG_RXCONFIG, SX1232->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK );
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

        if( DIO1 == 1 ) // FifoLevel
        {
            if( ( RxPacketSize == 0 ) && ( RxBytesRead == 0 ) ) // Read received packet size
            {
                if( ( SX1232->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE )
                {
                    SX1232ReadFifo( ( uint8_t* )&RxPacketSize, 1 );
                }
                else
                {
                    RxPacketSize = SX1232->RegPayloadLength;
                }
            }

            if( ( RxPacketSize - RxBytesRead ) > ( SX1232->RegFifoThresh & 0x3F ) )
            {
                SX1232ReadFifo( ( RFBuffer + RxBytesRead ), ( SX1232->RegFifoThresh & 0x3F ) );
                RxBytesRead += ( SX1232->RegFifoThresh & 0x3F );
            }
            else
            {
                SX1232ReadFifo( ( RFBuffer + RxBytesRead ), RxPacketSize - RxBytesRead );
                RxBytesRead += ( RxPacketSize - RxBytesRead );
            }
        }

        if( DIO0 == 1 ) // PayloadReady/CrcOk
        {
            RxTimeoutTimer = GET_TICK_COUNT( );
            if( ( RxPacketSize == 0 ) && ( RxBytesRead == 0 ) ) // Read received packet size
            {
                if( ( SX1232->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE )
                {
                    SX1232ReadFifo( ( uint8_t* )&RxPacketSize, 1 );
                }
                else
                {
                    RxPacketSize = SX1232->RegPayloadLength;
                }
                SX1232ReadFifo( RFBuffer + RxBytesRead, RxPacketSize - RxBytesRead );
                RxBytesRead += ( RxPacketSize - RxBytesRead );
                PacketDetected = true;
                RFState = RF_STATE_RX_DONE;
            }
            else
            {
                SX1232ReadFifo( RFBuffer + RxBytesRead, RxPacketSize - RxBytesRead );
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
        SX1232Write( REG_RXCONFIG, SX1232->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK );
        RFState = RF_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;
    case RF_STATE_RX_LEN_ERROR:
        RxBytesRead = 0;
        RxPacketSize = 0;
        SX1232Write( REG_RXCONFIG, SX1232->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK );
        RFState = RF_STATE_RX_INIT;
        result = RF_LEN_ERROR;
        break;
    // Tx management
    case RF_STATE_TX_INIT:
        // Packet DIO mapping setup
        //                           PacketSent,               FifoLevel,              FifoFull,               TxReady
        SX1232->RegDioMapping1 = RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_01;
        //                           LowBat,                   Data
        SX1232->RegDioMapping2 = RF_DIOMAPPING2_DIO4_00 | RF_DIOMAPPING2_DIO5_10;
        SX1232WriteBuffer( REG_DIOMAPPING1, &SX1232->RegDioMapping1, 2 );

        SX1232->RegFifoThresh = RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY | 0x18; // 24 bytes of data
        SX1232Write( REG_FIFOTHRESH, SX1232->RegFifoThresh );

        SX1232SetOpMode( RF_OPMODE_TRANSMITTER );
        RFState = RF_STATE_TX_READY_WAIT;
        TxBytesSent = 0;
        break;
    case RF_STATE_TX_READY_WAIT:
        if( DIO3 == 1 )    // TxReady
        {
            if( ( SX1232->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE )
            {
                SX1232WriteFifo( ( uint8_t* )&TxPacketSize, 1 );
            }
            
            if( ( TxPacketSize > 0 ) && ( TxPacketSize <= 64 ) )
            {
                DataChunkSize = TxPacketSize;
            }
            else
            {
                DataChunkSize = 32;
            }
            
            SX1232WriteFifo( RFBuffer, DataChunkSize );
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
                SX1232WriteFifo( ( RFBuffer + TxBytesSent ), DataChunkSize );
                TxBytesSent += DataChunkSize;
            }
            else 
            {
                // we write the last chunk of data
                SX1232WriteFifo( RFBuffer + TxBytesSent, TxPacketSize - TxBytesSent );
                TxBytesSent += TxPacketSize - TxBytesSent;
            }
        }

        if( DIO0 == 1 ) // PacketSent
        {
            TxTimeoutTimer = GET_TICK_COUNT( );
            RFState = RF_STATE_TX_DONE;
            SX1232SetOpMode( RF_OPMODE_STANDBY );
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

#endif // USE_SX1232_RADIO
