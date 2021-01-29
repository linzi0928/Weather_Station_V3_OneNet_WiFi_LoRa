//VEML6070紫外线传感器底层驱动，来自互联网
#include "VEML6070.h"
#include "stm32l0xx_hal_gpio.h"
void delay_us(int nus)
{		

	u8 i,j;
	for(i=0;i<nus;i++)
	  for(j=0;j<50;j++);
}
void VEML6070_WriteCmd(u8 addr, u8 cmd_data)
{
//   I2C_Start();
// 	
// 	IIC_Send_Byte(addr);	   //发送写命令
// 	IIC_Wait_Ack();
//   IIC_Send_Byte(cmd_data);	   //发送写命令 
// 	IIC_Wait_Ack();
//   I2C_Stop();//产生一个停止条件	
		  I2C_Start();  //起始信号
	if(!(IIC_Write_Byte(addr)))	//发送写命令并检查应答位
	{
		if(!(IIC_Write_Byte(cmd_data)))
		{
			;;
		}
			//发送数据
		//return RESET;
	}
	I2C_Stop();
//	return RESET;
}
u8 I2C_Write_Byte(u8 dat)
{
	u8 i;
	u8 iic_ack=0;	

	SDA_OUT();	

	for(i = 0;i < 8;i++)
	{
		if(dat & 0x80)	
		IIC_SDA_H;
		else	
		IIC_SDA_L;
			
		delay_us(2);
		IIC_SCL_H;
	    delay_us(2);
		IIC_SCL_L;
		dat<<=1;
	}

	IIC_SDA_H;	//释放数据线

	SDA_IN();	//设置成输入

	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	
	iic_ack |= READ_SDA;	//读入应答位
	IIC_SCL_L;
	return iic_ack;	//返回应答信号
}
	u8 VEML6070_ReadData(u8 addr)
	{
 		u8 temp;
// 		
// 		I2C_Start();
// 		
// 		IIC_Send_Byte(addr);	   //发送写命令
// 		IIC_Wait_Ack();
// 		
// 		temp=IIC_Read_Byte(1);		   
// 		
// 		I2C_Stop();//产生一个停止条件	
// 		
		I2C_Start();
		if(!(I2C_Write_Byte(addr)))	//发送写命令并检查应答位
		{
			temp=I2C_Read_Byte(1);
		}
		I2C_Ack();
		//IIC_Write_Byte(addr);	   //发送写命令		
				   
		
		I2C_Stop();//产生一个停止条件	
		
		return temp;
	}

void VEML6070_Init(void)
{ 
	VEML6070_ReadData(VEML6070_ARA);
	VEML6070_WriteCmd(VEML6070_SLAVE_ADDRESS,VEML6070_RESET);
	VEML6070_ReadData(VEML6070_ARA);
	VEML6070_WriteCmd(VEML6070_SLAVE_ADDRESS,VEML6070_SET_VALUE);
	
}

u16 veml6070_val;

u16 VEML6070_ReadValue(void)
{
	u8  value_h=0;
	u8  value_l=0;
	
  VEML6070_ReadData(VEML6070_ARA);
	value_h = VEML6070_ReadData(VEML6070_READ_VALUE2);
	
	VEML6070_ReadData(VEML6070_ARA);
	value_l = VEML6070_ReadData(VEML6070_READ_VALUE1);
	veml6070_val = (value_h<<8) + value_l;
	
	VEML6070_ReadData(VEML6070_ARA);
	VEML6070_WriteCmd(VEML6070_SLAVE_ADDRESS,VEML6070_SET_VALUE);
	
	return veml6070_val;
}






//////////////////以下为IIC底层驱动//////////////////


void SDA_IN()  {
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void SDA_OUT() {
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
//产生IIC起始信号
void I2C_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void I2C_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_H; 
	IIC_SDA_H;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA_H;delay_us(1);	   
	IIC_SCL_H;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	IIC_SCL_L;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void I2C_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_H;
		else
			IIC_SDA_L;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL_H;
		delay_us(2); 
		IIC_SCL_L;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL_L; 
        delay_us(2);
		IIC_SCL_H;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        I2C_Ack(); //发送ACK   
    return receive;
}
u8 IIC_Write_Byte(u8 dat)
{
	u8 i;
	u8 iic_ack=0;	

	SDA_OUT();	

	for(i = 0;i < 8;i++)
	{
		if(dat & 0x80)	
		IIC_SDA_H;
		else	
		IIC_SDA_L;
			
		delay_us(2);
		IIC_SCL_H;
	    delay_us(2);
		IIC_SCL_L;
		dat<<=1;
	}

	IIC_SDA_H;	//释放数据线

	SDA_IN();	//设置成输入

	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	
	iic_ack |= READ_SDA;	//读入应答位
	IIC_SCL_L;
	return iic_ack;	//返回应答信号
}
