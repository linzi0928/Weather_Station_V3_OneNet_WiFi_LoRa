#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "iwdg.h"
extern void Fill_RAM(unsigned char Data);
extern void OLED_WR_Byte(unsigned char dat,unsigned char cmd);
extern void Asc5_8(unsigned char x,unsigned char y,unsigned char ch[]);
extern void print_pc(unsigned char inputdata[],int length);			
extern void Asc8_16(unsigned char x,unsigned char y,unsigned char ch[]);
extern uint16_t step;
extern int EncoderKey_Scan(void);
extern void Draw_Rectangle(unsigned char Data, uint16_t a, uint16_t b, unsigned char c, unsigned char d, unsigned char e);
extern void HZ12_12( unsigned char x, unsigned char y, unsigned char num);
extern void Show_HZ12_12(unsigned char  x,unsigned char  y, unsigned char  d,unsigned char num1,unsigned char num2);
extern void Asc12_24(unsigned char x,unsigned char y,unsigned char ch[]);
extern uint8_t BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
extern uint8_t BSP_W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
extern uint8_t BSP_W25Qx_Erase_Block(uint32_t Address);
extern uint8_t BSP_W25Qx_Erase_Chip(void);
extern void Main_UI_Refresh(void);
extern float buffer2float(unsigned char * inp);
extern RTC_DateTypeDef sdatestructure;
extern RTC_TimeTypeDef stimestructure;
extern int curr_addr_flash;
extern float int2float_mem(int *pf);
extern unsigned char default_setting[];
extern int charge_com_flag;
extern void voltage_get(void);
extern double read_adc;
float Get_Free_Space(int addr);
void Time_Set_Menu(void);
void Data_Query(void);
void Package_Decoding_Dis(uint8_t input[]);
void Query_Time_Rec_Disp(int addr);
void Data_Del(void);
void Charge_Full(void);
typedef struct {
	float temp;
	float humi;
	float pressure;
	float gas_r;
	uint32_t dust_25;
	uint16_t uvv;
	uint16_t co2_eq_ppm;
	uint16_t tvoc_ppb;
}env_dis;
env_dis env_display;
