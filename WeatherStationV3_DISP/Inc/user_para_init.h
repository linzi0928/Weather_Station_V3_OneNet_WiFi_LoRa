#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
unsigned char px[32];
#include "platform.h"
#include "radio.h"
extern tRadioDriver *Radio;
#define BUFFER_SIZE     32
static uint16_t BufferSize = BUFFER_SIZE;
tRadioDriver *Radio = NULL;
uint32_t user_offset=27000000;//410+27=437MHz
int i=0;
extern void OLED_Init(void);
extern void Show_HZ12_12(unsigned char  x,unsigned char  y, unsigned char  d,unsigned char num1,unsigned char num2);
extern void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e);
extern void OLED_WR_Byte(unsigned char dat,unsigned char  cmd);
extern void Asc6_12(unsigned char x,unsigned char y,unsigned char ch[]);
extern void Asc5_8(unsigned char x,unsigned char y,unsigned char ch[]);
extern void Fill_RAM(unsigned char Data);
extern void HZ24_24( unsigned char x, unsigned char y, unsigned char num);
extern void Asc12_24(unsigned char x,unsigned char y,unsigned char ch[]);
extern int DATA_upload(unsigned char* package_up);
extern uint8_t BSP_W25Qx_Init(void);
extern uint8_t BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
extern uint8_t BSP_W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
extern uint8_t BSP_W25Qx_Erase_Block(uint32_t Address);
extern uint8_t BSP_W25Qx_Erase_Chip(void);
extern int HTTP_Time_Get(unsigned char* package_time);
extern unsigned char ESP8266_Init(void);
extern uint8_t TempChar,TempChar2;
extern int upload_user(void);
extern void Enter_Menu_UI(void);
extern int EncoderKey_Scan(void);
extern uint32_t USART3_RX_STA;
extern double SX1276LoRaReadRssi( void );
typedef struct {
	float temp;
	float humi;
	float pressure;
	float gas_r;
	uint32_t dust_25;
	uint16_t uvv;
	uint16_t co2_eq_ppm;
	uint16_t tvoc_ppb;
	float batval;
}env_send;
env_send env_send_com;
int year=0,mon=0,day=0,hour=0,min=0,sec=0;//本地时间
int Rssi1278=-100;
void voltage_get(void);
void Main_UI_Refresh(void);
void charge_check(void);
void Save_Local_Data(void);
void HTTP_Time_Convert(char HTTP_IN[]);
void Package_Decoding_Com(uint8_t input[]);
void Refresh_Addr(void);
int ahextoi( const char* hex_str);
int moncmp(char* in1,char* in2);
float buffer2float(unsigned char * inp);
void Package_Decoding(void);
double read_adc=0;
float temp=22.22,humi=88.88,pressure=100000,gas_r=1111,batval=9.9;
unsigned char voc_level;
uint32_t dust_25=1111;
uint16_t uvv=2222,tvoc_ppb=333, co2_eq_ppm=555;
int flag_40=0,flag_395=0,charge_flag=1,ixx=0;
void load_config(void);
int data_check(void);
void print_pc(unsigned char inputdata[],int length);
void Read_recent_rec(void);
void time2hex(unsigned char *output);
void hex2time(unsigned char *output);
void COM_Handler(void);
int charge_com_flag=0;
int ers_f_flag=0;
unsigned char USART3_RX_BUF[128];
unsigned char Save_Package[64];
unsigned char pack_time[40];
