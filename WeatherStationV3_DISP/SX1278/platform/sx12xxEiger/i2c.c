#include "sx12xxEiger.h"

#include "i2c.h"

#define TIMEOUT_MAX                                 0x3000 /*<! The value of the maximal timeout for I2C waiting loops */

static void I2cGpioConfig( void );
static void I2cConfig( void );

#if 0
static void I2cDmaConfig( uint8_t* buffer, uint32_t size, uint32_t direction );
#endif

#if ( defined( STM32F4XX ) || defined( STM32F2XX ) )

static void I2cFsmcClkEnable( bool enable )
{
    
}

#else

static void I2cFsmcClkEnable( bool enable )
{
    __IO int32_t i;
    
    if( enable == true )
    {
        for( i = 0; i < 10000; i++ );
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_FSMC, ENABLE );
    }
    else
    {
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_FSMC, DISABLE );
        for( i = 0; i < 10000; i++ );
    }
}

#endif

void I2cInit( void )
{
    I2C_DeInit( I2C_INTERFACE );

    I2cGpioConfig( );
    I2cConfig( );
}

uint8_t I2cWrite( uint8_t deviceAddr, uint8_t data )
{
    __IO int32_t i;
    uint32_t timeOut;

    I2cFsmcClkEnable( false );
   
    /* Test on BUSY Flag */
    timeOut = TIMEOUT_MAX;
    while( I2C_GetFlagStatus( I2C_INTERFACE, I2C_FLAG_BUSY) ) 
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send START condition */
    I2C_GenerateSTART( I2C_INTERFACE, ENABLE );

    /* Test on EV5 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_MODE_SELECT ) )
    {
        if( I2C_INTERFACE->SR1 & I2C_FLAG_RXNE )
        {
            volatile uint32_t drTemp;
            drTemp = I2C_INTERFACE->DR;
        }
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }
    /* Send device's address for write */
    I2C_Send7bitAddress( I2C_INTERFACE, deviceAddr, I2C_Direction_Transmitter );

    /* Test on EV6 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    I2C_SendData( I2C_INTERFACE, data );

    /* Test on EV8 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    I2C_GenerateSTOP( I2C_INTERFACE, ENABLE ); 
    for( i = 0 ; i < 500 ; i++ ){ ; }

    I2cFsmcClkEnable( true );
    return( SX_OK );
}

uint8_t I2cRead( uint8_t deviceAddr, uint8_t *data )
{
    __IO int32_t i;
    int32_t size = 1;
    uint32_t timeOut;

    I2cFsmcClkEnable( false );
    
    /* Test on BUSY Flag */
    timeOut = TIMEOUT_MAX;
    while( I2C_GetFlagStatus( I2C_INTERFACE, I2C_FLAG_BUSY) ) 
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send START condition a second time */
    I2C_GenerateSTART( I2C_INTERFACE, ENABLE );

    /* Test on EV5 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_MODE_SELECT ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send device's address for read */
    I2C_Send7bitAddress( I2C_INTERFACE, deviceAddr, I2C_Direction_Receiver );

    /* Test on EV6 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* While there is data to be read */
    while( size )
    {
        if( size == 1 )
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig( I2C_INTERFACE, DISABLE );

            /* Send STOP Condition */
            I2C_GenerateSTOP( I2C_INTERFACE, ENABLE );
        }

        /* Test on EV7 and clear it */
        timeOut = TIMEOUT_MAX;
        if( I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_RECEIVED ) )
        {
            /* Read a byte from the device */
            *data = I2C_ReceiveData( I2C_INTERFACE );

            /* Decrement the read bytes counter */
            size--;
        }
        else
        {
            if( ( timeOut-- ) == 0 )
            {
                I2cInit( );
                I2cFsmcClkEnable( true );
                return( SX_TIMEOUT );
            }
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig( I2C_INTERFACE, ENABLE );

    for( i = 0 ; i < 500 ; i++ ){ ; }

    I2cFsmcClkEnable( true );
    return( SX_OK );
}

uint8_t I2cWriteBuffer( uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size, tI2cAddrSize addrSize )
{
    __IO int32_t i;
    uint32_t timeOut;

    I2cFsmcClkEnable( false );

    /* Test on BUSY Flag */
    timeOut = TIMEOUT_MAX;
    while( I2C_GetFlagStatus( I2C_INTERFACE, I2C_FLAG_BUSY) ) 
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send START condition */
    I2C_GenerateSTART( I2C_INTERFACE, ENABLE );

    /* Test on EV5 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_MODE_SELECT ) )
    {
        if( I2C_INTERFACE->SR1 & I2C_FLAG_RXNE )
        {
            volatile uint32_t drTemp;
            drTemp = I2C_INTERFACE->DR;
        }
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send device's address for write */
    I2C_Send7bitAddress( I2C_INTERFACE, deviceAddr, I2C_Direction_Transmitter );

    /* Test on EV6 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    if( addrSize == I2C_ADDR_SIZE_16 )
    {
        /* Send the device's internal address MSB to write to */
        I2C_SendData( I2C_INTERFACE, ( uint8_t )( ( addr & 0xFF00 ) >> 8 )  );

        /* Test on EV8 and clear it */
        timeOut = TIMEOUT_MAX;
        while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
        {
            if( ( timeOut-- ) == 0 )
            {
                I2cInit( );
                I2cFsmcClkEnable( true );
                return( SX_TIMEOUT );
            }
        }
    }

    /* Send the device's internal address LSB to write to */
    I2C_SendData( I2C_INTERFACE, ( uint8_t )( addr & 0x00FF ) );

    /* Test on EV8 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    while( size )
    {
        /* Send the byte to be written */
        I2C_SendData( I2C_INTERFACE, * buffer );

        /* Test on EV8 and clear it */
        timeOut = TIMEOUT_MAX;
        while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
        {
            if( ( timeOut-- ) == 0 )
            {
                I2cInit( );
                I2cFsmcClkEnable( true );
                return( SX_TIMEOUT );
            }
        }

        if( size == 1 )
        { 
            I2C_GenerateSTOP( I2C_INTERFACE, ENABLE ); 
        } /* STOP */

        buffer++;
        size--;
    }

    //I2C_GenerateSTOP( I2C_INTERFACE, ENABLE ); 
    for( i = 0 ; i < 500 ; i++ ){ ; }

    I2cFsmcClkEnable( true );
    return( SX_OK );
}

uint8_t I2cReadBuffer( uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size, tI2cAddrSize addrSize )
{
    __IO int32_t i;
    uint32_t timeOut;

    I2cFsmcClkEnable( false );

    /* Test on BUSY Flag */
    timeOut = TIMEOUT_MAX;
    while( I2C_GetFlagStatus( I2C_INTERFACE, I2C_FLAG_BUSY) ) 
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send START condition */
    I2C_GenerateSTART( I2C_INTERFACE, ENABLE );

    /* Test on EV5 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_MODE_SELECT ) )
    {
        if( I2C_INTERFACE->SR1 & I2C_FLAG_RXNE )
        {
            volatile uint32_t drTemp;
            drTemp = I2C_INTERFACE->DR;
        }
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send device's address for write */
    I2C_Send7bitAddress( I2C_INTERFACE, deviceAddr, I2C_Direction_Transmitter );


    /* Test on EV6 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd( I2C_INTERFACE, ENABLE );

    if( addrSize == I2C_ADDR_SIZE_16 )
    {
        /* Send the device's internal address MSB to write to */
        I2C_SendData( I2C_INTERFACE, ( uint8_t )( ( addr & 0xFF00 ) >> 8 )  );

        /* Test on EV8 and clear it */
        timeOut = TIMEOUT_MAX;
        while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
        {
            if( ( timeOut-- ) == 0 )
            {
                I2cInit( );
                I2cFsmcClkEnable( true );
                return( SX_TIMEOUT );
            }
        }
    }

    /* Send the device's internal address LSB to write to */
    I2C_SendData( I2C_INTERFACE, ( uint8_t )( addr & 0x00FF ) );

    /* Test on EV8 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send START condition a second time */
    I2C_GenerateSTART( I2C_INTERFACE, ENABLE );

    /* Test on EV5 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_MODE_SELECT ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* Send device's address for read */
    I2C_Send7bitAddress( I2C_INTERFACE, deviceAddr, I2C_Direction_Receiver );

    /* Test on EV6 and clear it */
    timeOut = TIMEOUT_MAX;
    while( !I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) )
    {
        if( ( timeOut-- ) == 0 )
        {
            I2cInit( );
            I2cFsmcClkEnable( true );
            return( SX_TIMEOUT );
        }
    }

    /* While there is data to be read */
    timeOut = TIMEOUT_MAX;
    while( size )
    {
        if( size == 1 )
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig( I2C_INTERFACE, DISABLE );

            /* Send STOP Condition */
            I2C_GenerateSTOP( I2C_INTERFACE, ENABLE );
        }

        /* Test on EV7 and clear it */
        if( I2C_CheckEvent( I2C_INTERFACE, I2C_EVENT_MASTER_BYTE_RECEIVED ) )
        {
            /* Read a byte from the device */
            *buffer = I2C_ReceiveData( I2C_INTERFACE );

            /* Point to the next location where the byte read will be saved */
            buffer++;

            /* Decrement the read bytes counter */
            size--;
        }
        else
        {
            if( ( timeOut-- ) == 0 )
            {
                I2cInit( );
                I2cFsmcClkEnable( true );
                return( SX_TIMEOUT );
            }
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig( I2C_INTERFACE, ENABLE );

    for( i = 0 ; i < 500 ; i++ ){ ; }

    I2cFsmcClkEnable( true );
    return( SX_OK );

}

/**
  * @brief  Initializes the GPIO pins used by the I2C interface.
  * @param  None
  * @retval None
  */
static void I2cGpioConfig( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable I2C_INTERFACE and I2C_GPIO_PORT & Alternate Function clocks */
    RCC_APB1PeriphClockCmd( I2C_CLK, ENABLE );

#if defined( STM32F4XX ) || defined( STM32F2XX )

    RCC_AHB1PeriphClockCmd( I2C_SCL_GPIO_CLK | I2C_SDA_GPIO_CLK, ENABLE );

    /* Configure I2C pins: SCL, SDA */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( I2C_SCL_GPIO_PORT, &GPIO_InitStructure );
    GPIO_Init( I2C_SDA_GPIO_PORT, &GPIO_InitStructure );

    /* Connect Pxx to I2C_SCL, I2C_SDA */
    GPIO_PinAFConfig( I2C_SCL_GPIO_PORT, I2C_SCL_SOURCE, I2C_SCL_AF );
    GPIO_PinAFConfig( I2C_SDA_GPIO_PORT, I2C_SDA_SOURCE, I2C_SDA_AF );  

#else

    RCC_APB2PeriphClockCmd( I2C_SCL_GPIO_CLK | I2C_SDA_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE );

    /* Reset I2C_INTERFACE IP */
    RCC_APB1PeriphResetCmd( I2C_CLK, ENABLE );

    /* Release reset signal of I2C_INTERFACE IP */
    RCC_APB1PeriphResetCmd( I2C_CLK, DISABLE );

    /* I2C_INTERFACE SCL and SDA pins configuration */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init( I2C_SCL_GPIO_PORT, &GPIO_InitStructure );

    /* I2C_INTERFACE SCL and SDA pins configuration */
    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
    GPIO_Init( I2C_SDA_GPIO_PORT, &GPIO_InitStructure );

#endif
}

/**
  * @brief  Configure the I2C Peripheral.
  * @param  None
  * @retval None
  */
static void I2cConfig( void )
{
    I2C_InitTypeDef I2C_InitStructure;

    /* I2C_INTERFACE configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
    I2C_Init( I2C_INTERFACE, &I2C_InitStructure );

#if defined( STM32F4XX ) || defined( STM32F2XX )

    /* I2C Peripheral Enable */
    I2C_Cmd( I2C_INTERFACE, ENABLE );

#endif

}
#if 0
/**
 * @brief  Configure the DMA Peripheral used to handle communication via I2C.
 * @param  None
 * @retval None
 */
void I2cDmaConfig( uint8_t* buffer, uint32_t size, uint32_t direction )
{

#if defined( STM32F4XX ) || defined( STM32F2XX )

    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd( I2C_DMA_CLK, ENABLE );

    /* Initialize the DMA_Channel member */
    DMA_InitStructure.DMA_Channel = I2C_DMA_CHANNEL;

    /* Initialize the DMA_PeripheralBaseAddr member */
    DMA_InitStructure.DMA_PeripheralBaseAddr = I2C_DR;

    /* Initialize the DMA_Memory0BaseAddr member */
    DMA_InitStructure.DMA_Memory0BaseAddr = ( uint32_t )buffer;

    /* Initialize the DMA_PeripheralInc member */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

    /* Initialize the DMA_MemoryInc member */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    /* Initialize the DMA_PeripheralDataSize member */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

    /* Initialize the DMA_MemoryDataSize member */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

    /* Initialize the DMA_Mode member */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

    /* Initialize the DMA_Priority member */
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;

    /* Initialize the DMA_FIFOMode member */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;

    /* Initialize the DMA_FIFOThreshold member */
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;

    /* Initialize the DMA_MemoryBurst member */
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;

    /* Initialize the DMA_PeripheralBurst member */
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    /* If using DMA for Reception */
    if( direction == I2C_DMA_DIR_RX )
    {    
        /* Initialize the DMA_DIR member */
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;

        /* Initialize the DMA_BufferSize member */
        DMA_InitStructure.DMA_BufferSize = size;

        DMA_DeInit( I2C_DMA_RX_STREAM );

        DMA_Init( I2C_DMA_RX_STREAM, &DMA_InitStructure );
    }
    /* If using DMA for Transmission */
    else if( direction == I2C_DMA_DIR_TX )
    { 
        /* Initialize the DMA_DIR member */
        DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;

        /* Initialize the DMA_BufferSize member */
        DMA_InitStructure.DMA_BufferSize = size;

        DMA_DeInit( I2C_DMA_TX_STREAM );

        DMA_Init( I2C_DMA_TX_STREAM, &DMA_InitStructure );
    }
#else

    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd( I2C_DMA_CLK, ENABLE );

    /* Initialize the DMA_PeripheralBaseAddr member */
    DMA_InitStructure.DMA_PeripheralBaseAddr = I2C_DR;
    /* Initialize the DMA_MemoryBaseAddr member */
    DMA_InitStructure.DMA_MemoryBaseAddr = ( uint32_t )buffer;
    /* Initialize the DMA_PeripheralInc member */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    /* Initialize the DMA_MemoryInc member */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /* Initialize the DMA_PeripheralDataSize member */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    /* Initialize the DMA_MemoryDataSize member */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    /* Initialize the DMA_Mode member */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    /* Initialize the DMA_Priority member */
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    /* Initialize the DMA_M2M member */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    /* If using DMA for Reception */
    if( direction == I2C_DMA_DIR_RX )
    {
        /* Initialize the DMA_DIR member */
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

        /* Initialize the DMA_BufferSize member */
        DMA_InitStructure.DMA_BufferSize = size;

        DMA_DeInit( I2C_DMA_RX_CHANNEL );

        DMA_Init( I2C_DMA_RX_CHANNEL, &DMA_InitStructure );
    }
    /* If using DMA for Transmission */
    else if( direction == I2C_DMA_DIR_TX )
    { 
        /* Initialize the DMA_DIR member */
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

        /* Initialize the DMA_BufferSize member */
        DMA_InitStructure.DMA_BufferSize = size;

        DMA_DeInit( I2C_DMA_TX_CHANNEL );

        DMA_Init( I2C_DMA_TX_CHANNEL, &DMA_InitStructure );
    }

#endif
}
#endif
