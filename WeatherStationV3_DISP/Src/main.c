/* USER CODE BEGIN Header */
/*************************************************************************************************************
舸轮综合电子工作室(GIE工作室)        出品
Gearing Interated Electronics Studio

气象站3.0 边缘数据服务器（室内中继、上传与存储）

本文件是主函数和数据存储、主界面刷新等

主要功能：通过SX1278接收437MHz频段来自室外采集端的数据包，将数据包解码、打时间戳后存储到本地Flash中，再上传至
中移Onenet平台，WiFi的相关参数可通过AT命令修改，数据删除可通过内部UI实现

关注贴吧ID：tnt34 获得最新工作成果
关注B站UP主：GIE工作室 获得更多视频资源

2020-12-21第一版
版权所有 禁止用于任何商业用途！
***********************************************************************************************************/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sx12xxEiger.h"
#include "string.h"
#include "stdlib.h"
#include "user_para_init.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
unsigned char CM_Onenet_devID[24]="657283663";//默认设备ID
unsigned char CM_Onenet_api_key[40]="SkVLFtTMpEFf=xG581jwowTZ9q0=";//默认API-Key
unsigned char API_Base_addr[]="183.230.40.33";//默认接入地址
unsigned char Wifi_SSID[21]="GIE_Studio_NetCenter";//默认Wifi名
unsigned char Wifi_password[20]="7104021730114";//默认Wifi密码
int curr_addr_flash=0;//当前最新数据所在地址尾
//flash(W25Q128)的第0 block用于存放配置信息：
unsigned char default_setting[]={
	0xaf,//0-0  0xaf起始位
	0x00,0x77,0xEF,0x4F,0x53,//1-5  最新数据时间戳
	0x01,0x00,0x00,//6-8  当前最新数据所在地址，去除第0 block的配置信息后地址从0x10000写起
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//9-30 Wifi名,以0x00结尾
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//31-52 Wifi密码，以0x00结尾
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//53-67 中移OneNet平台设备ID，以0x00结尾
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//68-108 中移OneNet平台设备接入API-Key，以0x00结尾
	0x01,0xB5,//109-110 LoRa默认通信频率（437MHz）
	0x00,0x01,0x00,//111-113  当前已上传最新数据所在地址
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
extern void OLED_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, &TempChar, 1);
	HAL_UART_Receive_IT(&huart3, &TempChar2, 1);//打开串口中断
	OLED_Init();
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);//打开ADC输入
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);//关闭充电
	charge_flag=1;
	BoardInit( );
	Radio = RadioDriverInit( );
	Radio->Init( );
	Radio->StartRx( );
	BSP_W25Qx_Init();
	load_config();
	Show_HZ12_12(90,0,0,0,2);Asc6_12(132,0,"V3.0");//气象站V3
	Draw_Rectangle(0xaa,1,0,255,13,14);
	print_pc("GIE Weather Station Edge Data Server\r\n",sizeof("GIE Weather Station Edge Data Server\r\n"));
	Asc6_12(38,52,"GIE Studio 2020-12-15");
	Asc5_8(0,20,"WiFi Connecting...");
	print_pc("WiFi Connecting...\r\n",sizeof("WiFi Connecting...\r\n"));
	unsigned char temp_bufferw[30];
	sprintf((char *)temp_bufferw,"SSID:%s",Wifi_SSID);
	print_pc(temp_bufferw,sizeof(temp_bufferw)-1);
	Asc5_8(0,29,temp_bufferw);
	HAL_IWDG_Refresh(&hiwdg);
	if(ESP8266_Init())//初始化ESP8266，并连接wifi
	{
		Asc5_8(0,39,"WiFi Connect error, save data local");
		print_pc("\r\nWiFi Connect error, save data local\r\n",sizeof("\r\nWiFi Connect error, save data local\r\n"));
	}
	else
	{
		Asc5_8(0,39,"WiFi Connected");
		print_pc("\r\nWiFi Connected\r\n",sizeof("\r\nWiFi Connected\r\n"));
	}
	unsigned char vol_dis[]="0.00V";
	Show_HZ12_12(150,20,0,6,6);Show_HZ12_12(166,20,0,8,8);//电压：
	while(i<30)
		{
			voltage_get();
			sprintf((char*)vol_dis,"%.2lfV",read_adc);
			Asc6_12(188,20,vol_dis);
			HAL_Delay(100);
			i++;
		}
		i=0;
	Fill_RAM(0x00);
	OLED_WR_Byte(0xAF,0); //	Display On
  HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	if(sdatestructure.Year==0)//日期未设置，联网对时
	{
		HAL_IWDG_Refresh(&hiwdg);
		Asc5_8(0,0,"Local Time not set");
		print_pc("Local Time not set\r\n",sizeof("Local Time not set\r\n"));
		if(!HTTP_Time_Get(pack_time))
		{
			Asc5_8(0,8,"Net Time Get");
			print_pc("Net Time Get\r\n",sizeof("Net Time Get\r\n"));
			print_pc(pack_time,31);print_pc("\r\n",2);
			HTTP_Time_Convert((char *)pack_time);
			HAL_Delay(1000);
			Fill_RAM(0x00);
			OLED_WR_Byte(0xAF,0); //	Display On
		}
		else
		{
			Asc5_8(0,8,"Net Time Get Failed");
			print_pc("Net Time Get Failed\r\n",sizeof("Net Time Get Failed\r\n"));
			HAL_Delay(1000);
			Fill_RAM(0x00);
			OLED_WR_Byte(0xAF,0); //	Display On
		}
	}
		//以上为启动界面，以下为主信息显示屏初始化
		Refresh_Addr();
		Read_recent_rec();
		Package_Decoding();
		Main_UI_Refresh();
		print_pc("System Boot Complete\r\n",sizeof("System Boot Complete\r\n"));
		print_pc("Listening at 437MHz for Weather Packages\r\n",sizeof("Listening at 437MHz for Weather Packages\r\n"));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		while( Radio->Process( ) == RF_RX_DONE)//从1278读取接收完成标志，如果这个包接收完成则进入
		{
		for(ixx=0;ixx<BUFFER_SIZE;ixx++)
		px[ixx] = 0;
		Radio->GetRxPacket( px, ( uint16_t* )&BufferSize );//依次取出缓冲区内容
		Rssi1278=(int)SX1276LoRaReadRssi();
		Radio->StartRx();
		Package_Decoding();
		if(!data_check())//检查数据是否合理
		{
				Fill_RAM(0x00);
				OLED_WR_Byte(0xAF,0); //	Display On
				Asc5_8(0,0,"Incoming Package...Analyzing...");
				//Asc5_8(0,0,"Incoming Package...Analyzing...");
				print_pc("Package Received\r\n",sizeof("Package Received\r\n"));
			
				upload_user();		//上传数据	
				Save_Local_Data();			//本地保存数据
		}
			else
		{
				print_pc("Data Invalid\r\n",sizeof("Data Invalid\r\n"));
				Read_recent_rec();
				Package_Decoding();
		}
			Main_UI_Refresh();
			memset(px,0x00,sizeof(px));
		}
		charge_check();
		COM_Handler();
		if(EncoderKey_Scan()==1)//扫描编码器进入菜单
		{
			Enter_Menu_UI();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_IWDG_Refresh(&hiwdg);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void COM_Handler(void)//AT命令响应与执行
{
	char* strx1=0,*strx2=0;
	unsigned char send_buffer[128];
	unsigned char read_buffer[64];
	int read_addr=0;//串口批量导出起始地址
	if(USART3_RX_STA&0x8000)
	{
		USART3_RX_STA=0;
		memset(send_buffer,0x00,sizeof(send_buffer));
		strx1=strstr((const char*)USART3_RX_BUF,"AT");
		if(strx1==0)
		{
			print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
			print_pc("Invalid Command\r\n",sizeof("Invalid Command\r\n"));
			memset(USART3_RX_BUF,0x00,sizeof(USART3_RX_BUF));
			return;
		}
		else
		{
			strx2=strstr((const char*)USART3_RX_BUF,"AT+");
			if(strx2==0)
			{
				print_pc("AT\r\n",sizeof("AT\r\n"));
				memset(USART3_RX_BUF,0x00,sizeof(USART3_RX_BUF));
				return;
			}
			else
			{
				strx1=strstr((const char*)USART3_RX_BUF,"EXP");
				if(strx1!=0)
				{
					if(*(strx1+3)=='?')//AT+EXP?查询当前最新数据的存储地址
					{
						print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
						sprintf((char*)send_buffer,"CURR_ADDR:%08X\r\n",curr_addr_flash);//AT+EXP=10000查询某地址之后的所有数据，16进制
						print_pc(send_buffer,20);
					}
					else if(*(strx1+3)=='=')
					{
						if(curr_addr_flash==0x10000)
						{
							print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
							print_pc("No data to export\r\n",sizeof("No data to export\r\n"));
						}
						strx1+=4;
						read_addr=ahextoi(strx1);
						if(read_addr>=0x10000&&read_addr<=0x1000000)
						{
							uint8_t time_store[6];
							read_addr=0x40*(read_addr/0x40);
							print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
							for(;read_addr<curr_addr_flash;read_addr+=0x40)
							{
								if(read_addr>=curr_addr_flash)
								{
									break;
								}
								BSP_W25Qx_Read(read_buffer,read_addr,0x40);
								Package_Decoding_Com(read_buffer);
								memcpy(time_store,read_buffer+32,6);
								sprintf((char *)send_buffer,"%.1fC %.1f%% %.0fPa %dug %duW %dppm %dppb 20%02d-%02d-%02d %02d:%02d:%02d %.2fV",
									env_send_com.temp,env_send_com.humi,env_send_com.pressure,env_send_com.dust_25,env_send_com.uvv,
									env_send_com.co2_eq_ppm,env_send_com.tvoc_ppb,time_store[0],time_store[1],time_store[2],time_store[3],time_store[4],time_store[5],env_send_com.batval);
								print_pc(send_buffer,sizeof(send_buffer));print_pc("\r\n",2);
								memset(send_buffer,0x00,sizeof(send_buffer));
							}
						}
						else
						{
							print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
							print_pc("Invalid Address",sizeof("Invalid Address"));
						}
					}
					
				}
				else
				{
					strx1=strstr((const char*)USART3_RX_BUF,"ERS");//AT+ERS删除所有存储的气象信息
					if(strx1!=0)
					{
						if(ers_f_flag==0)
						{
							print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
							print_pc("Type again to comfirm the erase of whole chip\r\n",sizeof("Type again to comfirm the erase of whole chip\r\n"));
							ers_f_flag=1;
						}
						else
						{
							print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
								print_pc("Working...\r\n",sizeof("Working...\r\n"));
								BSP_W25Qx_Erase_Chip();
								curr_addr_flash=0x10000;
								default_setting[6]=curr_addr_flash>>16;
								default_setting[7]=(curr_addr_flash&0xff00)>>8;
								default_setting[8]=(curr_addr_flash&0x00ff);
								BSP_W25Qx_Write(default_setting, 0x00,114);
								print_pc("Erase done!\r\n",sizeof("Erase done!\r\n"));
							ers_f_flag=0;
						}
					}
					else
					{
						strx1=strstr((const char*)USART3_RX_BUF,"CWJAP=");//AT+CWJAP=HUAWEI Mate9,7104021730114设置Wifi的SSID和密码
						if(strx1!=0)
						{
							strx1+=6;
							strx2=strstr((const char*)USART3_RX_BUF,",");
							if(strx2==0)
							{
								print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
								print_pc("Invalid Command\r\n",sizeof("Invalid Command\r\n"));
								memset(USART3_RX_BUF,0x00,sizeof(USART3_RX_BUF));
								return;
							}
							else
							{
								memset(Wifi_SSID,0x00,sizeof(Wifi_SSID));
								memcpy(Wifi_SSID,strx1,strx2-strx1);
								strx2+=1;
								strx2=strstr((const char*)USART3_RX_BUF,",");
								memset(Wifi_password,0x00,sizeof(Wifi_password));
								memcpy(Wifi_password,strx2+1,20);
								BSP_W25Qx_Erase_Block(0x00);
								memcpy(default_setting+9,Wifi_SSID,sizeof(Wifi_SSID));
								default_setting[9+sizeof(Wifi_SSID)-1]=0x00;
								memcpy(default_setting+31,Wifi_password,sizeof(Wifi_password));
								default_setting[31+sizeof(Wifi_password)-1]=0x00;
								BSP_W25Qx_Write(default_setting, 0x00,sizeof(default_setting));
								sprintf((char *)send_buffer,"New WiFi Set OK\r\nSSID:%s Pass:%s\r\nReboot to take effect\r\n",Wifi_SSID,Wifi_password);
								print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
								print_pc(send_buffer,sizeof(send_buffer));
							}
						}
						else
						{
							strx1=strstr((const char*)USART3_RX_BUF,"RST");	//AT+RST重启
							if(strx1!=0)
							{
								print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
								print_pc("System Reset\r\n",sizeof("System Reset\r\n"));
								__set_FAULTMASK(1); NVIC_SystemReset();
							}
							else
							{
								strx1=strstr((const char*)USART3_RX_BUF,"CMSET");	//AT+CMSET=657283663,SkVLFtTMpEFf=xG581jwowTZ9q0=设置设备ID和API-Key
								if(strx1!=0)
									{
										strx2=0;
										strx1=0;
										strx1=strstr((const char*)USART3_RX_BUF,"?");
										if(strx1!=0)
										{
											print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
											sprintf((char *)send_buffer,"DevID:%s API-Key:%s\r\n",CM_Onenet_devID,CM_Onenet_api_key);
											print_pc(send_buffer,sizeof(send_buffer));
											memset(USART3_RX_BUF,0x00,sizeof(USART3_RX_BUF));
											return;
										}
										strx1=strstr((const char*)USART3_RX_BUF,"=");
										strx1+=1;
										strx2=strstr((const char*)strx1,",");
										if(strx2!=0)
										{
											memset(CM_Onenet_devID,0x00,sizeof(CM_Onenet_devID));
											memcpy(CM_Onenet_devID,strx1,strx2-strx1);
											strx1=strx2+1;
											memset(CM_Onenet_api_key,0x00,sizeof(CM_Onenet_api_key));
											memcpy(CM_Onenet_api_key,strx1,40);
											BSP_W25Qx_Erase_Block(0x00);
											memcpy(default_setting+53,CM_Onenet_devID,24);
											default_setting[53+sizeof(CM_Onenet_devID)-1]=0x00;		
											memcpy(default_setting+68,CM_Onenet_api_key,40);
											default_setting[68+sizeof(CM_Onenet_api_key)-1]=0x00;	
											BSP_W25Qx_Write(default_setting, 0x00,sizeof(default_setting));
											print_pc("New Dev ID and API Key Set OK,Reboot to take effect\r\n",sizeof("New Dev ID and API Key Set OK,Reboot to take effect\r\n"));
											sprintf((char *)send_buffer,"DevID:%s API-Key:%s\r\n",CM_Onenet_devID,CM_Onenet_api_key);
											print_pc(USART3_RX_BUF,sizeof(USART3_RX_BUF));print_pc("\r\n",2);
											print_pc(send_buffer,sizeof(send_buffer));
										}
										else
										{
											print_pc("Invalid DevID\r\n",sizeof("Invalid DevID\r\n"));
										}
									}
								else
								{
									print_pc("Invalid Command\r\n",sizeof("Invalid Command\r\n"));
								}
							}
						}
					}
				}
			}
		}
		memset(USART3_RX_BUF,0x00,sizeof(USART3_RX_BUF));
	}
}
void Package_Decoding(void)//解码数据包
{	
	if(px[0]==0&&px[1]==0&&px[2]==0&&px[3]==0&&px[4]==0&&px[5]==0&&px[6]==0&&px[7]==0&&px[8]==0)
		return;
		temp=buffer2float(px);
		humi=buffer2float(px+4);
		pressure=buffer2float(px+8);
		gas_r=buffer2float(px+12);
		voc_level=px[16];
		dust_25=px[17]<<24 | px[18]<<16 | px[19]<<8 | px[20]; 
		uvv=px[21]<<8 | px[22];
		co2_eq_ppm=px[23]<<8 | px[24];
		tvoc_ppb=px[25]<<8 | px[26];
		batval=buffer2float(px+27);
}
void Package_Decoding_Com(uint8_t input[])//串口导出时的数据包解码
{	
		env_send_com.temp=buffer2float(input);
		env_send_com.humi=buffer2float(input+4);
		env_send_com.pressure=buffer2float(input+8);
		env_send_com.gas_r=buffer2float(input+12);
		env_send_com.dust_25=input[17]<<24 | input[18]<<16 | input[19]<<8 | input[20]; 
		env_send_com.uvv=input[21]<<8 | input[22];
		env_send_com.co2_eq_ppm=input[23]<<8 | input[24];
		env_send_com.tvoc_ppb=input[25]<<8 | input[26];
		env_send_com.batval=buffer2float(input+27);
}
void Main_UI_Refresh(void)//主界面数据刷新
{
	unsigned char dis_buffer[32];
	if(!data_check())//检查数据是否合理
	{
		Draw_Rectangle(0xaa,1,0,255,25,26);

		
		HZ24_24(0,0,0);//温度显示区
		sprintf((char*)dis_buffer,"%02.2f",temp);
		Asc12_24(36,0,dis_buffer);
		HZ24_24(96,0,1);
		
		
		HZ24_24(136,0,2);//湿度显示区
		sprintf((char*)dis_buffer,"%02.2f%",humi);
		Asc12_24(172,0,dis_buffer);

		Show_HZ12_12(0,29,0,12,12);//气压显示区
		sprintf((char*)dis_buffer,"%6.0fPa",pressure);
		Asc6_12(13,29,dis_buffer);
		
		Draw_Rectangle(0xaa,1,82,82,26,40);
		sprintf((char*)dis_buffer,"%03dppm",co2_eq_ppm);
		Asc6_12(87,29,"CO2:");
		Asc6_12(112,29,dis_buffer);
		
		Draw_Rectangle(0xaa,1,175,175,26,40);
		sprintf((char*)dis_buffer,"%03d",tvoc_ppb);
		Asc6_12(180,29,"TVOC:");
		Asc6_12(220,29,dis_buffer);
		
		Draw_Rectangle(0xaa,1,0,255,40,40);
		
		Asc6_12(0,41,"PM2.5:");
		sprintf((char*)dis_buffer,"%04dug",dust_25);
		Asc6_12(50,41,dis_buffer);
		
		Asc6_12(100,41,"UV:");
		sprintf((char*)dis_buffer,"%04duW",uvv);
		Asc6_12(120,41,dis_buffer);
		
		Draw_Rectangle(0xaa,1,175,175,40,52);
		
		Asc6_12(180,41,"GR:");
		sprintf((char*)dis_buffer,"%.0fh",gas_r);
		Asc6_12(216,41,dis_buffer);
		
		Draw_Rectangle(0xaa,1,0,255,52,52);
		
		sprintf((char*)dis_buffer,"UPT:%02d%02d%02d %02d:%02d",year,mon,day,hour,min);
		Asc6_12(100,54,dis_buffer);
		sprintf((char*)dis_buffer,"%.2fV",batval);
		Asc6_12(0,54,dis_buffer);
		sprintf((char*)dis_buffer,"%ddb",Rssi1278);
		Asc6_12(50,54,dis_buffer);
	}
}
void voltage_get(void)//本机电压获取
{
	int time_out=0,adc_raw;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_GetValue(&hadc1);    
	while(HAL_ADC_PollForConversion(&hadc1, 0xff)!=HAL_OK)
	{
		time_out++;
		if(time_out>10000)
			return;
	}
	adc_raw=HAL_ADC_GetValue(&hadc1);
	read_adc=((float)adc_raw/65535.0)*3.3*70-2.75;//换算为电压
}
void charge_check(void)//控制充电开始与结束
{
	if(charge_com_flag==1)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		charge_flag=1;
		return;
	}
	voltage_get();
		if(read_adc>4.00&&charge_flag==1)
		{
			flag_40++;
		}
		else if(flag_40>0&&read_adc<=4.00)
		{
			flag_40--;
		}
		if(flag_40>50&&charge_flag==1)
		{
			
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);//关闭充电
			charge_flag=0;
		}
		if(read_adc<3.95&&charge_flag==0)
		{
			flag_395++;
		}
		else if(flag_395>0&&read_adc>3.95&&charge_flag==0)
		{
			flag_395--;
		}
		if(flag_395>50)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);//开启充电
			charge_flag=1;
			
		}
}
int data_check(void)//数据检查
{
	if(temp<60&&temp>-50&&humi>0&&humi<100&&pressure>50000&&pressure<110000&&batval>0&&batval<10)
		return 0;
	else
		return 1;
}
void load_config(void)//加载配置
{
	unsigned char write_buffer[256];
	BSP_W25Qx_Read(write_buffer, 0x00, 0xff);//读取配置block，仅第0page有效
	if(write_buffer[0]!=0xaf)//首次开机写入默认配置
	{
		BSP_W25Qx_Erase_Block(0x00);
		memcpy(default_setting+9,Wifi_SSID,sizeof(Wifi_SSID));
		default_setting[9+sizeof(Wifi_SSID)-1]=0x00;
		memcpy(default_setting+31,Wifi_password,sizeof(Wifi_password));
		default_setting[31+sizeof(Wifi_password)-1]=0x00;
		memcpy(default_setting+53,CM_Onenet_devID,sizeof(CM_Onenet_devID));
		default_setting[53+sizeof(CM_Onenet_devID)-1]=0x00;		
		memcpy(default_setting+68,CM_Onenet_api_key,sizeof(CM_Onenet_api_key));
		default_setting[68+sizeof(CM_Onenet_api_key)-1]=0x00;		
		curr_addr_flash=0x10000;
		BSP_W25Qx_Write(default_setting, 0x00,sizeof(default_setting));
	}
	else//非首次开机读取flash中的配置
	{
		memset(Wifi_SSID,0x00,sizeof(Wifi_SSID));
		memcpy(Wifi_SSID,write_buffer+9,sizeof(Wifi_SSID));
		memset(Wifi_password,0x00,sizeof(Wifi_password));
		memcpy(Wifi_password,write_buffer+31,sizeof(Wifi_password));
		memset(CM_Onenet_devID,0x00,sizeof(CM_Onenet_devID));
		memcpy(CM_Onenet_devID,write_buffer+53,sizeof(CM_Onenet_devID));
		memset(CM_Onenet_api_key,0x00,sizeof(CM_Onenet_api_key));
		memcpy(CM_Onenet_api_key,write_buffer+68,sizeof(CM_Onenet_api_key));
		curr_addr_flash=(write_buffer[6]<<16)|(write_buffer[7]<<8)|(write_buffer[8]);
		memcpy(default_setting,write_buffer,sizeof(default_setting));
	}
	
}
void Read_recent_rec(void)//读取flash中最新的数据
{
	unsigned char temp_data[64],time_store[6];
	if(curr_addr_flash==0x10000)
		return;
	BSP_W25Qx_Read(temp_data,curr_addr_flash-0x40,0x40);
	memcpy(px,temp_data,32);
	memcpy(time_store,temp_data+32,6);
	hex2time(time_store);
}
void Save_Local_Data(void)
{
	if(temp==22.22&&humi==88.88&&pressure==100000&&gas_r==1111&&batval==9.9)//避免保存无意义数据
		return;
	unsigned char time_store[6];
	unsigned char temp_pack[1];
	memset(Save_Package,0x00,64);
	memcpy(Save_Package,px,32);
	time2hex(time_store);
	memcpy(Save_Package+32,time_store,6);
	BSP_W25Qx_Read(temp_pack,curr_addr_flash,sizeof(temp_pack));
	while(temp_pack[0]!=0xff)
	{
		curr_addr_flash+=64;
		BSP_W25Qx_Read(temp_pack,curr_addr_flash,sizeof(temp_pack));
	}
	BSP_W25Qx_Write(Save_Package,curr_addr_flash,sizeof(Save_Package));
	BSP_W25Qx_Read(Save_Package,curr_addr_flash,sizeof(Save_Package));
	curr_addr_flash+=64;
	default_setting[6]=(curr_addr_flash&0xff0000)>>16;
	default_setting[7]=(curr_addr_flash&0xff00)>>8;
	default_setting[8]=curr_addr_flash&0x00ff;
	BSP_W25Qx_Erase_Block(0x00);
	BSP_W25Qx_Write(default_setting, 0x00,sizeof(default_setting));
}
void Refresh_Addr(void)
{
	unsigned char temp_pack[1];
	BSP_W25Qx_Read(temp_pack,curr_addr_flash,sizeof(temp_pack));
		while(temp_pack[0]!=0xff)
	{
		curr_addr_flash+=64;
		BSP_W25Qx_Read(temp_pack,curr_addr_flash,sizeof(temp_pack));
	}
	default_setting[6]=(curr_addr_flash&0xff0000)>>16;
	default_setting[7]=(curr_addr_flash&0xff00)>>8;
	default_setting[8]=curr_addr_flash&0x00ff;
	BSP_W25Qx_Erase_Block(0x00);
	BSP_W25Qx_Write(default_setting, 0x00,sizeof(default_setting));
}
void time2hex(unsigned char *output)//将时间转换为存储序列
{
		HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
    output[0]=sdatestructure.Year;
		year=2000+output[0];
    output[1]=sdatestructure.Month;
		mon=output[1];
    output[2]=sdatestructure.Date;
		day=output[2];
    output[3]=stimestructure.Hours;
		hour=output[3];
    output[4]=stimestructure.Minutes;
		min=output[4];
		output[5]=stimestructure.Seconds;
		sec=output[5];
}
void hex2time(unsigned char *input)//将存储的时间取出
{
    year=input[0]+2000;
		mon= input[1];
		day= input[2];
		hour=input[3];
		min= input[4];	
		sec= input[5];				 
}
void HTTP_Time_Convert(char HTTP_IN[])//转换HTTP时间戳为本地时间
{
    int lastday = 0;
		char *strx1;
		char *strx2;
		char mon_char[3];
		char* mon_eg[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
		unsigned int UTCyear,UTCmonth,UTCday,UTChour,UTCminute,UTCsecond;
		strx1=strstr((const char*)HTTP_IN,",")+2;
		strx2=strstr((const char*)strx1," ");
		*strx2=0;
		UTCday=atoi(strx1);
		memcpy(mon_char,strx2+1,3);
		for(i=1;i<13;i++)
		{
			if(moncmp(mon_char,mon_eg[i-1])==0)
			{
				UTCmonth=i;
				break;
			}
			else
				UTCmonth=0;
		}
		strx1=strstr((const char*)strx2+1,"20");
		strx2=strstr((const char*)strx1+1," ");
		*strx2=0;
		UTCyear=atoi(strx1);
		strx2+=1;
		strx1=strstr((const char*)strx2,":");
		*strx1=0;
		UTChour=atoi(strx2);
		strx1+=1;
		strx2=strstr((const char*)strx1,":");
		*strx2=0;
		UTCminute=atoi(strx1);
		
		strx2+=1;
		UTCsecond=atoi(strx2);
		
	  year=UTCyear;
		mon=UTCmonth;
	  day=UTCday;
	  hour=UTChour+8;
		min=UTCminute;
		sec=UTCsecond;
	  if(mon==1 || mon==3 || mon==5 || mon==7 || mon==8 || mon==10 || mon==12)
		{
        lastday = 31;
    }
    else if(mon == 4 || mon == 6 || mon == 9 || mon == 11)
		{
        lastday = 30;
    }
    else
		{
        if((year%400 == 0)||(year%4 == 0 && year%100 != 0))
            lastday = 29;
        else
            lastday = 28;
    }
		if(hour >= 24)
		{
				hour -= 24;
				day += 1; 
				if(day > lastday)
				{ 
						day -= lastday;
						mon += 1;

						if(mon > 12)
						{
								mon -= 12;
								year += 1;
						}
				}
		}
		stimestructure.Hours=hour;
		stimestructure.Minutes=min;
		stimestructure.Seconds=sec;
		sdatestructure.Date=day;
		sdatestructure.Month=mon;
		sdatestructure.Year=year-2000;
		HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
}

float int2float_mem(int *pf)//获取浮点数在内存的原始数据
{
	float n = *(float *)pf;
    return n;
}
float buffer2float(unsigned char * inp)//将分开存储的浮点数还原
{
	int n=0;
	n=(inp[0]<<24)|(inp[1]<<16)|(inp[2]<<8)|inp[3];
	return int2float_mem(&n);
}
int moncmp(char* in1,char* in2)//月份的三个字母比较
{
	if(in1[0]==in2[0]&&in1[1]==in2[1]&&in1[2]==in2[2])
		return 0;
	else
		return 1;
}
void print_pc(unsigned char inputdata[],int length)//串口打印到电脑
{
	int ixxx=0;
	for(ixxx=0;ixxx<length;ixxx++)
	{
		if(inputdata[ixxx]=='\0')
		{
			break;
		}
	}
	HAL_UART_Transmit(&huart3,inputdata,ixxx,100);
}
int ahextoi( const char* hex_str)//16进制字符串转16进制数
{
   int r =0;
  if(hex_str)
     sscanf(hex_str,"%x",&r);
   return r;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
