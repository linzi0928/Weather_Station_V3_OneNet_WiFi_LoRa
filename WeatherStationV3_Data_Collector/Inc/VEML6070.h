#ifndef _VEML6070_H
#define _VEML6070_H
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#define u16 uint16_t
#define u8 uint8_t

#define VEML6070_SLAVE_ADDRESS    0x70
#define VEML6070_READ_VALUE1      0x71   //���������ݵ�8λ
#define VEML6070_READ_VALUE2      0x73   //���������ݸ�8λ

#define VEML6070_SET_VALUE        0x22   //bit7-6 0;bit5 ACK;bit4 THD;bit3 IT1;bit2 IT0;bit1 1;bit0 SD

#define VEML6070_SLEEP            0x03   //˯�ߣ����͹���

#define VEML6070_RESET            0x06   //��ʼ��ʱд��IIC������

#define VEML6070_ARA              0x18   //��Ӧ��ַ

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
#define READ_SDA   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)  //����SDA 

//IIC���в�������
void SDA_IN(void);
void SDA_OUT(void);
void I2C_Start(void);				//����IIC��ʼ�ź�
void I2C_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 I2C_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void I2C_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif

