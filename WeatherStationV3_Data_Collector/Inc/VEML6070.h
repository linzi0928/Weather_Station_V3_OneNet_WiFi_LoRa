#ifndef _VEML6070_H
#define _VEML6070_H
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#define u16 uint16_t
#define u8 uint8_t

#define VEML6070_SLAVE_ADDRESS    0x70
#define VEML6070_READ_VALUE1      0x71   //紫外线数据低8位
#define VEML6070_READ_VALUE2      0x73   //紫外线数据高8位

#define VEML6070_SET_VALUE        0x22   //bit7-6 0;bit5 ACK;bit4 THD;bit3 IT1;bit2 IT0;bit1 1;bit0 SD

#define VEML6070_SLEEP            0x03   //睡眠，降低功耗

#define VEML6070_RESET            0x06   //初始化时写入IIC的数据

#define VEML6070_ARA              0x18   //响应地址

#define VEML6070_INIT_TRANSFER_COUNT  5 

#define VEML6070_ACK               11
#define VEML6070_BUFFER_SIZE       2


extern u16 veml6070_val;
	
void VEML6070_Init(void);
void VEML6070_WriteCmd(u8 addr, u8 cmd_data);
u8 VEML6070_ReadData(u8 addr);
u16 VEML6070_ReadValue(void);
void delay_us(int nus);
u8 IIC_Write_Byte(u8 dat);


#define IIC_SCL_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3,GPIO_PIN_SET)//SCL
#define IIC_SCL_L    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3,GPIO_PIN_RESET)//SCL
#define IIC_SDA_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,GPIO_PIN_SET) //SDA	 
#define IIC_SDA_L    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,GPIO_PIN_RESET) //SDA	 
#define READ_SDA   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)  //输入SDA 

//IIC所有操作函数
void SDA_IN(void);
void SDA_OUT(void);
void I2C_Start(void);				//发送IIC开始信号
void I2C_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 I2C_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void I2C_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif

