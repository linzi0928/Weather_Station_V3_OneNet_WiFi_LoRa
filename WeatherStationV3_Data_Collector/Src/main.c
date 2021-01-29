
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2021 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "adc.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "string.h"
//#include "nbiot.h"
//#include "hexstring.h"
#include "stdlib.h"
#include "platform.h"
#include "radio.h"
extern tRadioDriver *Radio;
#define BUFFER_SIZE     32
//static uint16_t BufferSize = BUFFER_SIZE;
tRadioDriver *Radio = NULL;

unsigned char Sensors_data[BUFFER_SIZE];
char msgSend[BUFFER_SIZE*2];
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
/*
GIE����վ����V3��
Ӳ���ṹ��STM32L051�͹��ĵ�Ƭ��
BME680���崫���������¶ȡ�ʪ�ȡ���ѹ������
ZPH02�۳�����������PM2.5
VEML6070�����ߴ���������������
SGP30����CO2��TVOC����
SX1278ģ��ͨ��LoRa���͸����ڻ��洢���ϴ�������OneNetƽ̨��Ӳ����ҲԤ����8266�Ŀڣ�����ֱ�Ӵӻ���ɼ����ϴ�����������
�͹���������ʽ��wakeup��ʱ��ÿ15s����һ��ι���Ź���28sδι����������Ƭ������Ȼ����������STOPģʽ������ȥ225��15s�������ں���һ�����ݣ�ʵ����Ϊ1Сʱһ�����ݣ�
����״̬����������~4uA
����״̬����������~80mA
�������÷�ʽ���䴮��������2.8V 3000F�������ݣ�3W9V��С̫���ܰ��ڳ������������µ�ѹҲ�������
ע�������CUBEMX���������ɴ��룬ע�ͽ�ȫ��������룬����ǰ���ݣ���������س�����sensors.c�ļ���
��ӭ��עbվup����GIE�����ң���ó���ɻ���
Designed by LinZi
GIE Studio 2020-12-12
*/
uint32_t dust25;//PM2.5ֵ
uint16_t uvv;//������ֵ
float tem;//�¶�ֵ
float hum;//ʪ��ֵ
float press;//��ѹֵ
float gas_r;//����ֵ
float batval;//��ص���
uint32_t voc_level;//voc�ȼ�
extern u16 tvoc_ppb, co2_eq_ppm;//CO2��TVOC
uint8_t sleep_counter=0;
uint8_t wakeup_count=0;
RTC_HandleTypeDef RTCHandle;
void SystemPower_Config(void);
void Enter_STOP_mode(void);
int Package_Point=0,succ_flag=0;
int load_float2buffer(float input,unsigned char* buffer_pointer);
void voltage_get(void);
uint32_t user_offset=27000000;
extern void VEML6070_Init(void);
extern void Get_sensors(void);
extern void osDelay(uint16_t time);
extern void bme680_init_user (void);
extern void Get_680(void);
void get_resert_status(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SPI2_Init();
  MX_RTC_Init();
  MX_ADC_Init();
  MX_USART2_UART_Init();
  MX_IWDG_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
		__PWR_CLK_ENABLE();
	  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
		get_resert_status();
		HAL_Delay(2000);//�����ȴ������ٴ򿪸������裬����ӿ��Ź���λ��������������û���������
		HAL_IWDG_Refresh(&hiwdg);
  while (1)
  {
	
	HAL_PWREx_EnableUltraLowPower();//ʹ�ܳ��͹���
  HAL_PWREx_EnableFastWakeUp();
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
		HAL_Delay(1000);
	//HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer1, 1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);//��3.3V��ѹ���أ�TPS63070����Ϊȫ���������Լ�����ģ�鹩��
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//�򿪵�ƽת��оƬ���أ�TXS0108��
	HAL_Delay(1000);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer2, 1);
	HAL_IWDG_Refresh(&hiwdg);
	bme680_init_user ();//��ʼ��BME680�����ùٷ�API��
	Get_680();//��ȡ�¶ȡ�ʪ�ȡ���ѹ����������
	BoardInit( );
	Radio = RadioDriverInit( );
	Radio->Init( );
	sleep_counter=0;
	Package_Point=0;
	succ_flag=0;
	VEML6070_Init();//��ʼ�������ߴ�����
	SGP_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		
		Get_sensors();//��ȡ����������
		voltage_get();
		//tem=23.45;hum=56.34;press=101232;gas_r=982;voc_level=1;dust25=1231;uvv=22;//co2_eq_ppm=431;tvoc_ppb=23;//�����ü�����
		Package_Point+=load_float2buffer(tem,Sensors_data+Package_Point);//0-3 �¶�
		Package_Point+=load_float2buffer(hum,Sensors_data+Package_Point);//4-7 ʪ��
		Package_Point+=load_float2buffer(press,Sensors_data+Package_Point);//8-11 ��ѹ
		Package_Point+=load_float2buffer(gas_r,Sensors_data+Package_Point);//12-15 ����
		Sensors_data[Package_Point]=voc_level;//16 VOC�ȼ�������ZPH02��
		Package_Point++;
		Sensors_data[Package_Point]=dust25>>24;//17-20 PM2.5
		Sensors_data[Package_Point+1]=(dust25&0x00ff0000)>>16;
		Sensors_data[Package_Point+2]=(dust25&0x0000ff00)>>8;
		Sensors_data[Package_Point+3]=(dust25&0x000000ff);
		Package_Point+=4;
		Sensors_data[Package_Point]=uvv>>8;//21-22 ������
		Sensors_data[Package_Point+1]=uvv&0x00ff;
		Package_Point+=2;
		Sensors_data[Package_Point]=co2_eq_ppm>>8;//23-24 CO2
		Sensors_data[Package_Point+1]=co2_eq_ppm&0x00ff;
		Sensors_data[Package_Point+2]=tvoc_ppb>>8;//25-26 TVOC
		Sensors_data[Package_Point+3]=tvoc_ppb&0x00ff;
		Package_Point+=4;
		Package_Point+=load_float2buffer(batval,Sensors_data+Package_Point);//27-30 ���ݵ���
		HAL_IWDG_Refresh(&hiwdg);
		Radio->SetTxPacket( Sensors_data, 32 );
		while(1)
		{
			while(Radio->Process( ) == RF_TX_DONE)
			{
				Enter_STOP_mode();//����STOP�͹���ģʽ
				succ_flag=1;
				//Radio->SetTxPacket( TxBuf, 32 );
			}
			if(succ_flag==1)
			{
				break;
			}
		}

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* RTC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* USER CODE BEGIN 4 */
void SystemPower_Config(void)//���õ�Ƭ���������裬ʹ���ڵ͹���ģʽ�²���ĵ�
{
  GPIO_InitTypeDef GPIO_InitStructure;
	

	//HAL_UART_DeInit(&huart1);//�رմ���1
  HAL_UART_DeInit(&huart2);//�رմ���2
	HAL_SPI_DeInit(&hspi2);//�ر�SPI
//	__HAL_RCC_DMA1_CLK_DISABLE();
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);//ʧ��3.3V��ѹ���ϵ�ȫ������������Ƶģ��
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  GPIO_InitStructure.Pin = GPIO_PIN_All;//��ȫ���˿�����Ϊģ������ģʽ����©��
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

//  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();//�ر�����GPIOʱ��
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();

  /* Configure RTC */
  hrtc.Instance = RTC;//����RTC��ʱ��

  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}
void Enter_STOP_mode(void)//����STOP����ģʽ������
{
	//	HAL_DBGMCU_EnableDBGStopMode();//����STOPģʽ�µĵ��ԣ��򿪺�ϵͳʱ�ӻ����Ϊ�������ṩʱ�ӣ�����STOPģʽ�µ���
		SystemPower_Config();//����˯��ģʽǰ�ĵ�Դ����
		HAL_IWDG_Refresh(&hiwdg);//ι��
	while(1)
	{
		HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 15, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);//���û��Ѽ�ʱ��Ϊ15��
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);//����STOPģʽ�������ڴ���ͣ�����Ѻ�Ӵ˴�����ִ�У�
		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);//���Ѻ��ȹرջ��Ѽ�ʱ��
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//�������FLAG�����޷��ٴ�˯��
		HAL_IWDG_Refresh(&hiwdg);//���Ѻ�ι��
		wakeup_count++;//����һ�����һ�Σ�������while(1)�г���ι��
		if(wakeup_count>=155)//������225�κ�Լ1Сʱ��
		{
			wakeup_count=0;
			HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);//�رջ��Ѽ�ʱ�������ѭ��
			break;
		}
		HAL_UART_DeInit(&huart2);
	}
		SystemClock_Config();//�ָ�ϵͳʱ��
		MX_GPIO_Init();//��ʼ����������
		//MX_USART1_UART_Init();
		MX_USART2_UART_Init();
		MX_SPI2_Init();
	  MX_NVIC_Init();//Ȼ�󽫼�����main������while(1)��ִ��һ���ȡ�����뷢��
}
int float2int_mem(float *pf)//��ȡ���������ڴ��ԭʼ����
{
	int n = *(int *)pf;
    return n;
}
int load_float2buffer(float input,unsigned char* buffer_pointer)//���������ֳ�4����װ��u8��������
{
    int ttn=0;
    ttn=float2int_mem(&input);
    *buffer_pointer=ttn>>24;
    *(buffer_pointer+1)=(0x00ff0000&ttn)>>16;
    *(buffer_pointer+2)=(0x0000ff00&ttn)>>8;
    *(buffer_pointer+3)= 0x000000ff&ttn;
    return 4;
}	
void voltage_get(void)
{
	int time_out=0,adc_raw;
	HAL_ADC_Start(&hadc);
	HAL_ADC_GetValue(&hadc);    
	while(HAL_ADC_PollForConversion(&hadc, 0xff)!=HAL_OK)
	{
		time_out++;
		if(time_out>10000)
			return;
	}
	adc_raw=HAL_ADC_GetValue(&hadc);
	batval=((float)adc_raw/65535.0)*1.8*70+0.20;//����Ϊ��ѹ
}
void print_pc(unsigned char inputdata[])//���ڴ�ӡ������
{
	int ixxx=0;
	for(ixxx=0;ixxx<10;ixxx++)
	{
		if(inputdata[ixxx]=='\0')
		{
			break;
		}
	}
	HAL_UART_Transmit(&huart2,inputdata,ixxx,100);
}
void get_resert_status(void)
{
	print_pc("RES_REASON:");
    if( READ_BIT(RCC->CSR, RCC_CSR_FWRSTF) != RESET)
    {
        print_pc("HWREN\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_OBLRSTF) != RESET)
    {
        print_pc("BITPRET\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_PINRSTF) != RESET)
    {
        print_pc("RST_KEY\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF) != RESET)
    {
        print_pc("SFT\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF) != RESET)
    {
        print_pc("IWDG\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_WWDGRSTF) != RESET)
    {
        print_pc("WWDG\n");
    }
    if( READ_BIT(RCC->CSR, RCC_CSR_LPWRRSTF) != RESET)
    {
        print_pc("LPWR\n");
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();
	}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
