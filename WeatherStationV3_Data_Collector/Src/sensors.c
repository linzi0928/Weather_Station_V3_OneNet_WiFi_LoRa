/*
GIE气象站程序
		本文件为传感器相关函数。
Designed by LinZi
GIE Studio 2020-02-06
*/
#include "sensors.h"
void Get_sensors(void)//传感器数据采集函数
{
	int ttt=0,tt=0;
	float co2_sum=0,tvoc_sum=0;
	Get_UV();//获取紫外线数据
	SGP_Get();
	while((!Get_dust25()||(ttt<20))&&(ttt<25))//通过串口获得PM2.5数据，反复执行20次确保数据包整齐
	{
		//HAL_Delay(500);/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HAL_IWDG_Refresh(&hiwdg);
		SGP_Get();
		if(co2_eq_ppm!=400)
		{
			co2_sum+=co2_eq_ppm;
			tvoc_sum+=tvoc_ppb;
			tt++;
		}
		ttt++;
	}
	ttt=0;
	if(tt==0)
	{
		while(ttt<20)
		{
			HAL_IWDG_Refresh(&hiwdg);
			SGP_Get();
			if(co2_eq_ppm!=400)
			{
				co2_sum=co2_eq_ppm;
				tvoc_sum=tvoc_ppb;
				tt=1;
				break;
			}
			ttt++;
		}
	}
	co2_eq_ppm=(co2_sum/(float)tt);
	tvoc_ppb=(tvoc_sum/(float)tt);
	if(co2_eq_ppm==0)
	{
		co2_eq_ppm=400;
	}
}
int Get_dust25(void)//PM2.5检测程序
{
	float dust_duty=0;
	if(USART2_RX_BUF[0]==0xff&&USART2_RX_BUF[1]==0x18)//确保数据包对齐，方便提取有效数据
	{
		dust_duty=(USART2_RX_BUF[3]*100)+USART2_RX_BUF[4];
		dust_duty/=100;
		dust25=dust_duty*8.8888f;
		voc_level=USART2_RX_BUF[7];
		return 1;//获取成功
	}
	else
		return 0;//获取失败
}
void Get_UV(void)//紫外线检测程序
{
	uvv=VEML6070_ReadValue();
}
void Get_680(void)//温度湿度气压TVOC检测程序
{
	gas_sensor.power_mode = BME680_FORCED_MODE;
	bme680_set_sensor_mode(&gas_sensor);
	HAL_Delay(2000);
	bme680_get_sensor_data(&data, &gas_sensor);//获取bme680数据
	tem=((float)(data.temperature))/100.0f;//将数据传出
	hum=((float)(data.humidity))/1000.0f;
	press=(float)data.pressure;
	gas_r=data.gas_resistance/100.0f;
	if((data.temperature<(-5000))||(data.temperature>8000)||(data.humidity>(100000))||(data.pressure>(150000))||(data.pressure<(50000)))
	{//检测数据有效性，如果不满足要求则再测一次
		gas_sensor.power_mode = BME680_FORCED_MODE;
		bme680_set_sensor_mode(&gas_sensor);
		HAL_Delay(2000);
		bme680_get_sensor_data(&data, &gas_sensor);
		tem=((float)(data.temperature))/100.0;//将数据传出
		hum=((float)(data.humidity))/1000.0;
		press=data.pressure;
		gas_r=data.gas_resistance/100;
	}
	else if(fflag==0)
	{
		fflag=1;
		gas_sensor.power_mode = BME680_SLEEP_MODE;
		bme680_set_sensor_mode(&gas_sensor);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
	}
}
void bme680_init_user (void)
{
	
//	bsec_iot_init(BSEC_SAMPLE_RATE_LP, 0.0f, user_spi_write, user_spi_read, HAL_Delay, state_load, config_load).bme680_status;
    /* You may assign a chip select identifier to be handled later */
    gas_sensor.dev_id = 0;
    gas_sensor.intf = BME680_SPI_INTF;
    gas_sensor.read = user_spi_read;
    gas_sensor.write = user_spi_write;
    gas_sensor.delay_ms = HAL_Delay;
    /* amb_temp can be set to 25 prior to configuring the gas sensor 
     * or by performing a few temperature readings without operating the gas sensor.
     */
    gas_sensor.amb_temp = 25;

//    int8_t rslt = BME680_OK;
//    rslt = 
	bme680_init(&gas_sensor);
	
	bme680_get_profile_dur(&meas_period, &gas_sensor);
	
	 uint8_t set_required_settings;

    /* Set the temperature, pressure and humidity settings */
    gas_sensor.tph_sett.os_hum = BME680_OS_2X;
    gas_sensor.tph_sett.os_pres = BME680_OS_4X;
    gas_sensor.tph_sett.os_temp = BME680_OS_8X;
    gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
    gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    gas_sensor.power_mode = BME680_FORCED_MODE; 

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL 
        | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    bme680_set_sensor_settings(set_required_settings,&gas_sensor);

    /* Set the power mode */
		bme680_set_sensor_mode(&gas_sensor);

	
}
int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
	uint8_t regaddr[1]={reg_addr};
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
	rslt=HAL_SPI_Transmit(&hspi2,regaddr,1,100);
	rslt=HAL_SPI_Transmit(&hspi2,reg_data,len,100);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
  return rslt;
}
int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	 int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
	uint8_t regaddr[1]={reg_addr|0x80};
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
	rslt=HAL_SPI_Transmit(&hspi2,regaddr,1,100);
	rslt=HAL_SPI_Receive(&hspi2, reg_data, len, 100);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
   return rslt;
}
void user_delay_ms(uint32_t period)
{    
   uint16_t i=0;  
   while(period--)
   {
      i=3450;  //自己定义  调试时间：20141116
      while(i--) ;    
   }
}

