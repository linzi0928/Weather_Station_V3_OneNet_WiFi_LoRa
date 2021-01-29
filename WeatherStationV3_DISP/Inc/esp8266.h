#ifndef esp8266_h_
#define esp8266_h_
#include "iwdg.h"
unsigned char ESP8266_Init(void);
int DATA_upload(unsigned char* package_up);
void ysm(unsigned int ms);
void ESP_RESET(void);
extern void HTTP_Time_Convert(char HTTP_IN[]);
extern unsigned char pack_time[40];
extern void Fill_RAM(unsigned char Data);
extern void OLED_WR_Byte(unsigned char dat,unsigned char cmd);
extern void Asc5_8(unsigned char x,unsigned char y,unsigned char ch[]);
extern void print_pc(unsigned char inputdata[],int length);			
#endif
