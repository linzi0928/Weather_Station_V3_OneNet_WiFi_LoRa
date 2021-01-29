#ifndef __I2C_H__
#define __I2C_H__

typedef enum
{
    I2C_ADDR_SIZE_8 = 0,
    I2C_ADDR_SIZE_16,
}tI2cAddrSize;

#if defined( STM32F4XX ) || defined( STM32F2XX )

#define I2C_INTERFACE                I2C2
#define I2C_CLK                      RCC_APB1Periph_I2C2
#define I2C_SCL_PIN                  GPIO_Pin_4                  /* PH.04 */
#define I2C_SCL_GPIO_PORT            GPIOH                       /* GPIOH */
#define I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOH
#define I2C_SCL_SOURCE               GPIO_PinSource4
#define I2C_SCL_AF                   GPIO_AF_I2C2
#define I2C_SDA_PIN                  GPIO_Pin_5                  /* PH.05 */
#define I2C_SDA_GPIO_PORT            GPIOH                       /* GPIOH */
#define I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOH
#define I2C_SDA_SOURCE               GPIO_PinSource5
#define I2C_SDA_AF                   GPIO_AF_I2C2
#define I2C_DR                       ( ( uint32_t )0x40005410 )
#define I2C_SPEED                    400000

#define I2C_DMA_CLK                  RCC_AHB1Periph_DMA1
#define I2C_DMA_CHANNEL              DMA_Channel_1
#define I2C_DMA_TX_STREAM            DMA1_Stream6   
#define I2C_DMA_RX_STREAM            DMA1_Stream0
#define I2C_DMA_TX_TCFLAG            DMA_FLAG_TCIF6
#define I2C_DMA_RX_TCFLAG            DMA_FLAG_TCIF0
   
#define I2C_DMA_DIR_TX               0
#define I2C_DMA_DIR_RX               1  

#else

#define I2C_INTERFACE                I2C1
#define I2C_CLK                      RCC_APB1Periph_I2C1
#define I2C_SCL_PIN                  GPIO_Pin_6                  /* PB.06 */
#define I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define I2C_SDA_PIN                  GPIO_Pin_7                  /* PB.07 */
#define I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB
#define I2C_DR                       ( ( uint32_t )0x40005410 )
#define I2C_SPEED                    400000

#define I2C_DMA                      DMA1
#define I2C_DMA_CLK                  RCC_AHBPeriph_DMA1
#define I2C_DMA_TX_CHANNEL           DMA1_Channel6
#define I2C_DMA_RX_CHANNEL           DMA1_Channel7
#define I2C_DMA_TX_TCFLAG            DMA1_FLAG_TC6
#define I2C_DMA_RX_TCFLAG            DMA1_FLAG_TC7

#define I2C_DMA_DIR_TX               0
#define I2C_DMA_DIR_RX               1  

#endif

void I2cInit( void );
uint8_t I2cWrite( uint8_t deviceAddr, uint8_t data );
uint8_t I2cRead( uint8_t deviceAddr, uint8_t *data );
uint8_t I2cWriteBuffer( uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size, tI2cAddrSize addrSize );
uint8_t I2cReadBuffer( uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size, tI2cAddrSize addrSize );

#endif // __I2C_H__
