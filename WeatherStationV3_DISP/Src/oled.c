//////////////////////////////////////////////////////////////////////////////////	 
// M31S1322_01_V1 FOR STM32 201505 WWW.LCDSOC.COM

//********************************************************************************
//接口定义参照oled.h文件
#include "oled.h"
#include "stdlib.h"
//定义ASCII库
#include "Asc5x8.h" 	   //ascii 5*8字库
#include "Asc6x12.h" 	   //ascii 8*16字库
#include "Asc8x16.h" 	   //ascii 8*16字库
#include "Asc12x24.h" 	   //ascii 12*24字库
#include "Asc20x40.h" 	   //ascii 20*40字库
//自定义汉字库
#include "HZ12X12_S.h" 	   //12*12宋体自定义汉字库
#include "HZ16X16_S.h" 	   //16*16宋体自定义汉字库
#include "HZ24X24_S.h" 	   //24*24宋体自定义汉字库
// #include "HZ24X24_H.h" 	   //24*24宋体自定义汉字库
// #include "HZ24X24_K.h" 	   //24*24宋体自定义汉字库


//向SSD1325写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 
  
   
//-----------------OLED端口定义 4线SPI----------------  					   
/* 采用IO口模拟SPI通讯方式驱动oled模块
CS   ~ PE6
RES ~ PE5
DC   ~ PE4
CLK  ~ PE3
DIN ~ PE2
*/  
//初始化SSD1322				    
void OLED_Init(void)
{ 	
 
  OLED_RST_Set();
	HAL_Delay(10);
	OLED_RST_Clr();
	HAL_Delay(10);
	OLED_RST_Set(); 
		
	OLED_WR_Byte(0xAE,OLED_CMD); //	Display Off
	
	OLED_WR_Byte(0xFD,OLED_CMD); // Set Command Lock
	OLED_WR_Byte(0x12,OLED_DATA); //
	
	OLED_WR_Byte(0xB3,OLED_CMD); // Set Clock as 80 Frames/Sec
	OLED_WR_Byte(0x91,OLED_DATA); //  

   	OLED_WR_Byte(0xCA,OLED_CMD); // Set Multiplex Ratio
	OLED_WR_Byte(0x3F,OLED_DATA); // 1/64 Duty (0x0F~0x5F)
   
	OLED_WR_Byte(0xA2,OLED_CMD); // Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_DATA); //	
	
	OLED_WR_Byte(0xA1,OLED_CMD); // Set Mapping RAM Display Start Line (0x00~0x7F)
	OLED_WR_Byte(0x00,OLED_DATA); //	

	
	OLED_WR_Byte(0xA0,OLED_CMD); //Set Column Address 0 Mapped to SEG0 
	OLED_WR_Byte(0x14,OLED_DATA);      	//   Default => 0x40
						                //     Horizontal Address Increment
						//     Column Address 0 Mapped to SEG0
						//     Disable Nibble Remap
						//     Scan from COM0 to COM[N-1]
						//     Disable COM Split Odd Even
	OLED_WR_Byte(0x11,OLED_DATA);      	//    Default => 0x01 (Disable Dual COM Mode)

	
	OLED_WR_Byte(0xB5,OLED_CMD); //  Disable GPIO Pins Input
	OLED_WR_Byte(0x00,OLED_DATA); //	
	
    OLED_WR_Byte(0xAB,OLED_CMD); //   Enable Internal VDD Regulator
	OLED_WR_Byte(0x01,OLED_DATA); //

	OLED_WR_Byte(0xB4,OLED_CMD); //  Display Enhancement  
	OLED_WR_Byte(0xA0,OLED_DATA); // Enable External VSL
	OLED_WR_Byte(0xF8,OLED_DATA); // Enhance Low Gray Scale Display Quality

    OLED_WR_Byte(0xC1,OLED_CMD); //  Set Contrast Current 
	OLED_WR_Byte(0xEF,OLED_DATA); //  Default => 0x7F

    OLED_WR_Byte(0xC7,OLED_CMD); //  Master Contrast Current Control 
	OLED_WR_Byte(0xff,OLED_DATA); //  Default => 0x0f (Maximum)

    OLED_WR_Byte(0xB8,OLED_CMD); //  	// Set Gray Scale Table 
	OLED_WR_Byte(0x0C,OLED_DATA); //
	OLED_WR_Byte(0x18,OLED_DATA); //
	OLED_WR_Byte(0x24,OLED_DATA); //
	OLED_WR_Byte(0x30,OLED_DATA); //
	OLED_WR_Byte(0x3C,OLED_DATA); //
	OLED_WR_Byte(0x48,OLED_DATA); //
	OLED_WR_Byte(0x54,OLED_DATA); //
	OLED_WR_Byte(0x60,OLED_DATA); //
	OLED_WR_Byte(0x6C,OLED_DATA); //
	OLED_WR_Byte(0x78,OLED_DATA); //
	OLED_WR_Byte(0x84,OLED_DATA); //
	OLED_WR_Byte(0x90,OLED_DATA); //
	OLED_WR_Byte(0x9C,OLED_DATA); //
	OLED_WR_Byte(0xA8,OLED_DATA); //
	OLED_WR_Byte(0xB4,OLED_DATA); //
 	OLED_WR_Byte(0x00,OLED_CMD); 	// Enable Gray Scale Table

	
	OLED_WR_Byte(0xB1,OLED_CMD); //   Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
	OLED_WR_Byte(0xE2,OLED_DATA); //  Default => 0x74 (7 Display Clocks [Phase 2] / 9 Display Clocks [Phase 1])

    OLED_WR_Byte(0xD1,OLED_CMD); //    Enhance Driving Scheme Capability (0x00/0x20)
	OLED_WR_Byte(0xA2,OLED_DATA); //   Normal
	OLED_WR_Byte(0x20,OLED_DATA); //

   
    OLED_WR_Byte(0xBB,OLED_CMD); //     Set Pre-Charge Voltage Level as 0.60*VCC
	OLED_WR_Byte(0x1F,OLED_DATA); //

    OLED_WR_Byte(0xB6,OLED_CMD); //  // Set Second Pre-Charge Period as 8 Clocks 
	OLED_WR_Byte(0x08,OLED_DATA); //

    OLED_WR_Byte(0xBE,OLED_CMD); //  Set Common Pins Deselect Voltage Level as 0.86*VCC 
	OLED_WR_Byte(0x07,OLED_DATA); //  Default => 0x04 (0.80*VCC)

    OLED_WR_Byte(0xA6,OLED_CMD); //     Normal Display Mode (0x00/0x01/0x02/0x03)
	 								  //   Default => 0xA4
						//     0xA4  => Entire Display Off, All Pixels Turn Off
						//     0xA5  => Entire Display On, All Pixels Turn On at GS Level 15
						//     0xA6  => Normal Display
						//     0xA7  => Inverse Display
	 
	 OLED_WR_Byte(0xA9,OLED_CMD); //   Disable Partial Display
     	 	
	Fill_RAM(0x00);
	OLED_WR_Byte(0xAF,OLED_CMD); //	Display On
 

}  

void Set_Column_Address(unsigned char a, unsigned char b)
{
	OLED_WR_Byte(0x15,OLED_CMD);			// Set Column Address
	OLED_WR_Byte(a,OLED_DATA);			//   Default => 0x00
	OLED_WR_Byte(b,OLED_DATA);			//   Default => 0x77
}


void Set_Row_Address(unsigned char a, unsigned char b)
{
	OLED_WR_Byte(0x75,OLED_CMD);			// Set Row Address
	OLED_WR_Byte(a,OLED_DATA);			//   Default => 0x00
	OLED_WR_Byte(b,OLED_DATA);			//   Default => 0x7F
}

void Set_Write_RAM()
{
 
	OLED_WR_Byte(0x5C,OLED_CMD);			// Enable MCU to Write into RAM
}

void Set_Read_RAM()
{
	OLED_WR_Byte(0x5D,OLED_CMD);			// Enable MCU to Read from RAM
}


 void Set_Remap_Format(unsigned char d)
{
	 OLED_WR_Byte(0xA0,OLED_CMD);  			// Set Re-Map / Dual COM Line Mode
  OLED_WR_Byte(d,OLED_DATA);  				//   Default => 0x40
						//     Horizontal Address Increment
						//     Column Address 0 Mapped to SEG0
						//     Disable Nibble Remap
						//     Scan from COM0 to COM[N-1]
						//     Disable COM Split Odd Even
    OLED_WR_Byte(0x11,OLED_DATA); 	 			//   Default => 0x01 (Disable Dual COM Mode)
} 
  void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c)
{
  	OLED_WR_Byte((0xA8|a),OLED_CMD);		// Default => 0x8F
					
	if(a == 0x00)
	{
	    OLED_WR_Byte(b,OLED_DATA); 
		OLED_WR_Byte(c,OLED_DATA); 
	 
	}
}

void Set_Display_Offset(unsigned char d)
{
	OLED_WR_Byte(0xA2,OLED_CMD);
 	OLED_WR_Byte(d,OLED_DATA);
 
}

void Set_Start_Line(unsigned char d)
{
	OLED_WR_Byte(0xA1,OLED_CMD);	// Set Vertical Scroll by RAM
 	OLED_WR_Byte(d,OLED_DATA);
 
}

void Set_Master_Current(unsigned char d)
{
   OLED_WR_Byte(0xC7,OLED_CMD);	//  Master Contrast Current Control
 	OLED_WR_Byte(d,OLED_DATA);

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table()
{

    OLED_WR_Byte(0xB8,OLED_CMD); //  	// Set Gray Scale Table 
	OLED_WR_Byte(0x0C,OLED_DATA); //
	OLED_WR_Byte(0x18,OLED_DATA); //
	OLED_WR_Byte(0x24,OLED_DATA); //
	OLED_WR_Byte(0x30,OLED_DATA); //
	OLED_WR_Byte(0x3C,OLED_DATA); //
	OLED_WR_Byte(0x48,OLED_DATA); //
	OLED_WR_Byte(0x54,OLED_DATA); //
	OLED_WR_Byte(0x60,OLED_DATA); //
	OLED_WR_Byte(0x6C,OLED_DATA); //
	OLED_WR_Byte(0x78,OLED_DATA); //
	OLED_WR_Byte(0x84,OLED_DATA); //
	OLED_WR_Byte(0x90,OLED_DATA); //
	OLED_WR_Byte(0x9C,OLED_DATA); //
	OLED_WR_Byte(0xA8,OLED_DATA); //
	OLED_WR_Byte(0xB4,OLED_DATA); //
 	OLED_WR_Byte(0x00,OLED_CMD); 	// Enable Gray Scale Table

}
void Set_Linear_Gray_Scale_Table()
{
   	OLED_WR_Byte(0xB9,OLED_CMD); //	Set Default Linear Gray Scale Table
 
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Scrolling (Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward)		向上滚屏
//       "0x01" (Downward)		向下滚屏
//    b: Set Numbers of Row Scroll per Step	  每帧行数
//    c: Set Time Interval between Each Scroll Step	每帧间延时
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c)
{
unsigned char i,j;	

	Set_Partial_Display(0x00,0x00,Max_Row);

	switch(a)
	{
		case 0:
			for(i=0;i<(Max_Row+1);i+=b)
			{
				Set_Display_Offset(i+1);
				for(j=0;j<c;j++)
				{
					Delay_us(200);
				}
			}
			break;
		case 1:
			for(i=0;i<(Max_Row+1);i+=b)
			{
				Set_Display_Offset(Max_Row-i);
				for(j=0;j<c;j++)
				{
					Delay_us(200);
				}
			}
			break;
	}
	Set_Partial_Display(0x01,0x00,0x00);
}
  
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
 //清屏函数,清屏后,整个屏幕是黑的
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char Data)
{
unsigned char i,j;

	Set_Column_Address(0x00,0x77);
	Set_Row_Address(0x00,0x7F);
	Set_Write_RAM();

	for(i=0;i<128;i++)
	{
		for(j=0;j<120;j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
		    OLED_WR_Byte(Data,OLED_DATA); 
		}
	}
}
 


 //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  www.lcdsoc.com 201505
//	  Data:取值为 0x11，0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff, ff最亮
//    a: Line Width		线宽
//    b: Column Address of Start 开始列
//    c: Column Address of End	 结束列
//    d: Row Address of Start	 开始行
//    e: Row Address of End		 结束行
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
unsigned char i,j,k,l,b1,c1;
	b1=b/4;
	c1=c/4 ;
	k=a%4;
	if(k == 0)
	{
		l=(a/4)-1;
	}
	else
	{
		l=a/4;
	}

	Set_Column_Address(Shift+b1,Shift+c1);
	Set_Row_Address(d,(d+a-1));
	Set_Write_RAM();
	for(i=0;i<(c1-b1+1);i++)
	{
		for(j=0;j<a;j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
			OLED_WR_Byte(Data,OLED_DATA); 
		}
	}

	Set_Column_Address(Shift+(c1-l),Shift+c1);
	Set_Row_Address(d+a,e-a);
	Set_Write_RAM();
	for(i=0;i<(e-d+1);i++)
	{
		for(j=0;j<(l+1);j++)
		{
			if(j == 0)
			{
				switch(k)
				{
					case 0:
						OLED_WR_Byte(Data,OLED_DATA);
						OLED_WR_Byte(Data,OLED_DATA);
						break;
					case 1:
						OLED_WR_Byte(0x00,OLED_DATA);
						OLED_WR_Byte((Data&0x0F),OLED_DATA);
						 
						break;
					case 2:
					    OLED_WR_Byte(0x00,OLED_DATA);
						OLED_WR_Byte(Data,OLED_DATA);
					
						break;
					case 3:
						OLED_WR_Byte((Data&0x0F),OLED_DATA);
						OLED_WR_Byte(Data,OLED_DATA);
						 
						break;
				}
			}
			else
			{
						OLED_WR_Byte(Data,OLED_DATA);
		             	OLED_WR_Byte(Data,OLED_DATA); 
			}
		}
	}
   

	Set_Column_Address(Shift+b1,Shift+c1);
	Set_Row_Address((e-a+1),e);
	Set_Write_RAM();
	for(i=0;i<(c1-b1+1);i++)
	{
		for(j=0;j<a;j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
			OLED_WR_Byte(Data,OLED_DATA); 
		 
		}
	}
 
	Set_Column_Address(Shift+b1,Shift+(b1+l));
	Set_Row_Address(d+a,e-a);
	Set_Write_RAM();
	for(i=0;i<(e-d+1);i++)
	{
		for(j=0;j<(l+1);j++)
		{
			if(j == l)
			{
				switch(k)
				{
					case 0:
							OLED_WR_Byte(Data,OLED_DATA);
		                 	OLED_WR_Byte(Data,OLED_DATA); 
						break;
					case 1:
						
						OLED_WR_Byte((Data&0xF0),OLED_DATA);
						OLED_WR_Byte(0x00,OLED_DATA);
						 
						break;
					case 2:
						OLED_WR_Byte(Data,OLED_DATA);
						 OLED_WR_Byte(0x00,OLED_DATA);
					 
						break;
					case 3:
						
						OLED_WR_Byte(Data,OLED_DATA);
						OLED_WR_Byte((Data&0xF0),OLED_DATA);
				 
						break;
				}
			}
			else
			{
					OLED_WR_Byte(Data,OLED_DATA);
		         	OLED_WR_Byte(Data,OLED_DATA); 
			}
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start
//    d: Row Address of End
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char i,j;
	
	Set_Column_Address(Shift+a,Shift+b);
	Set_Row_Address(c,d);
	Set_Write_RAM();

	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			OLED_WR_Byte(Data,OLED_DATA);
		    OLED_WR_Byte(Data,OLED_DATA); 
		}
	}
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start
//    d: Row Address of End
// 灰度模式下显示一副图片
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
			 
unsigned char *Src_Pointer;
unsigned char i,j;
 
  //取模时候像素正序	（不能反序与2.7不同）
    Src_Pointer=Data_Pointer;
	Set_Column_Address(Shift+a,Shift+b);
	Set_Row_Address(c,d);
	Set_Write_RAM();
  	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			OLED_WR_Byte(*Src_Pointer,OLED_DATA);
			Src_Pointer++;
			OLED_WR_Byte(*Src_Pointer,OLED_DATA);
			Src_Pointer++;
		}
	}

 }

/**************************************
  数据转换程序：将2位分成1个字节存入显存，由于1个seg表示4个列所以要同时写2个字节即4个像素
  uchar DATA：取模来的字模数据
****************************************/
void Con_4_byte(unsigned char DATA)
{
   unsigned char d1_4byte[4],d2_4byte[4];
   unsigned char i;
   unsigned char d,k1,k2;
   d=DATA;
 
  for(i=0;i<2;i++)   // 一两位的方式写入  2*4=8位
   {
     k1=d&0xc0;     //当i=0时 为D7,D6位 当i=1时 为D5,D4位

     /****有4种可能，16级灰度,一个字节数据表示两个像素，一个像素对应一个字节的4位***/

     switch(k1)
       {
	 case 0x00:
           d1_4byte[i]=0x00;
		   
         break;
     case 0x40:  // 0100,0000
           d1_4byte[i]=0x0f;
		   
         break;	
	 case 0x80:  //1000,0000
           d1_4byte[i]=0xf0;
		   
         break;
     case 0xc0:   //1100,0000
           d1_4byte[i]=0xff;
		  
         break;	 
     default:
      	 break;
	   }
     
	   d=d<<2;
	  k2=d&0xc0;     //当i=0时 为D7,D6位 当i=1时 为D5,D4位

     /****有4种可能，16级灰度,一个字节数据表示两个像素，一个像素对应一个字节的4位***/

     switch(k2)
       {
	 case 0x00:
           d2_4byte[i]=0x00;
		   
         break;
     case 0x40:  // 0100,0000
           d2_4byte[i]=0x0f;
		   
         break;	
	 case 0x80:  //1000,0000
           d2_4byte[i]=0xf0;
		 
         break;
     case 0xc0:   //1100,0000
           d2_4byte[i]=0xff;
		  
         break;	 
     default:
      	 break;
	   }
	  
	  d=d<<2;                                //左移两位
      
	 OLED_WR_Byte(d1_4byte[i],OLED_DATA);	    //写前2列
	 OLED_WR_Byte(d2_4byte[i],OLED_DATA);               //写后2列	  共计4列
   }

}
 
/***************************************************************
//  显示12*12点阵汉字 2015-05晶奥测试通过
//  取模方式为：横向取模左高位,数据排列:从左到右从上到下    16列 12行 
//   num：汉字在字库中的位置
//   x: Start Column  开始列 范围 0~（256-16）
//   y: Start Row   开始行 0~63 
***************************************************************/
void HZ12_12( unsigned char x, unsigned char y, unsigned char num)
{
	unsigned char x1,j ;
	x1=x/4; 
	Set_Column_Address(Shift+x1,Shift+x1+3); // 设置列坐标，shift为列偏移量由1322决定。3为16/4-1
	Set_Row_Address(y,y+11); 
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<24;j++)
	{
		 Con_4_byte(HZ12X12_S[num*24+j]);
	}

}

//****************************************************
//   写入一串12*12汉字 www.lcdsoc.com	x坐标和字间距d要为4的倍数
//    num1,num2：汉字在字库中的位置	 从num1显示到num2
//    x: Start Column  开始列 范围 0~（255-16）
//    y: Start Row    开始行 0~63
//    d:字间距离 0为无间距 
//*****************************************************
void Show_HZ12_12(unsigned char  x,unsigned char  y, unsigned char  d,unsigned char num1,unsigned char num2)
{
  unsigned char  i,d1;
  d1=d+16;
  for(i=num1;i<num2+1;i++)
  {
  HZ12_12(x,y,i);
  x=x+d1;             
  }
}

/***************************************************************
//  显示16*16点阵汉字 2015-05晶奥测试通过
//  取模方式为：横向取模左高位,数据排列:从左到右从上到下    16列 16行 
//   num：汉字在字库中的位置
//   x: Start Column  开始列 范围 0~（256-16）
//   y: Start Row   开始行 0~63 
***************************************************************/
void HZ16_16( unsigned char x, unsigned char y, unsigned char num)
{
	unsigned char x1,j ;
	x1=x/4; 
	Set_Column_Address(Shift+x1,Shift+x1+3); // 设置列坐标，shift为列偏移量由1322决定。3为16/4-1
	Set_Row_Address(y,y+15); 
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<32;j++)
	{
		 Con_4_byte(HZ16X16_S[num*32+j]);
	}
  
		
}	

//****************************************************
//   写入一串16*16汉字 www.lcdsoc.com
//    num1,num2：汉字在字库中的位置	 从num1显示到num2
//    x: Start Column  开始列 范围 0~（255-16）
//    y: Start Row    开始行 0~63
//    d:字间距离 0为无间距 
//x坐标和字间距d要为4的倍数
//*****************************************************
void Show_HZ16_16(unsigned char  x,unsigned char  y, unsigned char  d,unsigned char num1,unsigned char num2)
{
  unsigned char  i,d1;
  d1=d+16;
  for(i=num1;i<num2+1;i++)
  {
  HZ16_16(x,y,i);
  x=x+d1;             
  }
}

/***************************************************************
//  显示24*24点阵汉字 2015-05晶奥测试通过
//  取模方式为：横向取模左高位,数据排列:从左到右从上到下    24列 24行 
//   num：汉字在字库中的位置
//   x: Start Column  开始列 范围 0~（255-16）
//   y: Start Row   开始行 0~63 
***************************************************************/
void HZ24_24( unsigned char x, unsigned char y, unsigned char num)
{
	unsigned char x1,j ;
	x1=x/4; 
	Set_Column_Address(Shift+x1,Shift+x1+5); // 设置列坐标，shift为列偏移量由1322决定。3为16/4-1
	Set_Row_Address(y,y+23); 
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<72;j++)
	{
		 Con_4_byte(HZ24X24_S[num*72+j]);	//宋体24*24 ····如需要楷体24*24（HZ24X24_K.h） ,黑体24*24（HZ24X24_H.h）包含相应的头文件并修改此处数组名
	}
 		
}	

//****************************************************
//   写入一串24*24汉字 www.lcdsoc.com
//    num1,num2：汉字在字库中的位置	 从num1显示到num2
//    x: Start Column  开始列 范围 0~（255-16）
//    y: Start Row    开始行 0~63
//    d:字间距离 0为无间距 
//    x坐标和字间距d要为4的倍数
//*****************************************************
void Show_HZ24_24(unsigned char  x,unsigned char  y, unsigned char  d,unsigned char num1,unsigned char num2)
{
  unsigned char  i,d1;
  d1=d+24;
  for(i=num1;i<num2+1;i++)
  {
  HZ24_24(x,y,i);
  x=x+d1;             
  }
}

 //==============================================================
//功能描述：写入一组标准ASCII字符串	 5x8
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc5_8(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
    if(x1>61)
	   {x=0;
	   x1=x/4;
	   y=y+8;}  //换行																	
    Set_Column_Address(Shift+x1,Shift+x1+1); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+7); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<8;j++)
	 		  {
				 Con_4_byte(ASC5X8[c*8+j]);	//数据转换
			   }
	 i++;
	 x=x+8;	 //字间距，8为最小
  }
}
 //==============================================================
//功能描述：写入一组标准ASCII字符串	 6x12
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc6_12(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
    if(x1>61)
	   {x=0;
	   x1=x/4;
	   y=y+12;}  //换行																	
    Set_Column_Address(Shift+x1,Shift+x1+1); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+11); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<12;j++)
	 		  {
				 Con_4_byte(ASC6X12[c*12+j]);	//数据转换
			   }
	 i++;
	 x=x+8;	 //字间距，8为最小
  }
}
//==============================================================
//功能描述：写入一组标准ASCII字符串	 8x16
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc8_16(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
    if(x1>61)
	   {x=0;
	   x1=x/4;
	   y=y+16;}  //换行																	
    Set_Column_Address(Shift+x1,Shift+x1+1); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+15); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<16;j++)
	 		  {
				 Con_4_byte(ASC8X16[c*16+j]);	//数据转换
			   }
	 i++;
	 x=x+8;	 //字间距，8为最小
  }
}
 
//==============================================================
//功能描述：写入一组标准ASCII字符串	 12x24
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc12_24(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
    if(x1>59)
	   {x=0;
	   x1=x/4;
	   y=y+24;}  //换行																	
    Set_Column_Address(Shift+x1,Shift+x1+3); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+23); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<48;j++)
	 		  {
				 Con_4_byte(ASC12X24[c*48+j]);	//数据转换
			   }
	 i++;
	 x=x+12;//字间距，12为最小	
  }
}

//==============================================================
//功能描述：写入一组标准ASCII字符串	 20x40	 256*64 只能显示一行
//参数：显示的位置（x,y），ch[]要显示的字符串
//返回：无
//==============================================================  
void Asc20_40(unsigned char x,unsigned char y,unsigned char ch[])
{
  unsigned char x1,c=0, i=0,j=0;      
  while (ch[i]!='\0')
  {    
    x1=x/4;
	c =ch[i]-32;
   /* if(x1>10)
	   {x=0;
	   x1=x/4;
	    }  //只能显示一行		*/														
    Set_Column_Address(Shift+x1,Shift+x1+5); // 设置列坐标，shift为列偏移量由1322决定 
	Set_Row_Address(y,y+39); 
	Set_Write_RAM();	 //	写显存    
  	
		for(j=0;j<120;j++)
	 		  {
				 Con_4_byte(ASC20X40[c*120+j]);	//数据转换
			   }
	 i++;
	 x=x+20;//字间距，20为最小	
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Gray Scale Bar (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Grayscale()
{
	// Level 16 => Column 1~16
		Fill_Block(0xFF,0x00,0x03,0x00,Max_Row);
   	// Level 15 => Column 17~32
		Fill_Block(0xEE,0x04,0x07,0x00,Max_Row);
  	// Level 14 => Column 33~48
		Fill_Block(0xDD,0x08,0x0B,0x00,Max_Row);
 	// Level 13 => Column 49~64
		Fill_Block(0xCC,0x0C,0x0F,0x00,Max_Row);
  	// Level 12 => Column 65~80
		Fill_Block(0xBB,0x10,0x13,0x00,Max_Row);
 	// Level 11 => Column 81~96
		Fill_Block(0xAA,0x14,0x17,0x00,Max_Row);
 	// Level 10 => Column 97~112
		Fill_Block(0x99,0x18,0x1B,0x00,Max_Row);
 	// Level 9 => Column 113~128
		Fill_Block(0x88,0x1C,0x1F,0x00,Max_Row);
 	// Level 8 => Column 129~144
		Fill_Block(0x77,0x20,0x23,0x00,Max_Row);
  	// Level 7 => Column 145~160
		Fill_Block(0x66,0x24,0x27,0x00,Max_Row);
  	// Level 6 => Column 161~176
		Fill_Block(0x55,0x28,0x2B,0x00,Max_Row);
 	// Level 5 => Column 177~192
		Fill_Block(0x44,0x2C,0x2F,0x00,Max_Row);
  	// Level 4 => Column 193~208
		Fill_Block(0x33,0x30,0x33,0x00,Max_Row);
  	// Level 3 => Column 209~224
		Fill_Block(0x22,0x34,0x37,0x00,Max_Row);
   	// Level 2 => Column 225~240
		Fill_Block(0x11,0x38,0x3B,0x00,Max_Row);
		// Level 1 => Column 241~256
		Fill_Block(0x00,0x3C,Max_Column,0x00,Max_Row);
}
void Delay_us(int time)
{    
   int i;  
   while(time--)
   {
      i=8;  //自己定义  调试时间：20141116
      while(i--){};
        __nop();
        __nop();
        __nop();
        __nop();
   }
}










