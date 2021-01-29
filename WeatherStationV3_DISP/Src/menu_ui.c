/*************************************************************************************************************
舸轮综合电子工作室(GIE工作室)        出品
Gearing Interated Electronics Studio

气象站3.0 设置菜单

本文件是设置菜单的相关函数


关注贴吧ID：tnt34 获得最新工作成果
关注B站UP主：GIE工作室 获得更多视频资源

2020-12-20第一版
版权所有 禁止用于任何商业用途！
***********************************************************************************************************/
#include "menu_ui.h"
void Enter_Menu_UI()//进入主菜单
{	
	int pl=0,step_old=0;
	unsigned char dis_bufffer[32];
	Fill_RAM(0x00);
	Draw_Rectangle(0xdd,2,95,95,0,64);
	while(1)
	{
		HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
		voltage_get();
		HAL_IWDG_Refresh(&hiwdg);
		Show_HZ12_12(10,0,0,13,14);//返回
		Show_HZ12_12(10,13,0,15,18);//本地时间
		sprintf((char*)dis_bufffer,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
		Draw_Rectangle(0xdd,1,98,255,15,16);
		Asc5_8(103,3 ,dis_bufffer);
		Show_HZ12_12(10,26,0,19,22);//数据查询
		Show_HZ12_12(10,39,0,19,20);Show_HZ12_12(40,39,0,23,24);//数据删除
		Show_HZ12_12(10,52,0,5,6);Show_HZ12_12(40,52,0,27,27);Asc5_8(52,55 ,"100%");//充电至100%
		sprintf((char*)dis_bufffer,"0x%08X",curr_addr_flash);
		Show_HZ12_12(100,26,0,25,26);Show_HZ12_12(132,26,0,28,29);Asc5_8(164,30 ,dis_bufffer);	//最新地址0xxxxxx
		sprintf((char*)dis_bufffer,"%03.2fMib",Get_Free_Space(curr_addr_flash));
		Show_HZ12_12(100,39,0,30,33);Asc5_8(164,43 ,dis_bufffer);	//剩余空间
		sprintf((char*)dis_bufffer,"%.2lfV",read_adc);
		Show_HZ12_12(100,52,0,15,15);Show_HZ12_12(116,52,0,34,34);Show_HZ12_12(132,52,0,6,6);Show_HZ12_12(148,52,0,8,8);Asc5_8(164,56 ,dis_bufffer);//本机电压
		Asc5_8(0,13*pl+3 ,">");//光标位置
		if((step-step_old)>3)//旋钮编码器读取
		{
			step_old=step;			
			Asc5_8(0,13*pl+3 ," ");//根据读取结果移动光标
			pl++;
			if(pl>=4)
			{
				pl=4;
			}
		}
		else if((step_old-step)>3)
		{
			step_old=step;
			Asc5_8(0,13*pl+3 ," ");	
			pl--;
			if(pl<0)pl=0;
		}
		
		/////////////////////////////////////////////////////////
		if(EncoderKey_Scan()==1)//当按下按钮后进入对应子菜单
		{
			if(pl==0)
			{
			Fill_RAM(0x00);
			Main_UI_Refresh();	
			return;
			}
			if(pl==1) Time_Set_Menu();
			if(pl==2) Data_Query();
			if(pl==3) Data_Del();
			if(pl==4) Charge_Full();
//			if(pl==4) EJMenu();
//			if(pl==5) GPOMenu();
		}
		
	}
}
void Charge_Full(void)
{
	Fill_RAM(0x00);
	Asc5_8(30,20,"Setting Success!");
	HAL_Delay(2000);
	Fill_RAM(0x00);
	Draw_Rectangle(0xdd,2,95,95,0,64);
	charge_com_flag=1;
}
void Data_Del(void)
{
	unsigned char dis_bufffer[32];
	int query_block_addr,pl=0,step_old;
	Fill_RAM(0x00);
	query_block_addr=0x1000*(curr_addr_flash/0x1000);
	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);
		sprintf((char*)dis_bufffer,"0x%08X",query_block_addr);
		Show_HZ12_12(10,0,0,23,26);Show_HZ12_12(74,0,0,39,39);Asc5_8(88,5,dis_bufffer);//删除最新块
		Show_HZ12_12(10,13,0,23,24);Show_HZ12_12(42,13,0,40,41);Show_HZ12_12(74,13,0,39,39);//删除全部块
		Show_HZ12_12(10,26,0,13,14);//返回
		Asc5_8(0,13*pl+3 ,">");//光标位置
		if((step-step_old)>3)//旋钮编码器读取
		{
			step_old=step;			
			Asc5_8(0,13*pl+3 ," ");//根据读取结果移动光标
			pl++;
			if(pl>=2)
			{
				pl=2;
			}
		}
		else if((step_old-step)>3)
		{
			step_old=step;
			Asc5_8(0,13*pl+3 ," ");	
			pl--;
			if(pl<0)pl=0;
		}
		if(EncoderKey_Scan()==1)
		{
			if(pl==0)
			{
				while(1)
				{
					int pl2;
					Show_HZ12_12(128,39,0,42,44);//确定吗
					Asc5_8(128,52 ,"Y");Asc5_8(146,52 ,"N");
					if(pl2==0)
					{
						Asc5_8(120,52 ,">");
						Asc5_8(136,52 ," ");
					}
					else
					{
						Asc5_8(120,52 ," ");
						Asc5_8(136,52 ,">");
					}
						if((step-step_old)>3)//旋钮编码器读取
						{
							step_old=step;			
							pl2++;
							if(pl2>=1)
							{
								pl2=1;
							}
						}
						else if((step_old-step)>3)
						{
							step_old=step;
							pl2--;
							if(pl2<0)pl2=0;
						}
					if(EncoderKey_Scan()==1)
						{
							if(pl2==0)
							{
								Show_HZ12_12(128,39,0,45,45);Show_HZ12_12(144,39,0,45,45);Show_HZ12_12(160,39,0,45,45);
								Show_HZ12_12(128,52,0,45,45);Show_HZ12_12(144,52,0,45,45);Show_HZ12_12(160,52,0,45,45);
								if(curr_addr_flash>=0x11000)
									curr_addr_flash-=0x1000;
								else
									curr_addr_flash=0x10000;
								BSP_W25Qx_Erase_Block(0x00);
								//curr_addr_flash=0x10000;
								default_setting[6]=curr_addr_flash>>16;
								default_setting[7]=(curr_addr_flash&0xff00)>>8;
								default_setting[8]=(curr_addr_flash&0x00ff);
								BSP_W25Qx_Write(default_setting, 0x00,114);
								BSP_W25Qx_Erase_Block(query_block_addr);
								Asc5_8(120,52 ,"done!       ");
								break;
							}
							else
							{
								Show_HZ12_12(128,39,0,45,45);Show_HZ12_12(144,39,0,45,45);Show_HZ12_12(160,39,0,45,45);
								Show_HZ12_12(128,52,0,45,45);Show_HZ12_12(144,52,0,45,45);Show_HZ12_12(160,52,0,45,45);
								break;
							}
						}
				}
			}
			if(pl==1)
			{
				while(1)
				{
					int pl2;
					Show_HZ12_12(128,39,0,42,44);//确定吗
					Asc5_8(128,52 ,"Y");Asc5_8(146,52 ,"N");
					if(pl2==0)
					{
						Asc5_8(120,52 ,">");
						Asc5_8(136,52 ," ");
					}
					else
					{
						Asc5_8(120,52 ," ");
						Asc5_8(136,52 ,">");
					}
						if((step-step_old)>3)//旋钮编码器读取
						{
							step_old=step;			
							pl2++;
							if(pl2>=1)
							{
								pl2=1;
							}
						}
						else if((step_old-step)>3)
						{
							step_old=step;
							pl2--;
							if(pl2<0)pl2=0;
						}
					if(EncoderKey_Scan()==1)
						{
							if(pl2==0)
							{
								Show_HZ12_12(128,39,0,45,45);Show_HZ12_12(144,39,0,45,45);Show_HZ12_12(160,39,0,45,45);
								Show_HZ12_12(128,52,0,45,45);Show_HZ12_12(144,52,0,45,45);Show_HZ12_12(160,52,0,45,45);
								Asc5_8(136,52 ,"working");
								BSP_W25Qx_Erase_Chip();
								curr_addr_flash=0x10000;
								default_setting[6]=curr_addr_flash>>16;
								default_setting[7]=(curr_addr_flash&0xff00)>>8;
								default_setting[8]=(curr_addr_flash&0x00ff);
								BSP_W25Qx_Write(default_setting, 0x00,114);
								Asc5_8(120,52 ,"done!       ");
								break;
							}
							else
							{
								Show_HZ12_12(128,39,0,45,45);Show_HZ12_12(144,39,0,45,45);Show_HZ12_12(160,39,0,45,45);
								Show_HZ12_12(128,52,0,45,45);Show_HZ12_12(144,52,0,45,45);Show_HZ12_12(160,52,0,45,45);
								break;
							}
						}
				}
			}
			if(pl==2)
			{
			Fill_RAM(0x00);
			Draw_Rectangle(0xdd,2,95,95,0,64);
			return;
			}
		}
	}
}
void Data_Query(void)
{
	unsigned char dis_bufffer[32];
	int query_addr,pl=0,step_old;
	Fill_RAM(0x00);
	if(curr_addr_flash==0x10000)
	{
		Asc5_8(30,20,"No record can be found!");
		HAL_Delay(2000);
		Fill_RAM(0x00);
		Draw_Rectangle(0xdd,2,95,95,0,64);
		return;
	}
	query_addr=curr_addr_flash-0x40;
	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);
		if(pl==0)
		{
			Asc5_8(0,0,">");//光标位置
			Asc5_8(0,52," ");
			Asc5_8(100,52," ");
			Asc5_8(160,52," ");
		}
		if(pl==1)
		{
			Asc5_8(0,0," ");//光标位置
			Asc5_8(0,52,">");
			Asc5_8(100,52," ");
			Asc5_8(160,52," ");
		}
		if(pl==2)
		{
			Asc5_8(0,0," ");//光标位置
			Asc5_8(0,52," ");
			Asc5_8(100,52,">");
			Asc5_8(160,52," ");
		}
		if(pl==3)
		{
			Asc5_8(0,0," ");//光标位置
			Asc5_8(0,52," ");
			Asc5_8(100,52," ");
			Asc5_8(160,52,">");
		}
		if((step-step_old)>3)//旋钮编码器读取
		{
			step_old=step;			
			pl++;
			if(pl>=3)
			{
				pl=3;
			}
		}
		else if((step_old-step)>3)
		{
			step_old=step;
			pl--;
			if(pl<0)pl=0;
		}
		Show_HZ12_12(10,52,0,13,14);//返回
		Show_HZ12_12(112,52,0,35,37);//上一页
		Show_HZ12_12(172,52,0,38,38);Show_HZ12_12(188,52,0,36,37);//下一页
		sprintf((char*)dis_bufffer,"0x%08X",query_addr);
		Show_HZ12_12(10,0,0,21,22);Show_HZ12_12(42,0,0,28,29);Asc5_8(72,5 ,dis_bufffer);//查询地址
		if(EncoderKey_Scan()==1)
		{
			if(pl==0)
			{
				while(1)
				{
					HAL_IWDG_Refresh(&hiwdg);
					sprintf((char*)dis_bufffer,"0x%08X",query_addr);
					Show_HZ12_12(10,0,0,21,22);Show_HZ12_12(42,0,0,28,29);Asc5_8(72,5 ,dis_bufffer);//查询地址
					if((step-step_old)>3)//旋钮编码器读取
						{
							step_old=step;			
							query_addr+=0x40;
							if(query_addr>=curr_addr_flash-0x40)
							{
								query_addr=0x10000;
							}
						}
					else if((step_old-step)>3)
					{
						step_old=step;
						query_addr-=0x40;
						if(query_addr<0x10000)query_addr=curr_addr_flash-0x40;
					}
					if(EncoderKey_Scan()==1)
					{
						Query_Time_Rec_Disp(query_addr);
						break;
					}
				}
			}
			if(pl==1)
			{
				Fill_RAM(0x00);
				Draw_Rectangle(0xdd,2,95,95,0,64);
				return;
			}
			if(pl==2)
			{
				query_addr-=0x80;
				if(query_addr<0x10000)
				{
					query_addr=curr_addr_flash-0x80;
					if(curr_addr_flash==0x10040)
						query_addr=0x10000;
				}
				Query_Time_Rec_Disp(query_addr);
			}
			if(pl==3)
			{
				query_addr+=0x80;
				if(query_addr>=curr_addr_flash-0x40)
				{
					query_addr=0x10000;
				}
				Query_Time_Rec_Disp(query_addr);
			}
		}
	}
	
	
}
void Time_Set_Menu(void)//时间设置菜单
{
	int pl=-1,step_old=0,lastday=0;
	unsigned char dis_bufffer_2[32];
	Fill_RAM(0x00);
	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);
		Show_HZ12_12(10,52,0,13,14);//返回
		if(pl==5)
			Asc5_8(0,52 ,">");
		else
		{
			Asc5_8(0,52 ," ");
			Asc12_24(pl*35+60,44,"^");
		}
		
		if((step-step_old)>3)//旋钮编码器读取
		{
			step_old=step;			
			Asc12_24(pl*35+60,44," ");//根据读取结果移动光标
			pl++;
			if(pl>=5)
			{
				pl=5;
			}
		}
		else if((step_old-step)>3)
		{
			step_old=step;
			Asc12_24(pl*35+60,44 ," ");	
			pl--;
			if(pl<0)pl=0;
		}
		sprintf((char*)dis_bufffer_2,"20%02d-%02d-%02d %02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes);
		Asc12_24(30,20,dis_bufffer_2);
		if(EncoderKey_Scan()==1)//当按下按钮后开始调时
		{
			if(pl==5)
			{
			Fill_RAM(0x00);
			Draw_Rectangle(0xdd,2,95,95,0,64);
			return;
			}
			if(pl==0)
			{
				while(1)
				{
					HAL_IWDG_Refresh(&hiwdg);
					if((step-step_old)>3)//旋钮编码器读取
					{
						step_old=step;			
						sdatestructure.Year++;
						if(sdatestructure.Year++>=99)
						{
							sdatestructure.Year=0;
						}
					}
					else if((step_old-step)>3)
					{
						step_old=step;
						sdatestructure.Year--;
						if(sdatestructure.Year==0)sdatestructure.Year=99;
					}
					sprintf((char*)dis_bufffer_2,"20%02d-%02d-%02d %02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes);
					Asc12_24(30,20,dis_bufffer_2);
					if(EncoderKey_Scan()==1)//当按下按钮后保存结果
					{HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);break;}
				}
			}
			if(pl==1)
			{
				while(1)
				{
					HAL_IWDG_Refresh(&hiwdg);
					if((step-step_old)>3)//旋钮编码器读取
					{
						step_old=step;			
						sdatestructure.Month++;
						if(sdatestructure.Month++>=13)
						{
							sdatestructure.Month=1;
						}
					}
					else if((step_old-step)>3)
					{
						step_old=step;
						sdatestructure.Month--;
						if(sdatestructure.Month==0)sdatestructure.Month=12;
					}
					sprintf((char*)dis_bufffer_2,"20%02d-%02d-%02d %02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes);
					Asc12_24(30,20,dis_bufffer_2);
					if(EncoderKey_Scan()==1)//当按下按钮后保存结果
					{HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);break;}
				}
			}
			if(pl==2)
			{
				while(1)
				{
					HAL_IWDG_Refresh(&hiwdg);
					if(sdatestructure.Month==1 || sdatestructure.Month==3 || sdatestructure.Month==5 || sdatestructure.Month==7 || sdatestructure.Month==8 || sdatestructure.Month==10 || sdatestructure.Month==12)
					{
							lastday = 31;
					}
					else if(sdatestructure.Month == 4 || sdatestructure.Month == 6 || sdatestructure.Month == 9 || sdatestructure.Month == 11)
					{
							lastday = 30;
					}
					else
					{
							if((sdatestructure.Year%400 == 0)||(sdatestructure.Year%4 == 0 && sdatestructure.Year%100 != 0))
									lastday = 29;
							else
									lastday = 28;
					}
					if((step-step_old)>3)//旋钮编码器读取
					{
						step_old=step;			
						sdatestructure.Date++;
						if(sdatestructure.Date++>=lastday)
						{
							sdatestructure.Date=1;
						}
					}
					else if((step_old-step)>3)
					{
						step_old=step;
						sdatestructure.Date--;
						if(sdatestructure.Date==0)sdatestructure.Date=lastday;
					}
					sprintf((char*)dis_bufffer_2,"20%02d-%02d-%02d %02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes);
					Asc12_24(30,20,dis_bufffer_2);
					if(EncoderKey_Scan()==1)//当按下按钮后保存结果
					{HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);break;}
					
				}
			}
			if(pl==3)
			{
				while(1)
				{
					HAL_IWDG_Refresh(&hiwdg);
					if((step-step_old)>3)//旋钮编码器读取
					{
						step_old=step;			
						stimestructure.Hours++;
						if(stimestructure.Hours++>=24)
						{
							stimestructure.Hours=0;
						}
					}
					else if((step_old-step)>3)
					{
						step_old=step;
						stimestructure.Hours--;
						if(stimestructure.Hours==0)stimestructure.Hours=23;
					}
					sprintf((char*)dis_bufffer_2,"20%02d-%02d-%02d %02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes);
					Asc12_24(30,20,dis_bufffer_2);
					if(EncoderKey_Scan()==1)//当按下按钮后保存结果
					{HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);break;}
				}
			}
			if(pl==4)
			{
				while(1)
				{
					HAL_IWDG_Refresh(&hiwdg);
					if((step-step_old)>3)//旋钮编码器读取
					{
						step_old=step;			
						stimestructure.Minutes++;
						if(stimestructure.Minutes++>=60)
						{
							stimestructure.Minutes=0;
						}
					}
					else if((step_old-step)>3)
					{
						step_old=step;
						stimestructure.Minutes--;
						if(stimestructure.Minutes==0)stimestructure.Minutes=59;
					}
					sprintf((char*)dis_bufffer_2,"20%02d-%02d-%02d %02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes);
					Asc12_24(30,20,dis_bufffer_2);
					if(EncoderKey_Scan()==1)//当按下按钮后保存结果
					{HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);break;}
				}
			}
		}
		
	}
}
float Get_Free_Space(int addr)
{
	float free_addr;
	if(addr>0x1000000)
		return 0;
	free_addr=0x1000000-addr;
	free_addr/=1024.0;
	free_addr/=1024.0;
	free_addr*=8;
	return free_addr;
	
}
void Package_Decoding_Dis(uint8_t input[])
{	
		env_display.temp=buffer2float(input);
		env_display.humi=buffer2float(input+4);
		env_display.pressure=buffer2float(input+8);
		env_display.gas_r=buffer2float(input+12);
		env_display.dust_25=input[17]<<24 | input[18]<<16 | input[19]<<8 | input[20]; 
		env_display.uvv=input[21]<<8 | input[22];
		env_display.co2_eq_ppm=input[23]<<8 | input[24];
		env_display.tvoc_ppb=input[25]<<8 | input[26];
}
void Query_Time_Rec_Disp(int addr)
{
	unsigned char temp_data2[64],dis_bufffer[128],time_store[6];
	
	BSP_W25Qx_Read(temp_data2,addr,0x40);
	memcpy(time_store,temp_data2+32,6);
	Package_Decoding_Dis(temp_data2);
	sprintf((char *)dis_bufffer,"%.1fC %.1f%% %.0fPa %dug",env_display.temp,env_display.humi,env_display.pressure,env_display.dust_25);
	Asc5_8(0,13,dis_bufffer);
	sprintf((char *)dis_bufffer,"%duW %dppm %dppb 20%02d%02d%02d %02d%02d",env_display.uvv,env_display.co2_eq_ppm,env_display.tvoc_ppb,time_store[0],time_store[1],time_store[2],time_store[3],time_store[4]);
	Asc5_8(0,22,dis_bufffer);
	
	if(curr_addr_flash==0x10040)
		return;
	BSP_W25Qx_Read(temp_data2,addr+0x40,0x40);
	if(temp_data2[0]==0xFF&&temp_data2[1]==0xFF&&temp_data2[2]==0xFF)
		return;
	memcpy(time_store,temp_data2+32,6);
	Package_Decoding_Dis(temp_data2);
	sprintf((char *)dis_bufffer,"%.1fC %.1f%% %.0fPa %dug",env_display.temp,env_display.humi,env_display.pressure,env_display.dust_25);
	Asc5_8(0,31,dis_bufffer);
	sprintf((char *)dis_bufffer,"%duW %dppm %dppb 20%02d%02d%02d %02d%02d",env_display.uvv,env_display.co2_eq_ppm,env_display.tvoc_ppb,time_store[0],time_store[1],time_store[2],time_store[3],time_store[4]);
	Asc5_8(0,40,dis_bufffer);
}
