/*************************************************************************************************************
舸轮综合电子工作室(GIE工作室)        出品
Gearing Interated Electronics Studio

气象站3.0 WiFi驱动

本文件是Wifi联网自动校时、上传与处理的相关函数


关注贴吧ID：tnt34 获得最新工作成果
关注B站UP主：GIE工作室 获得更多视频资源

2018-7-31第一版
版权所有 禁止用于任何商业用途！
***********************************************************************************************************/

#include "esp8266.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <usart.h>

extern unsigned char Wifi_SSID[];
extern unsigned char Wifi_password[];
extern unsigned char API_Base_addr[];
extern unsigned char CM_Onenet_devID[24];//默认设备ID
extern unsigned char CM_Onenet_api_key[40];
extern float temp,humi,pressure,gas_r,batval;
extern unsigned char voc_level;
extern uint32_t dust_25;
extern uint16_t uvv,tvoc_ppb, co2_eq_ppm;
#define u8 unsigned char
	
const unsigned char post_P1[]="POST /devices/";
	//		unsigned char post_P2[24];//657283663
const unsigned char post_P3[]="/datapoints?type=5 HTTP/1.1\r\n";
const unsigned char post_P4[]="api-key:";
		//	unsigned char post_P5[40];//SkVLFtTMpEFf=xG581jwowTZ9q0=
const unsigned char post_P6[]="Host:api.heclouds.com\r\n";
const unsigned char post_P7[]="Content-Length:";
			unsigned char post_P8[2];//85
const unsigned char post_P9[]="\r\n\r\n,;";
			unsigned char post_P10[100];//temp,28.78;humi,78.87;press,824506;eCO2,400;TVOC,15;PM2.5,1920;UV,135;GR,987;bat,3500
int calc_length(unsigned char * inp);
u8 USART1_RX_BUF[512];
uint32_t USART1_RX_STA;
int upload_user(void)
{
	if(temp==22.22&&humi==88.88&&pressure==100000&&gas_r==1111&&batval==9.9)//避免上传默认无意义数据
		return 1;
	int ret;
			Asc5_8(0,8,"Data Correct,Uploading...");
			print_pc("Data Correct,Uploading...\r\n",sizeof("Data Correct,Uploading...\r\n"));
			ret=DATA_upload(pack_time);
			if(ret==1)
			{
				print_pc("TCP Conn Failed Retrying...\r\n",sizeof("TCP Conn Failed Retrying...\r\n"));
				Asc5_8(0,16,"TCP Conn Failed Retrying...");
				ret=DATA_upload(pack_time);
				if(ret==1)
				{
					print_pc("ReConnect to Router\r\n",sizeof("ReConnect to Router\r\n"));
					Asc5_8(0,24,"ReConnect to Router");	
					printf("+++");					
					if(ESP8266_Init())
					{
						print_pc("ReConnect Failed,check wifi\r\n",sizeof("ReConnect Failed,check wifi\r\n"));
						Asc5_8(0,32,"ReConnect failed");		
					}
					else
					{
						ret=DATA_upload(pack_time);
						{
						print_pc("Retry Failed,Save data locally.\r\n",sizeof("Retry Failed,Save data locally.\r\n"));
						Asc5_8(0,32,"Retry Failed,Save data locally.");		
						}							
					}						
					HAL_Delay(1000);
				}
			}
			if(ret==3)
			{
				print_pc("Port Open Failed Retrying...\r\n",sizeof("Port Open Failed Retrying...\r\n"));
				Asc5_8(0,16,"Port Open Failed Retrying...");
				ret=DATA_upload(pack_time);
				if(ret)
				{
					print_pc("ReConnect to Router\r\n",sizeof("ReConnect to Router\r\n"));
					Asc5_8(0,24,"ReConnect to Router");		
					printf("+++");
					if(ESP8266_Init())
					{
						print_pc("ReConnect Failed,check wifi\r\n",sizeof("ReConnect Failed,check wifi\r\n"));
						Asc5_8(0,32,"ReConnect failed");		
					}
					else
					{
						ret=DATA_upload(pack_time);
						if(ret)
						{
						print_pc("Retry Failed,Save data locally.\r\n",sizeof("Retry Failed,Save data locally.\r\n"));
						Asc5_8(0,24,"Retry Failed,Save data locally.");		
						}							
					}
				HAL_Delay(1000);
				}
			}
			if(ret==2)
			{
				print_pc("Empty Reply from server\r\n",sizeof("Empty Reply from server\r\n"));
				Asc5_8(0,16,"Empty Reply from server...");	
				ret=DATA_upload(pack_time);
				if(ret)
				{
					print_pc("ReConnect to Router\r\n",sizeof("ReConnect to Router\r\n"));
					Asc5_8(0,24,"ReConnect to Router");		
					if(ESP8266_Init())
					{
						print_pc("ReConnect Failed,check wifi\r\n",sizeof("ReConnect Failed,check wifi\r\n"));
						Asc5_8(0,32,"ReConnect failed");		
					}
					else
					{
						ret=DATA_upload(pack_time);
					}
				}
			}
			if(ret==0)
			{
				print_pc("Upload Success\r\n",sizeof("Upload Success\r\n"));
				Asc5_8(0,24,"Upload Success\r\n");	
				Asc5_8(0,32,pack_time);	
				print_pc(pack_time,sizeof(pack_time));print_pc("\r\n",2);
				HTTP_Time_Convert((char *)pack_time);//由最新网络时间校准本地时间
				HAL_Delay(1000);
				Fill_RAM(0x00);
				OLED_WR_Byte(0xAF,0); //	Display On
			}
			Fill_RAM(0x00);
			OLED_WR_Byte(0xAF,0); //	Display On
			return ret;
}



u8* ESP8266_CheckCmd(char* str)// Chack string
{
  char *strx=0;

   // USART1_RX_BUF[USART1_RX_STA&0X3FFF]=0;
    strx=strstr((const char*)USART1_RX_BUF,(const char*)str);
   
  return (u8*)strx;
}

u8 ESP8266_SendCmd(char *cmd,char *ack,uint16_t waittime)// cmd =1 
{
  u8 res=0;
  printf("%s\r\n",cmd);
  if(ack&&waittime)
  {
    while(--waittime)
    {
      ysm(10);
        if(NULL != ESP8266_CheckCmd(ack))
        {	
          res=0;
//          OLED_Printf("%s....%s\n",cmd,ack);
          break;
        } 
    }
    if(waittime==0) 
		{
			res=1;
		}
  }
//	USART1_RX_STA=0;
//	memset(USART1_RX_BUF,0x00,sizeof(USART1_RX_BUF));
  return res;
}



u8 ESP8266_Init(void)
{
  u8 state=0;
	unsigned char Send_buffer[64];
	int waittime=500;
  USART1_RX_STA = 0;
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
  state = ESP8266_SendCmd("AT+CWMODE=3","OK",20);
	ESP_RESET();
	HAL_IWDG_Refresh(&hiwdg);
	while(--waittime)
   {
    ysm(10);
		if(NULL != ESP8266_CheckCmd("ready"))
       {
         break;
       }
   }
	 HAL_IWDG_Refresh(&hiwdg);
USART1_RX_STA = 0;
memset(Send_buffer,0x00,sizeof(Send_buffer));
sprintf((char*)Send_buffer,"AT+CWJAP=\"%s\",\"%s\"",Wifi_SSID,Wifi_password);
 state = ESP8266_SendCmd((char*)Send_buffer,"OK",1000);  // Connect router
 if(state) return 1;
	 HAL_IWDG_Refresh(&hiwdg);
  ESP8266_SendCmd("AT+CIPMUX=0","OK",300);
return state;
}


int month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
uint32_t datetemp;
int waittime=500;
int DATA_upload(unsigned char* package_time)
{
  u8 packetBuffer[128];
	char *strx1=0,*strx2=0;
  sprintf((char*)packetBuffer,"AT+CIPSTART=\"TCP\",\"%s\",80",API_Base_addr);
	HAL_IWDG_Refresh(&hiwdg);
  if(ESP8266_SendCmd((char*)packetBuffer,"OK",1000))
	{
		if(NULL !=ESP8266_CheckCmd("Bad Request"))
			goto TRANS_START;
		if(NULL ==ESP8266_CheckCmd("ALREADY CONNECTED"))
			return 1;//无法建立TCP连接
	}		
	USART1_RX_STA = 0;
	HAL_IWDG_Refresh(&hiwdg);
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
	ESP8266_SendCmd("AT+CIPMODE=1","OK",100);
  USART1_RX_STA = 0;
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
  ysm(100);
  memset(packetBuffer,0,sizeof(packetBuffer));
	HAL_IWDG_Refresh(&hiwdg);
  if(ESP8266_SendCmd("AT+CIPSEND","OK",500))
	{
		if(NULL !=ESP8266_CheckCmd("Bad Request"))
			goto TRANS_START;
		if(ESP8266_SendCmd("AT+CIPSEND","OK",500))
					return 3;//端口打开错误
	}
	HAL_IWDG_Refresh(&hiwdg);
	USART1_RX_STA = 0;
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
	TRANS_START:
	sprintf((char *)post_P10,"temp,%.2f;humi,%.2f;press,%.0f;eCO2,%d;TVOC,%d;PM2.5,%d;UV,%d;GR,%.0f;bat,%.2f",temp,humi,pressure,co2_eq_ppm,tvoc_ppb,dust_25,uvv,gas_r,batval);
	printf((char *)post_P1);printf("%s",CM_Onenet_devID);
	printf((char *)post_P3);
	printf((char *)post_P4);printf("%s\r\n",CM_Onenet_api_key);
	printf((char *)post_P6);printf((char *)post_P7);
	sprintf((char *)post_P8,"%d",calc_length(post_P10)+2);
	printf((char *)post_P8);
	printf((char *)post_P9);
	printf((char *)post_P10);
	HAL_Delay(100);
	while(--waittime)
   {
    ysm(10);
		if(NULL != ESP8266_CheckCmd("HTTP/1.1 200"))
       {
         break;
       }
   }
	 HAL_Delay(100);
	 HAL_IWDG_Refresh(&hiwdg);
	memset(package_time,0x00,40);
	if(USART1_RX_BUF[0]!=0)
	{
		strx1=strstr((const char*)USART1_RX_BUF,"Date: ");
		strx2=strstr((const char*)USART1_RX_BUF," GMT");
		memcpy(package_time,strx1,strx2-strx1);
	}
	if(package_time[0]!=0)
		return 0;
	else
		return 2;
}
int HTTP_Time_Get(unsigned char* package_time)
{
  u8 packetBuffer[128];
	char *strx1=0;
	USART1_RX_STA = 0;
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
  sprintf((char*)packetBuffer,"AT+CIPSTART=\"TCP\",\"%s\",80",API_Base_addr);
  if(ESP8266_SendCmd((char*)packetBuffer,"OK",1000))
	{
		if(NULL !=ESP8266_CheckCmd("Bad Request"))
			goto TRANS_START2;
		if(NULL ==ESP8266_CheckCmd("ALREADY CONNECTED"))
			return 1;//无法建立TCP连接
	}		
	HAL_IWDG_Refresh(&hiwdg);
	USART1_RX_STA = 0;
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
	ESP8266_SendCmd("AT+CIPMODE=1","OK",100);
  USART1_RX_STA = 0;
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
  ysm(100);
  memset(packetBuffer,0,sizeof(packetBuffer));
  if(ESP8266_SendCmd("AT+CIPSEND","OK",500))
	{
		if(NULL !=ESP8266_CheckCmd("Bad Request"))
			goto TRANS_START2;
		if(ESP8266_SendCmd("AT+CIPSEND","OK",500))
					return 3;//端口打开错误
	}
	HAL_IWDG_Refresh(&hiwdg);
	USART1_RX_STA = 0;
  memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
//	for(i=0;i<24;i++)
//	{
//		if(CM_Onenet_devID[i]==0xfe)
//			break;
//		else
//		post_P2[i]=CM_Onenet_devID[i];
//	}
	TRANS_START2:
	printf((char *)post_P1);printf("%s",CM_Onenet_devID);
	printf((char *)post_P3);
	printf((char *)post_P4);printf("%s\r\n",CM_Onenet_api_key);
	printf((char *)post_P6);printf((char *)post_P7);
	sprintf((char *)post_P8,"%d",1);
	printf((char *)post_P8);
	printf((char *)post_P9);
	printf(";");
	HAL_Delay(100);
	while(--waittime)
   {
    ysm(10);
		if(NULL != ESP8266_CheckCmd("HTTP"))
       {
         break;
       }
   }
	 HAL_Delay(100);
	memset(package_time,0x00,40);
	 HAL_IWDG_Refresh(&hiwdg);
	if(USART1_RX_BUF[0]!=0)
	{
		strx1=strstr((const char*)USART1_RX_BUF,"Date: ");
		memcpy(package_time,strx1,31);
	}
	if(package_time[0]!=0)
		return 0;
	else
		return 2;
}
int calc_length(unsigned char * inp)
{
	int i;
	for (i=0;;i++)
	{
		if(inp[i]==0)
			return i;
	}
}
void ysm(unsigned int ms)
{
HAL_Delay(ms);
}
void ESP_RESET(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
}










