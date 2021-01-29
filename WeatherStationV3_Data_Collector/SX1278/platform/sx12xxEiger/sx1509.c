#include "sx1509.h"

#include "i2c.h"

#define DEVICE_I2C_ADDRESS                          0x3E

static uint8_t I2cDeviceAddr = DEVICE_I2C_ADDRESS;

uint8_t SX1509Reset( )
{
    if( SX1509Write( RegReset, 0x12 ) == SX_OK )
    {
        if( SX1509Write( RegReset, 0x34 ) == SX_OK )
        {
            return SX_OK;
        }
    }
    return SX_ERROR;
}

uint8_t SX1509Write( uint8_t addr, uint8_t data )
{
    return SX1509WriteBuffer( addr, &data, 1 );
}

uint8_t SX1509WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cWriteBuffer( I2cDeviceAddr << 1, addr, data, size, I2C_ADDR_SIZE_8 );
}

uint8_t SX1509Read( uint8_t addr, uint8_t *data )
{
    return SX1509ReadBuffer( addr, data, 1 );
}

uint8_t SX1509ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cReadBuffer( I2cDeviceAddr << 1, addr, data, size, I2C_ADDR_SIZE_8 );
}

void SX1509SetDeviceAddr( uint8_t addr )
{
    I2cDeviceAddr = addr;
}

uint8_t SX1509GetDeviceAddr( void )
{
    return I2cDeviceAddr;
}
