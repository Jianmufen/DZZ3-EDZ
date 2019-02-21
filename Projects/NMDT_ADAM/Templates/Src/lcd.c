#include "lcd.h"
#include "lcdfont.h"
#include "rtc.h"
#include "stdio.h"
#include "main.h"

//static void GET_RTC_Time(void);
//static char disp_buf[128];//显示缓存
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
uint8_t OLED_GRAM[132][8];   //定义一个数组  先写数组  再把数组写到LCD
int8_t number=0;
int8_t number1=0;
int8_t number2=0;
int8_t number3=0;
int8_t number4=0;
int8_t number5=0;

 int8_t a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0,a8=0;
 int8_t b1=0,b2=0,b3=0,b4=0,b5=0,b6=0,b7=0;
 int8_t c1=0,c2=0,c3=0,c4=0,c5=0,c6=0,c7=0;
 int8_t d4=0,e4=0;
 
char disp_buf[128];
///* RTC Time*/
//static RTC_TimeTypeDef Time;
//static RTC_DateTypeDef Date;

///**************************实现函数******************************************** 
//*函数原型:		void GET_RTC_Time(void) 
//*功　　能:		得到RTC的时间
//*******************************************************************************/ 
//void GET_RTC_Time(void)
//{
//	RTC_DateTypeDef sdatestructureget;
//  RTC_TimeTypeDef stimestructureget;
//  /* 获取当前时间 */
//  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
//  /* 获取当前日期 */
//  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
//	
//}



//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_W_Command (0xb0+i);    //设置页地址（0~7）
		OLED_W_Command (0x00);      //设置显示位置—列低地址
		OLED_W_Command (0x10);      //设置显示位置—列高地址   
		for(n=0;n<132;n++)
		OLED_W_Data(OLED_GRAM[n][i]); 
	}   
}

/**************************实现函数******************************************** 
*函数原型:		unsigned char OLED_W_Command(void); 
*功　　能:		写命令到OLED上 
*******************************************************************************/ 
void OLED_W_Command(unsigned char com)
{
	unsigned char j;
	OLED_CS(1);
  for(j=0;j<3;j++);
	OLED_DC(0);
	OLED_Read(1);
	OLED_Write(0);
	com=com&0xFF;
	DATAOUT(com);
  OLED_CS(0);
	for(j=0;j<3;j++);
	OLED_CS(1);
	
} 	    

/**************************实现函数******************************************** 
*函数原型:		void OLED_W_Data(unsigned dat); 
*功　　能:		写数据到oled显示屏。 
*******************************************************************************/ 
void OLED_W_Data(unsigned char dat)
{
	unsigned char j;
	OLED_CS(1);
	for(j=0;j<3;j++);
	OLED_DC(1);
	OLED_Read(1);
	OLED_Write(0);
	dat=dat&0xff;
	DATAOUT(dat)
	OLED_CS(0);
	for(j=0;j<3;j++);
	OLED_CS(1);
} 	    


/**************************实现函数******************************************** 
*函数原型:		void OLED_Clear(void) 
*功　　能:		清屏 
*******************************************************************************/ 
void OLED_Clear(void) 
{ 
	uint8_t i,n; 
	for(i=0;i<8;i++) 
     for(n=0;n<132;n++)
        OLED_GRAM[n][i]=0X00;
   	OLED_Refresh_Gram();//更新显示
} 


/**************************实现函数******************************************** 
*函数原型:		void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t) 
*功　　能:		在x列 ，y行画一个点
*入口参数：x:0~~127；y:0~~63;t:1填充，0清空
*******************************************************************************/ 
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t) 
{ 
	uint8_t pos,bx,temp=0;
	x=x&0x7f;   //参数过滤  避免x>127  y>63
	y=y&0x3f;
	pos=y/8;   //对应在OLED_GRAM中的页
	bx=y%8;
	temp=1<<(7-bx);
	if(t)
		OLED_GRAM[x][pos]|=temp;
	else 
		OLED_GRAM[x][pos]&=~temp;	    
} 


/**************************实现函数******************************************** 
*函数原型:		void OLED_Char(uint8_t x,uint8_t y,unsigned char chr)
*功　　能:		在定点写一个8*16的ASCII码
*入口参数:x列(0<x<127)  y页（0<y<63）  i=1正常显示，i=0反显
*******************************************************************************/ 
void OLED_Char(unsigned char x,unsigned char y,unsigned char chr,uint8_t i)
{
	uint8_t t,temp,t1;
	uint8_t y0=y;
	x=x&0x7f;   //参数过滤  避免x>127  y>63
	y=y&0x3f;
	chr=chr-' ';//得到偏移后的值
	for(t=0;t<16;t++)             
	{
		temp=asc2_1608[chr][t];
		for(t1=0;t1<8;t1++)
		{
			if(i==1)
			{
			if(temp&0x01)
				OLED_DrawPoint(x,y,1);
			else 
				OLED_DrawPoint(x,y,0);
			temp>>=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}
			else
			{
				if(temp&0x01)
				OLED_DrawPoint(x,y,0);
			else 
				OLED_DrawPoint(x,y,1);
			temp>>=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
				
			}

	}
} 	    
}

/**************************实现函数******************************************** 
*函数原型:		void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p)
*功　　能:		显示字符串
*入口参数:x列(0<x<127)  y页（0<y<63）    i=1正常显示，i=0反显
*******************************************************************************/ 
void OLED_ShowString(unsigned char x,unsigned char y,const char *p,uint8_t i)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>120)
        {
					x=0;
					y+=16;
				}
        if(y>48)
        {
				  y=x=0;
					OLED_Clear();
				}
        OLED_Char(x,y,*p,i);	 
        x+=8;
        p++;
    }  
	
}	   


/**************************实现函数******************************************** 
*函数原型:		void OLED_China(uint8_t x,uint8_t y,unsigned char cha)
*功　　能:		在定点写一个16*16的汉字
*入口参数:x列(0<x<127)  y页（0<y<63）    i=1正常显示，i=0反显
*******************************************************************************/ 
void OLED_China(unsigned char x,unsigned char y,unsigned char n,uint8_t i)
{
	uint8_t t,t1,temp;
	uint8_t y0=y;
	x=x&0x7f;   //参数过滤  避免x>127  y>63
	y=y&0x3f;
//	chr=chr-'便';//得到偏移后的值
	for(t=0;t<32;t++)             
	{
		temp=china_1616[n][t];
		for(t1=0;t1<8;t1++)    //画点函数8个点一画
		{
			if(i==1)
			{
			  if(temp&0x01)
				  OLED_DrawPoint(x,y,1);
			  else 
				  OLED_DrawPoint(x,y,0);
			  temp>>=1;
			  y++;
			  if((y-y0)==16)
			  {
				  y=y0;
				  x++;
				  break;
			  }
		  }
			else
			{
				if(temp&0x01)
				  OLED_DrawPoint(x,y,0);
			  else 
			 	  OLED_DrawPoint(x,y,1);
			  temp>>=1;
			  y++;
			  if((y-y0)==16)
			  {
				  y=y0;
				  x++;
				  break;
		  	}
			}
		}
	}
} 	    

/**************************实现函数******************************************** 
*函数原型:		void OLED_China(uint8_t x,uint8_t y,uint8_t i)
*功　　能:		在定点写一个24*24的汉字
*入口参数:x列(0<x<127)  y页（0<y<63）
*******************************************************************************/ 
void OLED_China1(uint8_t x,uint8_t y,uint8_t i)
{
	uint8_t t,t1,temp;
	uint8_t y0=y;
	x=x&0x7f;   //参数过滤  避免x>127  y>63
	y=y&0x3f;
//	chr=chr-'便';//得到偏移后的值
	for(t=0;t<72;t++)             
	{
		temp=china_2424[i][t];
		for(t1=0;t1<8;t1++)    //画点函数8个点一画
		{
			if(temp&0x01)
				OLED_DrawPoint(x,y,1);
			else 
				OLED_DrawPoint(x,y,0);
			temp>>=1;
			y++;
			if((y-y0)==24)
			{
				y=y0;
				x++;
				break;
			}
		}
	}
} 	    


/**************************实现函数******************************************** 
*函数原型:		void OLED_Huatu(uint8_t x,uint8_t y,uint8_t i)
*功　　能:		在定点画一个图形
*入口参数:x列(0<x<127)  y页（0<y<63）
*******************************************************************************/ 
void OLED_Huatu(uint8_t x,uint8_t y,uint8_t i)
{
	uint8_t t,t1,temp;
	uint8_t y0=y;
	x=x&0x7f;   //参数过滤  避免x>127  y>63
	y=y&0x3f;
//	chr=chr-'便';//得到偏移后的值
	for(t=0;t<64;t++)             
	{
		temp=DianLiang[i][t];
		for(t1=0;t1<8;t1++)    //画点函数8个点一画
		{
			if(temp&0x01)
				OLED_DrawPoint(x,y,1);
			else 
				OLED_DrawPoint(x,y,0);
			temp>>=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}
	}
} 	    

/**************************实现函数******************************************** 
*函数原型:		void OLED_OpenMenu(void)
*功　　能:		开机菜单
*入口参数:无
*******************************************************************************/ 
void OLED_OpenMenu(void)
{
	  OLED_China1(17,8,0);
		OLED_China1(41,8,1);
		OLED_China1(65,8,2);
		OLED_China1(89,8,3);
	
	  OLED_China(1,40,0,1);
		OLED_China(17,40,1,1);
		OLED_China(33,40,2,1);
		OLED_China(49,40,3,1);
		OLED_China(65,40,4,1);
		OLED_China(81,40,5,1);
		OLED_China(97,40,6,1);
		OLED_China(113,40,7,1);
	
		OLED_Refresh_Gram();		//更新显示到OLED 
} 	    


/**************************实现函数******************************************** 
*函数原型:		void OLED_ZhuMenu(void)
*功　　能:		主菜单
*入口参数:无number
*******************************************************************************/ 
void OLED_ZhuMenu(void)
{
	OLED_China(17,0,8,1);//年
  OLED_China(49,0,9,1);//月
  OLED_China(81,0,10,1);//日
	OLED_ShowString(104,0,"///",1);

	OLED_ShowString(8,16,"00",1);//时
	OLED_Char(24,16,':',1);
	OLED_ShowString(32,16,"00",1);//分
	OLED_Char(48,16,':',1);
	OLED_ShowString(56,16,"00",1);//秒
	OLED_ShowString(96,16,"S00M",1);
	
	if(number==0)
	{
	OLED_China(48,32,11,1);//平均
	OLED_China(64,32,12,1);
	
	OLED_China(16,48,13,0);//瞬时
	OLED_China(32,48,14,0);
	OLED_China(80,48,15,1);//设置
	OLED_China(96,48,16,1);
	}
	else if(number==1)
	{
		OLED_China(48,32,11,1);//平均
	OLED_China(64,32,12,1);
	
	OLED_China(16,48,13,1);//瞬时
	OLED_China(32,48,14,1);
	OLED_China(80,48,15,0);//设置
	OLED_China(96,48,16,0);
	}
	else if(number==2)
	{
		OLED_China(48,32,11,0);//平均
	OLED_China(64,32,12,0);
	
	OLED_China(16,48,13,1);//瞬时
	OLED_China(32,48,14,1);
	OLED_China(80,48,15,1);//设置
	OLED_China(96,48,16,1);
	}
	
	OLED_Refresh_Gram();		//更新显示到OLED 	
} 	    

/**************************实现函数******************************************** 
*函数原型:		void OLED_DataMenu(void)
*功　　能:		温度菜单
*入口参数:无number1
*******************************************************************************/ 
void OLED_DataMenu(void)
{
	if(number1==0)
	{
	OLED_China(1,16,17,1);//气
	OLED_China(17,16,18,1);//温
	OLED_Char(33,16,':',1);
	OLED_ShowString(40,16,"     //./",1);
	OLED_China(113,16,61,1);//℃
	
	OLED_China(1,32,19,1);//湿
	OLED_China(17,32,20,1);//度
	OLED_Char(33,32,':',1);
	OLED_ShowString(40,32,"    /// %RH",1);
	
	OLED_China(1,48,17,1);//气
	OLED_China(17,48,21,1);//压
	OLED_Char(33,48,':',1);
	OLED_ShowString(40,48,"  ////./hpa",1);
	}
	else if(number1==1)
	{
		OLED_China(1,16,22,1);//阵
	OLED_China(17,16,23,1);//风
	OLED_China(33,16,23,1);//风
	OLED_China(49,16,24,1);//向
	OLED_Char(65,16,':',1);
	OLED_ShowString(73,16,"  ///",1);
	OLED_China(113,16,62,1);//°
	
  OLED_China(1,32,22,1);//阵
	OLED_China(17,32,23,1);//风
	OLED_China(33,32,23,1);//风
	OLED_China(49,32,25,1);//速
	OLED_Char(65,32,':',1);
	OLED_ShowString(72,32,"//./m/s",1);
	
	OLED_China(1,48,26,1);//地
	OLED_ShowString(17,48,"    ",1);
	OLED_China(49,48,18,1);//温
	OLED_Char(65,48,':',1);
	OLED_ShowString(73,48," //./",1);
	OLED_China(113,48,61,1);//℃
	}
	else if(number1==2)
	{
		OLED_China(1,16,27,1);//分
	OLED_China(17,16,28,1);//雨
	OLED_China(33,16,29,1);//量
	OLED_Char(49,16,':',1);
	OLED_ShowString(56,16,"    0.0mm",1);

  OLED_China(1,32,14,1);//时
	OLED_China(17,32,28,1);//雨
	OLED_China(33,32,29,1);//量
	OLED_Char(49,32,':',1);
	OLED_ShowString(56,32,"    0.0mm",1);
	
	OLED_China(1,48,10,1);//日
	OLED_China(17,48,28,1);//雨
	OLED_China(33,48,29,1);//量
	OLED_Char(49,48,':',1);
	OLED_ShowString(56,48,"    0.0mm",1);
	}
	
	OLED_Refresh_Gram();		//更新显示到OLED 
} 	    

/**************************实现函数******************************************** 
*函数原型:		void OLED_AverageMenu(void)
*功　　能:		平均菜单
*入口参数:无
*******************************************************************************/ 
void OLED_AverageMenu(void)
{
	OLED_China(1,0,59,1);//二
	OLED_China(17,0,27,1);//分
	OLED_China(33,0,23,1);//风
	OLED_China(49,0,24,1);//向
	OLED_Char(65,0,':',1);
	OLED_ShowString(88,0,"///",1);
	OLED_China(112,0,62,1);//°

	
  OLED_China(1,16,59,1);//二
	OLED_China(17,16,27,1);//分
	OLED_China(33,16,23,1);//风
	OLED_China(49,16,25,1);//速
	OLED_Char(65,16,':',1);
	OLED_ShowString(72,16,"//./m/s",1);
	
	
	OLED_China(1,32,60,1);//十
	OLED_China(17,32,27,1);//分
	OLED_China(33,32,23,1);//风
	OLED_China(49,32,24,1);//向
	OLED_Char(65,32,':',1);
	OLED_ShowString(88,32,"///",1);
	OLED_China(112,32,62,1);//°
	
	OLED_China(1,48,60,1);//十
	OLED_China(17,48,27,1);//分
	OLED_China(33,48,23,1);//风
	OLED_China(49,48,25,1);//速
	OLED_Char(65,48,':',1);
	OLED_ShowString(72,48,"//./m/s",1);
	
	OLED_Refresh_Gram();		//更新显示到OLED 
} 	    


/**************************实现函数******************************************** 
*函数原型:		void OLED_SheZhiMenu(void)
*功　　能:		平均菜单
*入口参数:number5
*******************************************************************************/ 
void OLED_SheZhiMenu(void)
{
	if(number5==0)
	{
		OLED_China(1,0,30,0);//工
	OLED_China(17,0,31,0);//作
	OLED_China(33,0,32,0);//方
	OLED_China(49,0,2,0);//式
	OLED_ShowString(65,0,"...",1);

  OLED_China(1,16,14,1);//时
	OLED_China(17,16,40,1);//钟
	OLED_China(33,16,15,1);//设
	OLED_China(49,16,16,1);//置
	OLED_ShowString(65,16,"...",1);
	
	OLED_China(1,32,42,1);//硬
	OLED_China(17,32,43,1);//件
	OLED_China(33,32,15,1);//设
	OLED_China(49,32,16,1);//置
	OLED_ShowString(65,32,"...",1);
	
	OLED_China(1,48,55,1);//系
	OLED_China(17,48,56,1);//统
	OLED_China(33,48,57,1);//参
	OLED_China(49,48,58,1);//数
	OLED_ShowString(65,48," ***",1);
	}
	else if(number5==1)
	{
		OLED_China(1,0,30,1);//工
	OLED_China(17,0,31,1);//作
	OLED_China(33,0,32,1);//方
	OLED_China(49,0,2,1);//式
	OLED_ShowString(65,1,"...",1);

  OLED_China(1,16,14,0);//时
	OLED_China(17,16,40,0);//钟
	OLED_China(33,16,15,0);//设
	OLED_China(49,16,16,0);//置
	OLED_ShowString(65,16,"...",1);
	
	OLED_China(1,32,42,1);//硬
	OLED_China(17,32,43,1);//件
	OLED_China(33,32,15,1);//设
	OLED_China(49,32,16,1);//置
	OLED_ShowString(65,32,"...",1);
	
	OLED_China(1,48,55,1);//系
	OLED_China(17,48,56,1);//统
	OLED_China(33,48,57,1);//参
	OLED_China(49,48,58,1);//数
	OLED_ShowString(65,48," ***",1);
	}
	else if(number5==2)
	{
		OLED_China(1,0,30,1);//工
	OLED_China(17,0,31,1);//作
	OLED_China(33,0,32,1);//方
	OLED_China(49,0,2,1);//式
	OLED_ShowString(65,0,"...",1);

  OLED_China(1,16,14,1);//时
	OLED_China(17,16,40,1);//钟
	OLED_China(33,16,15,1);//设
	OLED_China(49,16,16,1);//置
	OLED_ShowString(65,16,"...",1);
	
	OLED_China(1,32,42,0);//硬
	OLED_China(17,32,43,0);//件
	OLED_China(33,32,15,0);//设
	OLED_China(49,32,16,0);//置
	OLED_ShowString(65,32,"...",1);
	
	OLED_China(1,48,55,1);//系
	OLED_China(17,48,56,1);//统
	OLED_China(33,48,57,1);//参
	OLED_China(49,48,58,1);//数
	OLED_ShowString(65,48," ***",1);
	}
	else if(number5==3)
	{
		OLED_China(1,0,30,1);//工
	OLED_China(17,0,31,1);//作
	OLED_China(33,0,32,1);//方
	OLED_China(49,0,2,1);//式
	OLED_ShowString(65,0,"...",1);

  OLED_China(1,16,14,1);//时
	OLED_China(17,16,40,1);//钟
	OLED_China(33,16,15,1);//设
	OLED_China(49,16,16,1);//置
	OLED_ShowString(65,16,"...",1);
	
	OLED_China(1,32,42,1);//硬
	OLED_China(17,32,43,1);//件
	OLED_China(33,32,15,1);//设
	OLED_China(49,32,16,1);//置
	OLED_ShowString(65,32,"...",1);
	
	OLED_China(1,48,55,0);//系
	OLED_China(17,48,56,0);//统
	OLED_China(33,48,57,0);//参
	OLED_China(49,48,58,0);//数
	OLED_ShowString(65,48," ***",1);
	}
	OLED_Refresh_Gram();		//更新显示到OLED 
}
/**************************实现函数******************************************** 
*函数原型:		void OLED_YingJianMenu(void)
*功　　能:		硬件菜单
*入口参数:number2
*******************************************************************************/ 
void OLED_YingJianMenu(void)
{
	if(number2==0)
	{
  OLED_China(1,0,44,0);//开
	OLED_China(17,0,45,0);//始
	OLED_China(33,0,42,0);//硬
	OLED_China(49,0,46,0);//铁
	OLED_China(65,0,47,0);//补
	OLED_China(81,0,48,0);//偿
	
	OLED_China(1,16,49,1);//结
	OLED_China(17,16,50,1);//束
	OLED_China(33,16,42,1);//硬
	OLED_China(49,16,46,1);//铁
	OLED_China(65,16,47,1);//补
	OLED_China(81,16,48,1);//偿
	
	OLED_China(1,32,51,1);//正
	OLED_China(17,32,52,1);//北
	OLED_China(33,32,53,1);//校
	OLED_China(49,32,51,1);//正
	OLED_ShowString(65,32," ***",1);
	
	OLED_China(1,48,54,1);//清
	OLED_ShowString(17,48,"PAMS",1);
	OLED_China(49,48,35,1);//存
	OLED_China(65,48,36,1);//储
	OLED_China(81,48,37,1);//区
	OLED_ShowString(97,48," ***",1);
}
	if(number2==1)
	{
  OLED_China(1,0,44,1);//开
	OLED_China(17,0,45,1);//始
	OLED_China(33,0,42,1);//硬
	OLED_China(49,0,46,1);//铁
	OLED_China(65,0,47,1);//补
	OLED_China(81,0,48,1);//偿
	
	OLED_China(1,16,49,0);//结
	OLED_China(17,16,50,0);//束
	OLED_China(33,16,42,0);//硬
	OLED_China(49,16,46,0);//铁
	OLED_China(65,16,47,0);//补
	OLED_China(81,16,48,0);//偿
	
	OLED_China(1,32,51,1);//正
	OLED_China(17,32,52,1);//北
	OLED_China(33,32,53,1);//校
	OLED_China(49,32,51,1);//正
	OLED_ShowString(65,32," ***",1);
	
	OLED_China(1,48,54,1);//清
	OLED_ShowString(17,48,"PAMS",1);
	OLED_China(49,48,35,1);//存
	OLED_China(65,48,36,1);//储
	OLED_China(81,48,37,1);//区
	OLED_ShowString(97,48," ***",1);
}
	if(number2==2)
	{
  OLED_China(1,0,44,1);//开
	OLED_China(17,0,45,1);//始
	OLED_China(33,0,42,1);//硬
	OLED_China(49,0,46,1);//铁
	OLED_China(65,0,47,1);//补
	OLED_China(81,0,48,1);//偿
	
	OLED_China(1,16,49,1);//结
	OLED_China(17,16,50,1);//束
	OLED_China(33,16,42,1);//硬
	OLED_China(49,16,46,1);//铁
	OLED_China(65,16,47,1);//补
	OLED_China(81,16,48,1);//偿
	
	OLED_China(1,32,51,0);//正
	OLED_China(17,32,52,0);//北
	OLED_China(33,32,53,0);//校
	OLED_China(49,32,51,0);//正
	OLED_ShowString(65,32," ***",1);
	
	OLED_China(1,48,54,1);//清
	OLED_ShowString(17,48,"PAMS",1);
	OLED_China(49,48,35,1);//存
	OLED_China(65,48,36,1);//储
	OLED_China(81,48,37,1);//区
	OLED_ShowString(97,48," ***",1);
}
	if(number2==3)
	{
  OLED_China(1,0,44,1);//开
	OLED_China(17,0,45,1);//始
	OLED_China(33,0,42,1);//硬
	OLED_China(49,0,46,1);//铁
	OLED_China(65,0,47,1);//补
	OLED_China(81,0,48,1);//偿
	
	OLED_China(1,16,49,1);//结
	OLED_China(17,16,50,1);//束
	OLED_China(33,16,42,1);//硬
	OLED_China(49,16,46,1);//铁
	OLED_China(65,16,47,1);//补
	OLED_China(81,16,48,1);//偿
	
	OLED_China(1,32,51,1);//正
	OLED_China(17,32,52,1);//北
	OLED_China(33,32,53,1);//校
	OLED_China(49,32,51,1);//正
	OLED_ShowString(65,32," ***",1);
	
	OLED_China(1,48,54,0);//清
	OLED_ShowString(17,48,"PAMS",0);
	OLED_China(49,48,35,0);//存
	OLED_China(65,48,36,0);//储
	OLED_China(81,48,37,0);//区
	OLED_ShowString(97,48," ***",1);
}


	OLED_Refresh_Gram();		//更新显示到OLED 
} 	    


/**************************实现函数******************************************** 
*函数原型:		void OLED_CunChuMenu(void)
*功　　能:		存储菜单
*入口参数:number3
*******************************************************************************/ 
void OLED_CunChuMenu(void)
{
	if(number3==0)
	{
  OLED_China(1,0,33,0);//本
	OLED_China(17,0,34,0);//机
	OLED_China(33,0,35,0);//存
	OLED_China(49,0,36,0);//储
	OLED_Char(65,0,':',1);
	OLED_ShowString(104,0,"00M",1);
	
	OLED_ShowString(1,16,"PAMS",1);
	OLED_China(33,16,35,1);//存
	OLED_China(49,16,36,1);//储
	OLED_Char(65,16,':',1);
	OLED_ShowString(104,16,"//M",1);
	
	OLED_ShowString(1,32,"PAMS",1);
	OLED_China(33,32,3,1);//自
	OLED_China(49,32,4,1);//动
	OLED_China(65,32,38,1);//发
	OLED_China(81,32,39,1);//送
	OLED_Char(97,32,':',1);
	OLED_ShowString(104,32," //",1);
	}
	if(number3==1)
	{
  OLED_China(1,0,33,1);//本
	OLED_China(17,0,34,1);//机
	OLED_China(33,0,35,1);//存
	OLED_China(49,0,36,1);//储
	OLED_Char(65,0,':',1);
	OLED_ShowString(104,0,"00M",1);
	
	OLED_ShowString(1,16,"PAMS",0);
	OLED_China(33,16,35,0);//存
	OLED_China(49,16,36,0);//储
	OLED_Char(65,16,':',1);
	OLED_ShowString(104,16,"//M",1);
	
	OLED_ShowString(1,32,"PAMS",1);
	OLED_China(33,32,3,1);//自
	OLED_China(49,32,4,1);//动
	OLED_China(65,32,38,1);//发
	OLED_China(81,32,39,1);//送
	OLED_Char(97,32,':',1);
	OLED_ShowString(104,32," //",1);
	}
	if(number3==2)
	{
  OLED_China(1,0,33,1);//本
	OLED_China(17,0,34,1);//机
	OLED_China(33,0,35,1);//存
	OLED_China(49,0,36,1);//储
	OLED_Char(65,0,':',1);
	OLED_ShowString(104,0,"00M",1);
	
	OLED_ShowString(1,16,"PAMS",1);
	OLED_China(33,16,35,1);//存
	OLED_China(49,16,36,1);//储
	OLED_Char(65,16,':',1);
	OLED_ShowString(104,16,"//M",1);
	
	OLED_ShowString(1,32,"PAMS",0);
	OLED_China(33,32,3,0);//自
	OLED_China(49,32,4,0);//动
	OLED_China(65,32,38,0);//发
	OLED_China(81,32,39,0);//送
	OLED_Char(97,32,':',1);
	OLED_ShowString(104,32," //",1);
	}
	
	OLED_Refresh_Gram();		//更新显示到OLED 
} 

/**************************实现函数******************************************** 
*函数原型:		void OLED_DingZhengMenu(void)
*功　　能:		存储菜单
*入口参数:无number4
*******************************************************************************/ 
void OLED_DingZhengMenu(void)
{
	if(number4==0)
	{
		OLED_ShowString(64,0,": */./",1);
	OLED_China(1,0,17,0);//气温订正
	OLED_China(17,0,18,0);
	OLED_China(33,0,64,0);
	OLED_China(49,0,51,0);
	OLED_China(112,0,61,1);
	
	OLED_ShowString(64,16,": *//%RH",1);
	OLED_China(1,16,19,1);//湿度订正
	OLED_China(17,16,20,1);
	OLED_China(33,16,64,1);
	OLED_China(49,16,51,1);
	
	
	OLED_ShowString(64,32,":*/./m/s",1);
	OLED_China(1,32,23,1);//风速订正
	OLED_China(17,32,25,1);
	OLED_China(33,32,64,1);
	OLED_China(49,32,51,1);
	
	OLED_ShowString(64,48,": */./",1);
	OLED_China(1,48,26,1);//地温订正
	OLED_China(17,48,18,1);
	OLED_China(33,48,64,1);
	OLED_China(49,48,51,1);
	if((a1!=0)||(b1!=0)||(c1!=0))
	{
		if(a1==0)
				OLED_ShowString(80,0,"+",1);
			else if(a1==1)
				OLED_ShowString(80,0,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b1);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c1);
			OLED_ShowString(104,0,disp_buf,1);
	}
	if((a2!=0)||(b2!=0)||(c2!=0))
	{
		if(a2==0)
				OLED_ShowString(80,16,"+",1);
			else if(a2==1)
				OLED_ShowString(80,16,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b2);
			OLED_ShowString(88,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c2);
			OLED_ShowString(96,16,disp_buf,1);
	}
	if((a3!=0)||(b3!=0)||(c3!=0))
	{
		if(a3==0)
				OLED_ShowString(72,32,"+",1);
			else if(a3==1)
				OLED_ShowString(72,32,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b3);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c3);
			OLED_ShowString(96,32,disp_buf,1);
	}
	}
	else if(number4==1)
	{
		OLED_ShowString(64,0,": */./",1);
	OLED_China(1,0,17,1);//气温订正
	OLED_China(17,0,18,1);
	OLED_China(33,0,64,1);
	OLED_China(49,0,51,1);
	OLED_China(112,0,61,1);
	
	OLED_ShowString(64,16,": *//%RH",1);
	OLED_China(1,16,19,0);//湿度订正
	OLED_China(17,16,20,0);
	OLED_China(33,16,64,0);
	OLED_China(49,16,51,0);
	
	
	OLED_ShowString(64,32,":*/./m/s",1);
	OLED_China(1,32,23,1);//风速订正
	OLED_China(17,32,25,1);
	OLED_China(33,32,64,1);
	OLED_China(49,32,51,1);
	
	OLED_ShowString(64,48,": */./",1);
	OLED_China(1,48,26,1);//地温订正
	OLED_China(17,48,18,1);
	OLED_China(33,48,64,1);
	OLED_China(49,48,51,1);
	OLED_China(112,48,61,1);
	if((a1!=0)||(b1!=0)||(c1!=0))
	{
		if(a1==0)
				OLED_ShowString(80,0,"+",1);
			else if(a1==1)
				OLED_ShowString(80,0,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b1);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c1);
			OLED_ShowString(104,0,disp_buf,1);
	}
	if((a2!=0)||(b2!=0)||(c2!=0))
	{
		if(a2==0)
				OLED_ShowString(80,16,"+",1);
			else if(a2==1)
				OLED_ShowString(80,16,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b2);
			OLED_ShowString(88,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c2);
			OLED_ShowString(96,16,disp_buf,1);
	}
	if((a3!=0)||(b3!=0)||(c3!=0))
	{
		if(a3==0)
				OLED_ShowString(72,32,"+",1);
			else if(a3==1)
				OLED_ShowString(72,32,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b3);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c3);
			OLED_ShowString(96,32,disp_buf,1);
	}
	}
	else if(number4==2)
	{
		OLED_ShowString(64,0,": */./",1);
	OLED_China(1,0,17,1);//气温订正
	OLED_China(17,0,18,1);
	OLED_China(33,0,64,1);
	OLED_China(49,0,51,1);
	OLED_China(112,0,61,1);
	
	OLED_ShowString(64,16,": *//%RH",1);
	OLED_China(1,16,19,1);//湿度订正
	OLED_China(17,16,20,1);
	OLED_China(33,16,64,1);
	OLED_China(49,16,51,1);
	
	
	OLED_ShowString(64,32,":*/./m/s",1);
	OLED_China(1,32,23,0);//风速订正
	OLED_China(17,32,25,0);
	OLED_China(33,32,64,0);
	OLED_China(49,32,51,0);
	
	OLED_ShowString(64,48,": */./",1);
	OLED_China(1,48,26,1);//地温订正
	OLED_China(17,48,18,1);
	OLED_China(33,48,64,1);
	OLED_China(49,48,51,1);
	OLED_China(112,48,61,1);
	if((a1!=0)||(b1!=0)||(c1!=0))
	{
		if(a1==0)
				OLED_ShowString(80,0,"+",1);
			else if(a1==1)
				OLED_ShowString(80,0,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b1);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c1);
			OLED_ShowString(104,0,disp_buf,1);
	}
	if((a2!=0)||(b2!=0)||(c2!=0))
	{
		if(a2==0)
				OLED_ShowString(80,16,"+",1);
			else if(a2==1)
				OLED_ShowString(80,16,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b2);
			OLED_ShowString(88,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c2);
			OLED_ShowString(96,16,disp_buf,1);
	}
	if((a3!=0)||(b3!=0)||(c3!=0))
	{
		if(a3==0)
				OLED_ShowString(72,32,"+",1);
			else if(a3==1)
				OLED_ShowString(72,32,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b3);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c3);
			OLED_ShowString(96,32,disp_buf,1);
	}
	
	}
	else if(number4==3)
	{
		OLED_ShowString(64,0,": */./",1);
	OLED_China(1,0,17,1);//气温订正
	OLED_China(17,0,18,1);
	OLED_China(33,0,64,1);
	OLED_China(49,0,51,1);
	OLED_China(112,0,61,1);
	
	OLED_ShowString(64,16,": *//%RH",1);
	OLED_China(1,16,19,1);//湿度订正
	OLED_China(17,16,20,1);
	OLED_China(33,16,64,1);
	OLED_China(49,16,51,1);
	//OLED_China(112,16,61,1);
	
	OLED_ShowString(64,32,":*/./m/s",1);
	OLED_China(1,32,23,1);//风速订正
	OLED_China(17,32,25,1);
	OLED_China(33,32,64,1);
	OLED_China(49,32,51,1);
	
	OLED_ShowString(64,48,": */./",1);
	OLED_China(1,48,26,1);//地温订正
	OLED_China(17,48,18,0);
	OLED_China(33,48,64,0);
	OLED_China(49,48,51,0);
	OLED_China(112,48,61,0);
	
	
	}
	else if(number4==4)
	{
		OLED_ShowString(64,0,": //.///",1);
	OLED_China(1,0,17,0);//气压常数
	OLED_China(17,0,21,0);
	OLED_China(33,0,63,0);
	OLED_China(49,0,58,0);
	
	OLED_ShowString(64,16,":*/./hpa",1);
	OLED_China(1,16,17,1);//气压订正
	OLED_China(17,16,21,1);
	OLED_China(33,16,64,1);
	OLED_China(49,16,51,1);
	
	OLED_ShowString(64,32,"+//./hpa",1);
	OLED_China(1,32,63,1);//常数订正
	OLED_China(17,32,58,1);
	OLED_China(33,32,64,1);
	OLED_China(49,32,51,1);
	
	OLED_ShowString(64,48,":/.//hpa",1);
	OLED_ShowString(64,48,":/.//hpa",1);
	  OLED_China(1,48,18,1);//温度系数
	  OLED_China(17,48,20,1);
	  OLED_China(33,48,55,1);
	  OLED_China(49,48,58,1);
	if((a4!=0)||(b4!=0)||(c4!=0)||(d4!=0)||(e4!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
	}
	if((a5!=0)||(b5!=0)||(c5!=0))
	{
		if(a5==0)
				OLED_ShowString(72,16,"+",1);
			else if(a5==1)
				OLED_ShowString(72,16,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b5);
			OLED_ShowString(80,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c5);
			OLED_ShowString(96,16,disp_buf,1);
	}
	if((a6!=0)||(b6!=0)||(c6!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a6);
			OLED_ShowString(72,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b6);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c6);
			OLED_ShowString(96,32,disp_buf,1);
	}
	if((a7!=0)||(b7!=0)||(c7!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a7);
			OLED_ShowString(72,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b7);
			OLED_ShowString(88,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c7);
			OLED_ShowString(96,48,disp_buf,1);
	}
	}
	else if(number4==5)
	{
			OLED_ShowString(64,0,": //.///",1);
	OLED_China(1,0,17,1);//气压常数
	OLED_China(17,0,21,1);
	OLED_China(33,0,63,1);
	OLED_China(49,0,58,1);
	
	OLED_ShowString(64,16,":*/./hpa",1);
	OLED_China(1,16,17,0);//气压订正
	OLED_China(17,16,21,0);
	OLED_China(33,16,64,0);
	OLED_China(49,16,51,0);
	
	OLED_ShowString(64,32,"+//./hpa",1);
	OLED_China(1,32,63,1);//常数订正
	OLED_China(17,32,58,1);
	OLED_China(33,32,64,1);
	OLED_China(49,32,51,1);
	
	OLED_ShowString(64,48,":/.//hpa",1);
	OLED_China(1,48,18,1);//温度系数
	OLED_China(17,48,20,1);
	OLED_China(33,48,64,1);
	OLED_China(49,48,55,1);
	OLED_China(112,48,58,1);
	if((a4!=0)||(b4!=0)||(c4!=0)||(d4!=0)||(e4!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
	}
	if((a5!=0)||(b5!=0)||(c5!=0))
	{
		if(a5==0)
				OLED_ShowString(72,16,"+",1);
			else if(a5==1)
				OLED_ShowString(72,16,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b5);
			OLED_ShowString(80,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c5);
			OLED_ShowString(96,16,disp_buf,1);
	}
	if((a6!=0)||(b6!=0)||(c6!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a6);
			OLED_ShowString(72,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b6);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c6);
			OLED_ShowString(96,32,disp_buf,1);
	}
	if((a7!=0)||(b7!=0)||(c7!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a7);
			OLED_ShowString(72,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b7);
			OLED_ShowString(88,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c7);
			OLED_ShowString(96,48,disp_buf,1);
	}
	
	}
	else if(number4==6)
	{
			OLED_ShowString(64,0,": //.///",1);
	OLED_China(1,0,17,1);//气压常数
	OLED_China(17,0,21,1);
	OLED_China(33,0,63,1);
	OLED_China(49,0,58,1);
	
	OLED_ShowString(64,16,":*/./hpa",1);
	OLED_China(1,16,17,1);//气压订正
	OLED_China(17,16,21,1);
	OLED_China(33,16,64,1);
	OLED_China(49,16,51,1);
	
	OLED_ShowString(64,32,"+//./hpa",1);
	OLED_China(1,32,63,0);//常数订正
	OLED_China(17,32,58,0);
	OLED_China(33,32,64,0);
	OLED_China(49,32,51,0);
	
	OLED_ShowString(64,48,":/.//hpa",1);
	OLED_China(1,48,18,1);//温度系数
	OLED_China(17,48,20,1);
	OLED_China(33,48,64,1);
	OLED_China(49,48,55,1);
	OLED_China(112,48,58,1);
	if((a4!=0)||(b4!=0)||(c4!=0)||(d4!=0)||(e4!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
	}
	if((a5!=0)||(b5!=0)||(c5!=0))
	{
		if(a5==0)
				OLED_ShowString(72,16,"+",1);
			else if(a5==1)
				OLED_ShowString(72,16,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b5);
			OLED_ShowString(80,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c5);
			OLED_ShowString(96,16,disp_buf,1);
	}
	if((a6!=0)||(b6!=0)||(c6!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a6);
			OLED_ShowString(72,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b6);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c6);
			OLED_ShowString(96,32,disp_buf,1);
	}
	if((a7!=0)||(b7!=0)||(c7!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a7);
			OLED_ShowString(72,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b7);
			OLED_ShowString(88,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c7);
			OLED_ShowString(96,48,disp_buf,1);
	}

	}
	else if(number4==7)
	{
			OLED_ShowString(64,0,": //.///",1);
	OLED_China(1,0,17,1);//气压常数
	OLED_China(17,0,21,1);
	OLED_China(33,0,63,1);
	OLED_China(49,0,58,1);
	
	OLED_ShowString(64,16,":*/./hpa",1);
	OLED_China(1,16,17,1);//气压订正
	OLED_China(17,16,21,1);
	OLED_China(33,16,64,1);
	OLED_China(49,16,51,1);
	
	OLED_ShowString(64,32,"+//./hpa",1);
	OLED_China(1,32,63,1);//常数订正
	OLED_China(17,32,58,1);
	OLED_China(33,32,64,1);
	OLED_China(49,32,51,1);
	
	OLED_ShowString(64,48,":/.//hpa",1);
	OLED_China(1,48,18,0);//温度系数
	OLED_China(17,48,20,0);
	OLED_China(33,48,64,0);
	OLED_China(49,48,55,0);
	OLED_China(112,48,58,0);
	if((a4!=0)||(b4!=0)||(c4!=0)||(d4!=0)||(e4!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
	}
	if((a5!=0)||(b5!=0)||(c5!=0))
	{
		if(a5==0)
				OLED_ShowString(72,16,"+",1);
			else if(a5==1)
				OLED_ShowString(72,16,"-",1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b5);
			OLED_ShowString(80,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c5);
			OLED_ShowString(96,16,disp_buf,1);
	}
	if((a6!=0)||(b6!=0)||(c6!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a6);
			OLED_ShowString(72,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b6);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c6);
			OLED_ShowString(96,32,disp_buf,1);
	}
	if((a7!=0)||(b7!=0)||(c7!=0))
	{
		snprintf(disp_buf,sizeof(disp_buf),"%01u",a7);
			OLED_ShowString(72,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b7);
			OLED_ShowString(88,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c7);
			OLED_ShowString(96,48,disp_buf,1);
	}
	
	}
	OLED_Refresh_Gram();		//更新显示到OLED 
}

//初始化SSD1303				    
void OLED_Init(void)
{
	OLED_W_Command(0xae); //display off显示关
        OLED_W_Command(0x40); //display start line 00000显示开始行00000
	      OLED_W_Command(0xb0);
        OLED_W_Command(0x81); //contrast对比度
        OLED_W_Command(0xff);
        OLED_W_Command(0x82); //brightness亮度
        OLED_W_Command(0x20);
        OLED_W_Command(0xa0); //no remap不重画
        OLED_W_Command(0xa4); //intire display off全部显示关
        OLED_W_Command(0xa6); //normal display正常显示
        OLED_W_Command(0xa8); //39 mux
        OLED_W_Command(0x3f);
        OLED_W_Command(0xad); //DCDC off
        OLED_W_Command(0x8b);
        OLED_W_Command(0xc8); //scan from COM[N-1] to COM0从COM[N-1] 至 COM0扫描
        OLED_W_Command(0xd3); //row 0->com 62
        OLED_W_Command(0x00);
        OLED_W_Command(0xd5);    //
        OLED_W_Command(0x70);    //
        OLED_W_Command(0xd8); //mono mode,normal power mode mono模式,正常电源模式
        OLED_W_Command(0x00);  //?
        OLED_W_Command(0xda); //alternative COM pin configuration另一个com引脚配置
        OLED_W_Command(0x12);
        OLED_W_Command(0xaf); //display on显示开
	OLED_Clear();
	
}  



