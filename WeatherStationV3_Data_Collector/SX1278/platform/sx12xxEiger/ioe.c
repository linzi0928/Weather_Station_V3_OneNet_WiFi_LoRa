#include <stdint.h>

#include "sx1509.h"

#include "ioe.h"

uint8_t IoePinPort[32] = { 
// IOE 1
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
// IOE 2
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataA,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB,
                        RegDataB
                    };

uint8_t IoePin[32] = {
// IOE 1
                    IOE_RF_DIO3_PIN,
                    IOE_RF_DIO4_PIN,
                    IOE_RF_DIO5_PIN,
                    IOE_RF_RXTX_PIN,
                    IOE_ADC_READY_PIN,
                    IOE_IOE2_NINT_PIN,
                    IOE_SHDN_VSUPC_PIN,
                    IOE_SHDN_VFEM_PIN,
                    IOE_FEM_ANT_SEL_PIN,
                    IOE_FEM_CSD_PIN,
                    IOE_FEM_CPS_PIN,
                    IOE_FEM_CTX_PIN,
                    IOE_FEM_SPARE_PIN,
                    0x20,
                    0x40,
                    0x80,
// IOE 2
                    IOE_LED_3_PIN,
                    IOE_LED_2_PIN,
                    IOE_LED_1_PIN,
                    IOE_TEMP_ALERT_PIN,
                    0x10,
                    0x20,
                    0x40,
                    0x80,
                    IOE_SX_GPIO0_PIN,
                    IOE_SX_GPIO1_PIN,
                    IOE_SX_GPIO2_PIN,
                    IOE_SX_GPIO3_PIN,
                    IOE_SX_GPIO4_PIN,
                    IOE_SX_GPIO5_PIN,
                    IOE_SX_GPIO6_PIN,
                    IOE_SX_GPIO7_PIN
    };
                    
uint8_t IoePortValue[4];

static void IoePinWrite( uint8_t port, uint8_t pin, uint8_t enable )
{
    uint8_t data = 0;

    SX1509Read( port, &data );
    if( enable == 1 )
    {
        SX1509Write( port, data | pin );
    }
    else
    {
        SX1509Write( port, data & ( ~pin ) );
    }
}

static uint8_t IoePinRead( uint8_t port, uint8_t pin )
{
    uint8_t data = 0;
    uint8_t trial = 10;

    while( trial-- )
    {
        if( SX1509Read( port, &data ) == SX_OK )
        {
            trial = 0;
        }
    }
    return ( ( data & pin ) == pin ) ? 1 : 0;
}

void IoePinOn( tIoePin pin )
{
    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3F );
    }
    
    IoePinWrite( IoePinPort[pin], IoePin[pin], 1 );

    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3E );
    }
}

void IoePinOff( tIoePin pin )
{
    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3F );
    }

    IoePinWrite( IoePinPort[pin], IoePin[pin], 0 );

    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3E );
    }
}

void IoePinToggle( tIoePin pin )
{
    uint8_t pinValue = 0;
    
    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3F );
    }

    pinValue = IoePinRead( IoePinPort[pin], IoePin[pin] );
    pinValue ^= 1;
    IoePinWrite( IoePinPort[pin], IoePin[pin], pinValue );
    
    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3E );
    }
}

uint8_t IoePinGet( tIoePin pin )
{
    uint8_t pinValue = 0;

    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3F );
    }

    pinValue = IoePinRead( IoePinPort[pin], IoePin[pin] );
    //switch( IoePinPort[pin] )
    //{
    //    case RegDataA:
    //        pinValue = ( IoePortValue[0] & IoePin[pin] ) == IoePin[pin];
    //        break;
    //    case RegDataB:
    //        pinValue = ( IoePortValue[1] & IoePin[pin] ) == IoePin[pin];
    //        break;
    //}
    
    if( pin > 15 )
    {
        SX1509SetDeviceAddr( 0x3E );
    }

    return pinValue;
}

void IoeInit( void )
{
    //-------------------------------------------------------------------------
    // Main IO expander initialization
    //-------------------------------------------------------------------------
    SX1509SetDeviceAddr( 0x3E );
    SX1509Reset( );

    SX1509Write( RegClock, 0x40 );

    // SHDN1 SHDN0 SIOEI READY RXTX  DIO5  DIO4  DIO3
    //  OUT   OUT   IN    IN    IN    IN    IN    IN
    SX1509Write( RegDirA, 0x3F );
    
    // N/A   N/A   N/A   SPARE CTX   CPS   CSD   ANT_S
    //  IN    IN    IN    IN    OUT   OUT   OUT   OUT
    SX1509Write( RegDirB, 0xF0 );
    
#if defined( USE_FEM_BOARD )
    SX1509Write( RegOpenDrainB, 0x0F );
    //SX1509Write( RegInputDisableB, 0x0F );
#endif

    SX1509Write( RegSenseHighA, 0x0F );
    SX1509Write( RegSenseLowA, 0xFF );

    SX1509Write( RegSenseHighB, 0x00 );
    SX1509Write( RegSenseLowB, 0x00 );
    
    SX1509Write( RegInterruptMaskA, 0xC4 );
    SX1509Write( RegInterruptMaskB, 0xFF );

    SX1509Write( RegDataA, 0x00 );
    SX1509Write( RegDataB, 0xEF );

    //-------------------------------------------------------------------------
    // Secondary IO expander initialization
    //-------------------------------------------------------------------------
    SX1509SetDeviceAddr( 0x3F );
    SX1509Reset( );

    SX1509Write( RegClock, 0x40 );

    // N/A   N/A   N/A   N/A   ALERT LED3  LED2  LED1
    //  IN    IN    IN    IN    IN    OUT   OUT   OUT
    SX1509Write( RegDirA, 0xF8 );
    SX1509Write( RegOpenDrainA, 0x07 );
    //SX1509Write( RegInputDisableA, 0x07 );
    
    // GPIO7 GPIO6 GPIO5 GPIO4 GPIO3 GPIO2 GPIO1 GPIO0
    //  IN    IN    IN    IN    IN    IN    IN    IN  
    SX1509Write( RegDirB, 0xFF );

    SX1509Write( RegSenseHighA, 0x00 );
    SX1509Write( RegSenseLowA, 0xC0 );

    SX1509Write( RegSenseHighB, 0x00 );
    SX1509Write( RegSenseLowB, 0x00 );
    
    SX1509Write( RegInterruptMaskA, 0xF7 );
    SX1509Write( RegInterruptMaskB, 0xFF );

    SX1509Write( RegDataA, 0x07 );
    SX1509Write( RegDataB, 0x00 );

    SX1509SetDeviceAddr( 0x3E );
}
