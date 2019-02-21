/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    01-July-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "usart.h"
#include "rtc.h"
#include "pcf8563.h"
#include "myiic.h"
#include "gpio.h"
#include "lcd.h"
#include "lcdfont.h"
#include "at_iic.h"
#include "at24c512.h"
#include "key.h"
#include "string.h"
#include "stdlib.h"
/** @addtogroup STM32L1xx_HAL_Examples
  * @{
  */
_PCF8563_Time_Typedef PCF_DataStruct_Time;
_PCF8563_Date_Typedef PCF_DataStruct_Date;

RTC_DateTypeDef sdatestructureget;
RTC_TimeTypeDef stimestructureget;
/* PLL source select */
//#define PLL_CLOCK_SOURCE_HSI
#define PLL_CLOCK_SOURCE_HSE
#define UART_RX_BUF_SIZE  (128)                  /* receiving buffer size */
static char rx_buffer[UART_RX_BUF_SIZE]={0};  /* receiving buffer */
static uint32_t rx_count=0;     /* receiving counter */
//static uint8_t cr=false;        /* '\r'  received */ 
static uint8_t rx_cplt=false;   /* received a frame of data ending with "\r\n" */
static uint8_t cr_begin=false;        /* '\r'  received */ 
static char rx1_buffer[UART_RX_BUF_SIZE]={0};  /* receiving buffer */
static uint32_t rx1_count=0;     /* receiving counter */
//static uint8_t cr1=false;        /* '\r'  received */ 
static uint8_t rx1_cplt=false;   /* received a frame of data ending with "\r\n" */
static uint8_t cr1_begin=false;        /* '\r'  received */ 

static uint8_t number6=0;
static uint8_t number7=0;
static uint8_t number8=0;
static uint8_t number9=0;
static uint8_t number10=0;
static uint8_t number11=0;
static uint8_t number12=0;
static uint8_t number13=0;


	
static	uint16_t rainzhi,rainzhi1,rainzhi2,rainzhi3,rainzhi4,rainzhi5,rainzhi6;
static uint8_t M=0;    //S60M的值
static float DianYa=0;  //电压值
static uint8_t AlarmAFlag=0;
static uint8_t read_buf[128];   //从EEPROM读数据存储的缓存
static uint32_t n=0;
static char *p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8,*p9,*p10,*p11,*p12;
static char *pointer_1,*pointer_2,*pointer_3,*pointer_4,*pointer_5,*pointer_6,*pointer_7,*pointer_8,*pointer_9,*pointer_10,*pointer_11,*pointer_12;//气温湿度气压风向风速2分风向2分风速10分风向10分风速


/** @addtogroup Templates
  * @{
  */
static void RTC_CalendarShow(void);
static void PCF8563_GetFunc( _PCF8563_Time_Typedef* PCF_TimeStruct, _PCF8563_Date_Typedef* PCF_DateStruct) ;
static void PCF8563_SetFunc(void) ;
static void Data_Save(void);
static void Data_Refresh(void);
static void Data_Time_Refresh(void);
static void Zhu_Refresh(void);
static void Time_Menu(void);
static void QWDZ(void);
static void SDDZ(void);
static void FSDZ(void);
static void QYCSDZ(void);
static void QYDZ(void);
static void CSDZ(void);
static void WDXSDZ(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
/* Private functions ---------------------------------------------------------*/
uint8_t aShowTime[50] = {0};
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	//uint8_t res,temp;
  uint8_t nian,yue,ri,shi,fen,miao,key;
  /* STM32L1xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();
  SystemClock_Config();
	MX_GPIO_Init();
	USART1_UART_Init(115200);
	USART2_UART_Init(115200);
	IIC_Init();
	OLED_Init();
	FM_IIC_Init();
	KEY_Init();

  /* Configure the system clock to 32 MHz */

	MX_RTC_Init();

//开机菜单
  OLED_OpenMenu();
	HAL_Delay(3000);
	
	//主菜单
	OLED_Clear();
	OLED_ZhuMenu();
  /* Add your application code here
     */
	//检查PCF8563
//  printf("正常运行！！！\r\n");
//	res = PCF8563_Check();
//	if(res)
//	{
//		printf("pcf8563 error------\r\n");
//	}else
//	{
//  	printf("pcf8563 normal------\r\n");
//	}
//	//检查AT24C512
//	temp=FM24C256_Check();
//	if(temp==3)
//		printf("俩AT24c256 normal------\r\n");
//	else if(temp==2)
//		printf("fm24c256 1_normal,2_error------\r\n");
//	else if(temp==1)
//		printf("fm24c256_2 normal,1_error------\r\n");
//	else if(temp==0)
//		printf("俩fm24c256 error------\r\n");
//    PCF8563_SetFunc();    //设置时间
	while(1)
	{
		 //每1S钟刷新一次时间
		if(AlarmAFlag==1)  
		{
		  RTC_CalendarShow();
			PCF8563_GetFunc(&PCF_DataStruct_Time,&PCF_DataStruct_Date);
			
			Zhu_Refresh();//主菜单实时数据的刷新
			//RTC和PCF8563时间以小时同步一次
			if((stimestructureget.Minutes==25)&(stimestructureget.Seconds==15))
			{
				PCF8563_GetFunc(&PCF_DataStruct_Time,&PCF_DataStruct_Date);
				
				sdatestructureget.Year   =  PCF_DataStruct_Date.RTC_Years;
				sdatestructureget.Month  =  PCF_DataStruct_Date.RTC_Months;
				sdatestructureget.Date  =   PCF_DataStruct_Date.RTC_Days;
				stimestructureget.Hours  =  PCF_DataStruct_Time.RTC_Hours;
				stimestructureget.Minutes = PCF_DataStruct_Time.RTC_Minutes;
				stimestructureget.Seconds = PCF_DataStruct_Time.RTC_Seconds;
				/* 获取当前时间 */
				HAL_RTC_SetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
				/* 获取当前日期 */
				HAL_RTC_SetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
				
			}

			AlarmAFlag=0;
		}
		//存储数据  10	S存储一次
		if( PCF_DataStruct_Time.RTC_Seconds%10==0)
		{
			Data_Save();     
		}
		//串口1与电脑通讯
		if(rx_cplt==true)
		{		
			HAL_UART_Transmit(&huart2,rx_buffer,sizeof(rx_buffer),1000);
			
			rx_count=0;
      rx_cplt=false;                                              /* clear the flag of receive */
      memset(rx_buffer,0,sizeof(rx_buffer));  			/* clear the register of receive */
			
			//串口2接收到的数据发送到串口1（电脑）
		  if(rx1_cplt==true)
		  {
			  HAL_UART_Transmit(&huart1,rx1_buffer,sizeof(rx1_buffer),1000);
			
			  rx1_count=0;
        rx1_cplt=false;                                              /* clear the flag of receive */
        memset(rx1_buffer,0,sizeof(rx1_buffer));  
			
		   }
	  	}
		
	 key= KEY_Scan(0);
			switch(key)
			{
				case 0:
					break;
				case KEY_Escape_PRES:
					break;
				case KEY_Enter_PRES:
				{
					OLED_Clear();//清屏
					if(number==0)
					{
						OLED_DataMenu();
						while(1)
						{
							if(AlarmAFlag==1)
							{
								Data_Time_Refresh();
							}
							Data_Refresh();
							key= KEY_Scan(0);
							if(key==KEY_Right_PRES)
							{
								number1++;
								if(number1>2)
									number1=0;
								OLED_DataMenu();
							}
							else if(key==KEY_Left_PRES)
							{
								number1--;
								if(number1<0)
									number1=2;
								OLED_DataMenu();
							}
							else if(key==KEY_Escape_PRES)
								break;
						 }
						OLED_ZhuMenu();
					}
					else if(number==1)
					{
						OLED_SheZhiMenu();
						while(1)
						{
							key= KEY_Scan(0);
							if(key==KEY_Right_PRES)
							{
							  number5++;
								if(number5>3)
									number5=0;
								OLED_SheZhiMenu();
							}
							else if(key==KEY_Left_PRES)
							{
							  number5--;
								if(number5<0)
									number5=3;
								OLED_SheZhiMenu();
							}
							else if(key==KEY_Enter_PRES)
							{
								OLED_Clear();//清屏
								if(number5==0)
								{
									OLED_CunChuMenu();
									while(1)
									{
										key=KEY_Scan(0);
										if(key==KEY_Right_PRES)
										{
											number5++;
											if(number5>2)
												number5=0;
											OLED_CunChuMenu();
											
										}
										else if(key==KEY_Left_PRES)
										{
											number5--;
											if(number5<0)
												number5=2;
											OLED_CunChuMenu();
										}
										else if(key==KEY_Enter_PRES)
										{
											if(number5==0)
											{
												OLED_China(1,0,33,1);//本
												OLED_China(17,0,34,1);//机
												OLED_China(33,0,35,1);//存
												OLED_China(49,0,36,1);//储
												snprintf(disp_buf,sizeof(disp_buf),"%02u",M);
												OLED_ShowString(104,0,disp_buf,0);
												OLED_Refresh_Gram();		//更新显示到OLED 
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Right_PRES)
													{
														M++;
														if(M>99)
															M=0;
														snprintf(disp_buf,sizeof(disp_buf),"%02u",M);
														OLED_ShowString(104,0,disp_buf,0);
													}
													else if(key==KEY_Left_PRES)
													{
														M--;
														if(M<0)
															M=99;
														snprintf(disp_buf,sizeof(disp_buf),"%02u",M);
														OLED_ShowString(104,0,disp_buf,0);
													}
													else if(key==KEY_Escape_PRES)
														break;
													else if(key==KEY_Enter_PRES)
														break;
												}
												OLED_CunChuMenu();
											}
										}
										else if(key==KEY_Escape_PRES)
										  break;										
									}
									OLED_Clear();
									OLED_SheZhiMenu();
								}								
								else if(number5==1)
								{
									/* 获取当前时间 */
									HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
									/* 获取当前日期 */
									HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
									Time_Menu();
									nian  =  sdatestructureget.Year;
									yue   =  sdatestructureget.Month;
									ri    =  sdatestructureget.Date;
									shi   =  stimestructureget.Hours;
									fen   =  stimestructureget.Minutes;
									miao  =  stimestructureget.Seconds;
									while(1)
									{
								    key=KEY_Scan(0);
										if(key==KEY_Enter_PRES)
										{
											number6++;
											if(number6>5)
												number6=0;
											Time_Menu();
										}
										else if(key==KEY_Right_PRES)
										{
											if(number6==0)
											{
												nian++;
												if(nian>99)
													nian=0;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",nian);
												OLED_ShowString(49,0,disp_buf,0);//“17年
											}
											else if(number6==1)
											{
												yue++;
												if(yue>12)
													yue=1;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",yue);
												OLED_ShowString(33,24,disp_buf,1);//”06月“
											}
											else if(number6==2)
											{
												ri++;
												if(ri>31)
													ri=1;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",ri);
												OLED_ShowString(65,24,disp_buf,1);//”06日“
											}
											else if(number6==3)
											{
												shi++;
												if(shi>23)
													shi=0;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",shi);
												OLED_ShowString(17,48,disp_buf,1);//”06时“
											}
											else if(number6==4)
											{
												fen++;
												if(fen>59)
													fen=0;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",fen);
												OLED_ShowString(49,48,disp_buf,1);//”06分“
											}
											else if(number6==5)
											{
												miao++;
												if(miao>59)
													miao=0;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",miao);
												OLED_ShowString(81,48,disp_buf,1);//”06秒“
											}
										}
										else if(key==KEY_Left_PRES)
										{
											if(number6==0)
											{
												nian--;
												if(nian<0)
													nian=99;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",nian);
												OLED_ShowString(49,0,disp_buf,0);//“17年
											}
											else if(number6==1)
											{
												yue--;
												if(yue<0)
													yue=12;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",yue);
												OLED_ShowString(33,24,disp_buf,1);//”06月“
											}
											else if(number6==2)
											{
												ri--;
												if(ri<0)
													ri=31;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",ri);
												OLED_ShowString(65,24,disp_buf,1);//”06日“
											}
											else if(number6==3)
											{
												shi--;
												if(shi<0)
													shi=23;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",shi);
												OLED_ShowString(17,48,disp_buf,1);//”06时“
											}
											else if(number6==4)
											{
												fen--;
												if(fen<0)
													fen=59;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",fen);
												OLED_ShowString(49,48,disp_buf,1);//”06分“
											}
											else if(number6==5)
											{
												miao--;
												if(miao<0)
													miao=59;
												snprintf(disp_buf,sizeof(disp_buf),"%02u",miao);
												OLED_ShowString(81,48,disp_buf,1);//”06秒“
											}
										}
										else if(key==KEY_Escape_PRES)
											break;										
									}
									OLED_Clear();
									OLED_SheZhiMenu();
									if((nian!=sdatestructureget.Year)&&(yue!=sdatestructureget.Month)&&(ri!=sdatestructureget.Date )&&(shi!=stimestructureget.Hours)&&(fen!=stimestructureget.Minutes)&&(miao!=stimestructureget.Seconds))
									{
										sdatestructureget.Year   =  nian;
										sdatestructureget.Month  =  yue;
										sdatestructureget.Date  =   ri;
										stimestructureget.Hours  =  shi;
										stimestructureget.Minutes = fen;
										stimestructureget.Seconds = miao;
										/* 设置RTC当前时间 */
										HAL_RTC_SetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
										/* 设置RTC当前日期 */
										HAL_RTC_SetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
										//设置PCF8563当前日期时间
										PCF_DataStruct_Date.RTC_Years    =nian; 
										PCF_DataStruct_Date.RTC_Months   =yue; 
										PCF_DataStruct_Date.RTC_Days     = ri; 
										//PCF_DataStruct_Date.RTC_WeekDays = 3; 
										
										PCF_DataStruct_Time.RTC_Hours      =shi; 
										PCF_DataStruct_Time.RTC_Minutes    =fen; 
										PCF_DataStruct_Time.RTC_Seconds    =miao; 

										PCF8563_SetMode(PCF_Mode_Normal); 
										PCF8563_Stop(); 
										PCF8563_SetTime(PCF_Format_BIN, &PCF_DataStruct_Time); 
										PCF8563_SetDate(PCF_Format_BIN, 0,&PCF_DataStruct_Date); 
										PCF8563_Start(); 										
									}
								}
								else if(number5==2)
								{
									OLED_YingJianMenu();
									while(1)
									{
										key=KEY_Scan(0);
										if(key==KEY_Enter_PRES)
										{
											if(number2==0)
												HAL_UART_Transmit(&huart2,"<BEGIN>",strlen("<BEGIN>"),1000);
											else if(number2==1)
												HAL_UART_Transmit(&huart2,"<CLOSE>",strlen("<CLOSE>"),1000);
											else if(number2==2)
												HAL_UART_Transmit(&huart2,"<N1113>",strlen("<N1113>"),1000);
											else if(number2==3)
												HAL_UART_Transmit(&huart2,"<QKONG>",strlen("<QKONG>"),1000);
										}
										else if(key==KEY_Right_PRES)
										{
											number2++;
											if(number2>2)
												number2=0;
											OLED_YingJianMenu();
										}
										else if(key==KEY_Left_PRES)
										{
											number2--;
											if(number2<0)
												number2=2;
											OLED_YingJianMenu();
										}
										else if(key==KEY_Escape_PRES)
											break;
									}
									OLED_Clear();//清屏
									OLED_SheZhiMenu();
								}
								else if(number5==3)
								{
									OLED_DingZhengMenu();
									while(1)
									{
										key=KEY_Scan(0);
										if(key==KEY_Right_PRES)
										{
											number4++;
											if(number4>7)
												number4=0;
											OLED_DingZhengMenu();
										}
										else if(key==KEY_Left_PRES)
										{
											number4--;
											if(number4<0)
												number4=7;
											OLED_DingZhengMenu();
										}
										else if(key==KEY_Enter_PRES)
										{
											if(number4==0)
											{
												OLED_China(1,0,17,1);//气温订正
												OLED_China(17,0,18,1);
												OLED_China(33,0,64,1);
												OLED_China(49,0,51,1);
												OLED_China(112,0,61,1);
												QWDZ();
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Enter_PRES)
													{
														number7++;
														if(number7>2)
															number7=0;
														QWDZ();
													}
													else if(key==KEY_Right_PRES)
													{
														if(number7==0)
														{
															a1++;
															if(a1>1)
																a1=0;
															QWDZ();
														}
														else if(number7==1)
														{
															b1++;
															if(b1>9)
																b1=0;
															QWDZ();
														}
														else if(number7==2)
														{
															c1++;
															if(c1>9)
																c1=0;
															QWDZ();
														}
													}
													else if(key==KEY_Left_PRES)
													{
														if(number7==0)
														{
															a1--;
															if(a1<0)
																a1=1;
															QWDZ();
														}
														else if(number7==1)
														{
															b1--;
															if(b1<0)
																b1=9;
															QWDZ();
														}
														else if(number7==2)
														{
															c1--;
															if(c1<0)
																c1=9;
															QWDZ();
														}
													}
													else if(key==KEY_Escape_PRES)
														break;
												}
												OLED_DingZhengMenu();
											}
											else if(number4==1)
											{
												OLED_China(1,16,19,1);//湿度订正
												OLED_China(17,16,20,1);
												OLED_China(33,16,64,1);
												OLED_China(49,16,51,1);
												SDDZ();
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Enter_PRES)
													{
														number8++;
														if(number8>2)
															number8=0;
														SDDZ();
													}
													else if(key==KEY_Right_PRES)
													{
														if(number8==0)
														{
															a2++;
															if(a2>1)
																a2=0;
															SDDZ();
														}
														else if(number8==1)
														{
															b2++;
															if(b2>9)
																b2=0;
															SDDZ();
														}
														else if(number8==2)
														{
															c2++;
															if(c2>9)
																c2=0;
															SDDZ();
														}
													}
													else if(key==KEY_Left_PRES)
													{
														if(number8==0)
														{
															a2--;
															if(a2<0)
																a2=1;
															SDDZ();
														}
														else if(number8==1)
														{
															b2--;
															if(b2<0)
																b2=9;
															SDDZ();
														}
														else if(number8==2)
														{
															c2--;
															if(c2<0)
																c2=9;
															SDDZ();
														}
													}
													else if(key==KEY_Escape_PRES)
														break;
												}
											}
											else if(number4==2)
											{
												OLED_China(1,32,23,1);//风速订正
												OLED_China(17,32,25,1);
												OLED_China(33,32,64,1);
												OLED_China(49,32,51,1);
												FSDZ();
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Enter_PRES)
													{
														number9++;
														if(number9>2)
															number9=0;
														FSDZ();
													}
													else if(key==KEY_Right_PRES)
													{
														if(number9==0)
														{
															a3++;
															if(a3>1)
																a3=0;
															FSDZ();
														}
														else if(number9==1)
														{
															b3++;
															if(b3>9)
																b3=0;
															FSDZ();
														}
														else if(number9==2)
														{
															c3++;
															if(c3>9)
																c3=0;
															FSDZ();
														}
													}
													else if(key==KEY_Left_PRES)
													{
														if(number9==0)
														{
															a3--;
															if(a3<0)
																a3=1;
															FSDZ();
														}
														else if(number9==1)
														{
															b3--;
															if(b3<0)
																b3=9;
															FSDZ();
														}
														else if(number9==2)
														{
															c3--;
															if(c3<0)
																c3=9;
															FSDZ();
														}
													}
													else if(key==KEY_Escape_PRES)
														break;
												}
											}
											else if(number4==3)
											{}
											else if(number4==4)
											{
												OLED_China(1,0,17,1);//气压常数
												OLED_China(17,0,21,1);
												OLED_China(33,0,63,1);
												OLED_China(49,0,58,1);
												QYCSDZ();
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Enter_PRES)
													{
														number10++;
														if(number10>4)
															number10=0;
														QYCSDZ();
													}
													else if(key==KEY_Right_PRES)
													{
														if(number10==0)
														{
															a4++;
															if(a4>9)
																a4=0;
															QYCSDZ();
														}
														else if(number10==1)
														{
															b4++;
															if(b4>9)
																b4=0;
															QYCSDZ();
														}
														else if(number10==2)
														{
															c4++;
															if(c4>9)
																c4=0;
															QYCSDZ();
														}
														else if(number10==3)
														{
															d4++;
															if(d4>9)
																d4=0;
															QYCSDZ();
														}
														else if(number10==4)
														{
															e4++;
															if(e4>9)
																e4=0;
															QYCSDZ();
														}
													}
													else if(key==KEY_Left_PRES)
													{
														if(number10==0)
														{
															a4--;
															if(a4<0)
																a4=9;
															QYCSDZ();
														}
														else if(number10==1)
														{
															b4--;
															if(b4<0)
																b4=9;
															QYCSDZ();
														}
														else if(number10==2)
														{
															c4--;
															if(c4<0)
																c4=9;
															QYCSDZ();
														}
														else if(number10==3)
														{
															d4--;
															if(d4<0)
																d4=9;
															QYCSDZ();
														}
														else if(number10==4)
														{
															e4--;
															if(e4<0)
																e4=9;
															QYCSDZ();
														}
													}
													else if(key==KEY_Escape_PRES)
														break;
												}
											}
											else if(number4==5)
											{
												OLED_China(1,16,17,0);//气压订正
												OLED_China(17,16,21,0);
												OLED_China(33,16,64,0);
												OLED_China(49,16,51,0);
												QYDZ();
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Enter_PRES)
													{
														number11++;
														if(number11>2)
															number11=0;
														QYDZ();
													}
													else if(key==KEY_Right_PRES)
													{
														if(number11==0)
														{
															a5++;
															if(a5>1)
																a5=0;
															QYDZ();
														}
														else if(number11==1)
														{
															b5++;
															if(b5>9)
																b5=0;
															QYDZ();
														}
														else if(number11==2)
														{
															c5++;
															if(c5>9)
																c5=0;
															QYDZ();
														}
													}
													else if(key==KEY_Left_PRES)
													{
														if(number11==0)
														{
															a5--;
															if(a5<0)
																a5=1;
															QYDZ();
														}
														else if(number11==1)
														{
															b5--;
															if(b5<0)
																b5=9;
															QYDZ();
														}
														else if(number11==2)
														{
															c5--;
															if(c5<0)
																c5=9;
															QYDZ();
														}
													}
													else if(key==KEY_Escape_PRES)
														break;
												}
											}
											else if(number4==6)
											{
												OLED_China(1,32,63,1);//常数订正
												OLED_China(17,32,58,1);
												OLED_China(33,32,64,1);
												OLED_China(49,32,51,1);
												CSDZ();
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Enter_PRES)
													{
														number12++;
														if(number12>9)
															number12=0;
														CSDZ();
													}
													else if(key==KEY_Right_PRES)
													{
														if(number12==0)
														{
															a6++;
															if(a6>9)
																a6=0;
															CSDZ();
														}
														else if(number12==1)
														{
															b6++;
															if(b6>9)
																b6=0;
															CSDZ();
														}
														else if(number12==2)
														{
															c6++;
															if(c6>9)
																c6=0;
															CSDZ();
														}
													}
													else if(key==KEY_Left_PRES)
													{
														if(number12==0)
														{
															a6--;
															if(a6<0)
																a6=1;
															CSDZ();
														}
														else if(number12==1)
														{
															b6--;
															if(b6<0)
																b6=9;
															CSDZ();
														}
														else if(number12==2)
														{
															c6--;
															if(c6<0)
																c6=9;
															CSDZ();
														}
													}
													else if(key==KEY_Escape_PRES)
														break;
												}
											}
											else if(number4==7)
											{
												OLED_China(1,48,18,1);//温度系数	
												OLED_China(17,48,20,1);
												OLED_China(33,48,64,1);
												OLED_China(49,48,55,1);
												OLED_China(112,48,58,1);
												WDXSDZ();
												while(1)
												{
													key=KEY_Scan(0);
													if(key==KEY_Enter_PRES)
													{
														number13++;
														if(number13>9)
															number13=0;
														WDXSDZ();
													}
													else if(key==KEY_Right_PRES)
													{
														if(number13==0)
														{
															a7++;
															if(a7>9)
																a7=0;
															WDXSDZ();
														}
														else if(number13==1)
														{
															b7++;
															if(b7>9)
																b7=0;
															WDXSDZ();
														}
														else if(number13==2)
														{
															c7++;
															if(c7>9)
																c7=0;
															WDXSDZ();
														}
													}
													else if(key==KEY_Left_PRES)
													{
														if(number13==0)
														{
															a7--;
															if(a7<0)
																a7=1;
															WDXSDZ();
														}
														else if(number13==1)
														{
															b7--;
															if(b7<0)
																b7=9;
															WDXSDZ();
														}
														else if(number13==2)
														{
															c7--;
															if(c7<0)
																c7=9;
															WDXSDZ();
														}
													}
													else if(key==KEY_Escape_PRES)
														break;
												}
											}
										}
									}
									OLED_Clear();
									OLED_SheZhiMenu();
									if((b1!=0)||(c1!=0))
									{
										snprintf(disp_buf,sizeof(disp_buf),"<ZG%01u%01u%01u>",a1,b1,c1);
										HAL_UART_Transmit(&huart2,disp_buf,sizeof(disp_buf),1000);
									}
									if((b2!=0)||(c2!=0))
									{
										snprintf(disp_buf,sizeof(disp_buf),"<HG%01u%01u%01u>",a2,b2,c2);
										HAL_UART_Transmit(&huart2,disp_buf,sizeof(disp_buf),1000);
									}
									if((b3!=0)||(c3!=0))
									{
										snprintf(disp_buf,sizeof(disp_buf),"<XG%01u%01u%01u>",a3,b3,c3);
										HAL_UART_Transmit(&huart2,disp_buf,sizeof(disp_buf),1000);
									}
									if((a4!=0)||(b4!=0)||(c4!=0)||(d4!=0)||(e4!=0))
									{
										snprintf(disp_buf,sizeof(disp_buf),"<%01u%01u%01u%01u%01u>",a4,b4,c4,d4,e4);
										HAL_UART_Transmit(&huart2,disp_buf,sizeof(disp_buf),1000);
									}
									if((b5!=0)||(c5!=0))
									{
										snprintf(disp_buf,sizeof(disp_buf),"<YG%01u%01u%01u>",a5,b5,c5);
										HAL_UART_Transmit(&huart2,disp_buf,sizeof(disp_buf),1000);
									}
									if((a6!=0)||(b1!=0)||(c1!=0))
									{
										snprintf(disp_buf,sizeof(disp_buf),"<L+%01u%01u%01u>",a6,b6,c6);
										HAL_UART_Transmit(&huart2,disp_buf,sizeof(disp_buf),1000);
									}
									if((a7!=0)||(b1!=0)||(c1!=0))
									{
										snprintf(disp_buf,sizeof(disp_buf),"<V+0%01u%01u>",b7,c7);
										HAL_UART_Transmit(&huart2,disp_buf,sizeof(disp_buf),1000);
									}
									
								}
							}
							else if(key==KEY_Data_PRES)
							{
								OLED_Clear();//清屏
								OLED_DataMenu();
								while(1)
									{
										if(AlarmAFlag==1)
											{
												Data_Time_Refresh();
											 }
										Data_Refresh();
										key= KEY_Scan(0);
										if(key==KEY_Right_PRES)
											{
												number1++;
												if(number1>2)
													number1=0;
												OLED_DataMenu();
											}
										else if(key==KEY_Left_PRES)
											{
												number1--;
												if(number1<0)
													number1=2;
												OLED_DataMenu();
											}
										else if(key==KEY_Escape_PRES)
											break;
									  }
									break;
							  }
							else if(key==KEY_Escape_PRES)
								break;
						}
						OLED_Clear();
						OLED_ZhuMenu();
					}
					else if(number==2)
					{
						OLED_AverageMenu();
						while(1)
						{
							if(rx1_cplt==true)
	            	{
									if(strlen(rx1_buffer)==69)
										{
											
											printf("数据：%s\r\n",rx1_buffer);//数据
											
											//printf("时间：%s\r\n",strtok(rx_buffer,"<,>"));//时间  
											
											p1=strchr(rx_buffer,'<');
											pointer_1=strtok(p1,"<,>");
											//printf("时间：%s\r\n",pointer_1);//时间 
				
											p2=strchr(p1,',');
											pointer_2=strtok(p2,",>");
											//printf("气温：%s\r\n",pointer_2);//气温
											
											p3=strchr(p2,',');
											pointer_3=strtok(p3,",");
											//printf("湿度：%s\r\n",pointer_3);//湿度
											
											p4=strchr(p3,',');
											pointer_4=strtok(p4,",");
											//printf("气压：%s\r\n",pointer_4);//气压
				     				 p5=strchr(p4,',');
										 pointer_5=strtok(p5,",");
										 //printf("风向：%s\r\n",pointer_5);//风向					
										 p6=strchr(p5,',');
										 pointer_6=strtok(p6,",");
										 //printf("风速：%s\r\n",pointer_6);//风速 
										 p7=strchr(p6,',');
										 pointer_7=strtok(p7,",");
										 //printf("2分风向：%s\r\n",pointer_7);//2分风向				 
										 p8=strchr(p7,',');
										 pointer_8=strtok(p8,",");
										 //printf("2分风速：%s\r\n",pointer_8);//2分风速
										 p9=strchr(p8,',');
										 pointer_9=strtok(p9,",");
										 //printf("10分风向：%s\r\n",pointer_9);//10分风向
										p10=strchr(p9,',');
										pointer_10=strtok(p10,",");
										//printf("10分风速：%s\r\n",pointer_10);//10分风速
										OLED_ShowString(88,0,pointer_7,1);//2分风向
										OLED_ShowString(72,16,pointer_8,1);//2分风速
										OLED_ShowString(88,32,pointer_9,1);//10分风向
										OLED_ShowString(72,48,pointer_10,1);//10分风速
										OLED_Refresh_Gram();		//更新显示到OLED 	
									 }
						     }
				    	 }
						OLED_ZhuMenu();
						 }
				  } 
				case KEY_Right_PRES:
				{
					number++;
					if(number>2)
						number=0;
					OLED_ZhuMenu();
				}
				case KEY_Left_PRES:
				{
					number--;
					if(number<0)
						number=2;
					OLED_ZhuMenu();
				}
				case KEY_Data_PRES:
				{
					OLED_Clear();//清屏
					OLED_DataMenu();
						while(1)
						{
							if(AlarmAFlag==1)
							{
								Data_Time_Refresh();
							}
							Data_Refresh();
							key= KEY_Scan(0);
							if(key==KEY_Right_PRES)
							{
								number1++;
								if(number1>2)
									number1=0;
								OLED_DataMenu();
							}
							else if(key==KEY_Left_PRES)
							{
								number1--;
								if(number1<0)
									number1=2;
								OLED_DataMenu();
							}
							else if(key==KEY_Escape_PRES)
								break;
						 }
						OLED_ZhuMenu();
				}
				default:
					break;
			}
	
	
		
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 32000000
  *            HCLK(Hz)                       = 32000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSI Frequency(Hz)              = 16000000
  *            PLLMUL                         = 12
  *            PLLDIV                         = 3
  *            Flash Latency(WS)              = 1
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

	#ifdef PLL_CLOCK_SOURCE_HSI
  /* Enable HSI Oscillator and Activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
	
	#elif (defined PLL_CLOCK_SOURCE_HSE)
  /* Enable HSE Oscillator and Activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL12;   /* 24MHz HSE Oscillator,need change HSE_VALUE in stm32l1xx_hal_conf.h */
  RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    /*while(1); */
    /* Enable HSI Oscillator and Activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLL_DIV3;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      while(1); 
    }
  }
#endif

	
//  /* Enable HSE Oscillator and Activate PLL with HSE as source */
//  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
//  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
//  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
//  RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL12;
//  RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLL_DIV3;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }

  /* Set Voltage scale1 as MCU will run at 32MHz */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}


/* USER CODE BEGIN 4 */
/**
  * 函数功能: 显示当前时间和日期
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void RTC_CalendarShow(void)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  /* 获取当前时间 */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* 获取当前日期 */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  /* 显示日期*/
  printf("TIME: %02d-%02d-%02d %02d:%02d:%02d\r\n ", 2000+sdatestructureget.Year,sdatestructureget.Month, sdatestructureget.Date,stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
 
  
}


static void PCF8563_SetFunc(void) 
{ 
	_PCF8563_Date_Typedef 	Date_InitStructure; 
	_PCF8563_Time_Typedef 	Timer_InitStruct; 


	Date_InitStructure.RTC_Years    =17; 
	Date_InitStructure.RTC_Months   =06; 
	Date_InitStructure.RTC_Days     = 22; 
	//Date_InitStructure.RTC_WeekDays = 3; 

	Timer_InitStruct.RTC_Hours      =13; 
	Timer_InitStruct.RTC_Minutes    =15; 
	Timer_InitStruct.RTC_Seconds    =0; 

	PCF8563_SetMode(PCF_Mode_Normal); 

	PCF8563_Stop(); 

	PCF8563_SetTime(PCF_Format_BIN, &Timer_InitStruct); 
	PCF8563_SetDate(PCF_Format_BIN, 0,&Date_InitStructure); 
	  
	PCF8563_Start(); 
} 

static void PCF8563_GetFunc( _PCF8563_Time_Typedef* PCF_TimeStruct, _PCF8563_Date_Typedef* PCF_DateStruct) 
{ 
	PCF8563_GetTime(PCF_Format_BCD, PCF_TimeStruct); 
	PCF8563_GetDate(PCF_Format_BCD,0, PCF_DateStruct);
	
	printf("Time:\"20%x-%x-%x %x:%x:%x\"\r\n", 
										PCF_DataStruct_Date.RTC_Years,PCF_DataStruct_Date.RTC_Months,PCF_DataStruct_Date.RTC_Days,
										PCF_DataStruct_Time.RTC_Hours,PCF_DataStruct_Time.RTC_Minutes,PCF_DataStruct_Time.RTC_Seconds );
}

static void Data_Save(void)
{
		if(rx1_cplt==true)
		{
			if(strlen(rx1_buffer)==69)
			{
				
				printf("数据：%s\r\n",rx1_buffer);//数据
				
				  //printf("时间：%s\r\n",strtok(rx_buffer,"<,>"));//时间  
				
				  p1=strchr(rx_buffer,'<');
				  pointer_1=strtok(p1,"<,>");
          //printf("时间：%s\r\n",pointer_1);//时间 
				
				  p2=strchr(p1,',');
				  pointer_2=strtok(p2,",>");
          //printf("气温：%s\r\n",pointer_2);//气温

				  p3=strchr(p2,',');
					pointer_3=strtok(p3,",");
          //printf("湿度：%s\r\n",pointer_3);//湿度
				 
				  p4=strchr(p3,',');
					pointer_4=strtok(p4,",");
          //printf("气压：%s\r\n",pointer_4);//气压
				 
				  p5=strchr(p4,',');
					pointer_5=strtok(p5,",");
          //printf("风向：%s\r\n",pointer_5);//风向
					
				  p6=strchr(p5,',');
					pointer_6=strtok(p6,",");
          //printf("风速：%s\r\n",pointer_6);//风速
 
				  p7=strchr(p6,',');
					pointer_7=strtok(p7,",");
          //printf("2分风向：%s\r\n",pointer_7);//2分风向
				 
				  p8=strchr(p7,',');
					pointer_8=strtok(p8,",");
          //printf("2分风速：%s\r\n",pointer_8);//2分风速
  
				  p9=strchr(p8,',');
					pointer_9=strtok(p9,",");
          //printf("10分风向：%s\r\n",pointer_9);//10分风向
					
					p10=strchr(p9,',');
					pointer_10=strtok(p10,",");
          //printf("10分风速：%s\r\n",pointer_10);//10分风速
					
					p11=strchr(p10,',');
					pointer_11=strtok(p11,",");
          //printf("地温：%s\r\n",pointer_11);//地温
					
					p12=strchr(p11,',');
					pointer_12=strtok(p12,",>");
          //printf("电压：%s\r\n",pointer_12);//电压
					DianYa=atof(pointer_12);  //将字符串转换成浮点型数据
					
					snprintf(disp_buf,sizeof(disp_buf),"[%s,%s%s%s%s%s%s%s%s%s%s%s]",pointer_1,pointer_2,pointer_3,pointer_4,pointer_5,pointer_6,pointer_7,pointer_8,pointer_9,pointer_10,pointer_11,pointer_12);
					//printf("历史数据：%s",disp_buf);
					if(n<1041)
					{
					 FM24C256_Write_NByte(0xa4,0x00+n*0x3F,64,disp_buf);	//写进去了	
					 //AT24C512_Read(0xa4,0x00,read_buf,128);//读取成功
					 //printf("读到的数据：%s",read_buf);
					}
					else
					{
						FM24C256_Write_NByte(0xa4,0x00+n*0x3F,64,disp_buf);	//写进去了	
					 //AT24C512_Read(0xa0,0x00,read_buf,128);//读取成功
					 //printf("读到的数据：%s",read_buf);
						
					}
					 rx_count=0;
          rx_cplt=false;                                              /* clear the flag of receive */
          memset(rx_buffer,0,sizeof(rx_buffer));                      /* clear the register of receive */
				}
			n++;
			if(n>2040)
				 n=0;
			OLED_Refresh_Gram();		//更新显示到OLED 	
	}
 
}

static void Data_Refresh(void)
{
	if(rx1_cplt==true)
		{
			if(strlen(rx1_buffer)==69)
			{
				
				printf("数据：%s\r\n",rx1_buffer);//数据
				
				  //printf("时间：%s\r\n",strtok(rx_buffer,"<,>"));//时间  
				
				  p1=strchr(rx_buffer,'<');
				  pointer_1=strtok(p1,"<,>");
          //printf("时间：%s\r\n",pointer_1);//时间 
				
				  p2=strchr(p1,',');
				  pointer_2=strtok(p2,",>");
          //printf("气温：%s\r\n",pointer_2);//气温

				  p3=strchr(p2,',');
					pointer_3=strtok(p3,",");
          //printf("湿度：%s\r\n",pointer_3);//湿度
				 
				  p4=strchr(p3,',');
					pointer_4=strtok(p4,",");
          //printf("气压：%s\r\n",pointer_4);//气压
				 
				  p5=strchr(p4,',');
					pointer_5=strtok(p5,",");
          //printf("风向：%s\r\n",pointer_5);//风向
					
				  p6=strchr(p5,',');
					pointer_6=strtok(p6,",");
          //printf("风速：%s\r\n",pointer_6);//风速
 
				  p7=strchr(p6,',');
					pointer_7=strtok(p7,",");
          //printf("2分风向：%s\r\n",pointer_7);//2分风向
				 
				  p8=strchr(p7,',');
					pointer_8=strtok(p8,",");
          //printf("2分风速：%s\r\n",pointer_8);//2分风速
  
				  p9=strchr(p8,',');
					pointer_9=strtok(p9,",");
          //printf("10分风向：%s\r\n",pointer_9);//10分风向
					
					p10=strchr(p9,',');
					pointer_10=strtok(p10,",");
          //printf("10分风速：%s\r\n",pointer_10);//10分风速
					
					p11=strchr(p10,',');
					pointer_11=strtok(p11,",");
          //printf("地温：%s\r\n",pointer_11);//地温
					
					p12=strchr(p11,',');
					pointer_12=strtok(p12,",>");
          //printf("电压：%s\r\n",pointer_12);//电压
					DianYa=atof(pointer_12);  //将字符串转换成浮点型数据

					if(number1==0)
					{
						OLED_ShowString(72,16,pointer_2,1);//气温
		        OLED_ShowString(72,32,pointer_3,1);//湿度
	      	  OLED_ShowString(56,48,pointer_4,1);//气压
					}
					else if(number1==1)
					{
						OLED_ShowString(88,16,pointer_5,1);//风向
		        OLED_ShowString(72,32,pointer_6,1);//风速
					}
					else if(number1==2)
					{
						rainzhi1=rainzhi;
						rainzhi2=rainzhi;
						rainzhi3=rainzhi;
						if(PCF_DataStruct_Time.RTC_Seconds==4) //分雨量1分钟刷新一次
							rainzhi4=rainzhi1;
						rainzhi1=rainzhi1-rainzhi4;
						if(rainzhi1>99)
							rainzhi1=rainzhi1%100;
						if((PCF_DataStruct_Time.RTC_Minutes==0)&(PCF_DataStruct_Time.RTC_Seconds==4))//时雨量1小时刷新一次
							rainzhi5=rainzhi2;
						rainzhi2=rainzhi2-rainzhi5;
						if((PCF_DataStruct_Time.RTC_Hours==0)&(PCF_DataStruct_Time.RTC_Minutes==0)&(PCF_DataStruct_Time.RTC_Seconds==4))//日雨量一天刷新一次
							rainzhi6=rainzhi3;
						rainzhi3=rainzhi3-rainzhi6;
						//printf("RAIN：%d\r\n",rainzhi);//次数
						snprintf(disp_buf,sizeof(disp_buf),"%01u.%01u",rainzhi1/10,rainzhi1%10);//分雨量
						//printf("分雨量：%s\r\n",disp_buf);//数据
						OLED_ShowString(88,16,disp_buf,1);//分雨量
						snprintf(disp_buf,sizeof(disp_buf),"%01u.%01u",rainzhi2/10,rainzhi2%10);//时雨量
						//printf("时雨量：%s\r\n",disp_buf);//数据
						OLED_ShowString(80,32,disp_buf,1);//时雨量
						snprintf(disp_buf,sizeof(disp_buf),"%01u.%01u",rainzhi3/10,rainzhi3%10);//日雨量
						//printf("日雨量：%s\r\n",disp_buf);//数据
						OLED_ShowString(72,48,disp_buf,1);//日雨量
					}
					
					 rx_count=0;
          rx_cplt=false;                                              /* clear the flag of receive */
          memset(rx_buffer,0,sizeof(rx_buffer));                      /* clear the register of receive */
				}
			OLED_Refresh_Gram();		//更新显示到OLED 	
	}
}

static void Data_Time_Refresh(void)
{
	snprintf(disp_buf,sizeof(disp_buf),"%02u:%02u:%02u",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
	OLED_ShowString(16,0,disp_buf,1);//时分秒
	if(DianYa==0)
			{
				OLED_ShowString(104,0,"///",1);
			}
			else if((DianYa<7.0)&(DianYa!=0))
			{
				OLED_Huatu(96,0,3);//电量空
			}
			else if((DianYa<7.2)&(DianYa>=7.0))
			{
				OLED_Huatu(96,0,2);//三分之一电量
			}
			else if((DianYa>=7.2)&(DianYa<7.4))
			{
				OLED_Huatu(96,0,1);//三分之二电量
			}
			else if(DianYa>=7.4)
			{
				OLED_Huatu(96,0,0);//满电量
			}
			OLED_Refresh_Gram();		//更新显示到OLED 	
}

static void Zhu_Refresh(void)
{
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Year);
	    OLED_ShowString(1,0,disp_buf,1);//年
	    snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Month);
	    OLED_ShowString(33,0,disp_buf,1);//月
		  snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Date);
	    OLED_ShowString(65,0,disp_buf,1);//日
	 	  snprintf(disp_buf,sizeof(disp_buf),"%02u:%02u:%02u",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
      OLED_ShowString(8,16,disp_buf,1);//时分秒
			snprintf(disp_buf,sizeof(disp_buf),"%02u",M);
			OLED_ShowString(104,16,disp_buf,1);//S00M
			if(DianYa==0)
			{
				OLED_ShowString(104,0,"///",1);
			}
			else if((DianYa<7.0)&(DianYa!=0))
			{
				OLED_Huatu(96,0,3);//电量空
			}
			else if((DianYa<7.2)&(DianYa>=7.0))
			{
				OLED_Huatu(96,0,2);//三分之一电量
			}
			else if((DianYa>=7.2)&(DianYa<7.4))
			{
				OLED_Huatu(96,0,1);//三分之二电量
			}
			else if(DianYa>=7.4)
			{
				OLED_Huatu(96,0,0);//满电量
			}
			
			OLED_Refresh_Gram();		//更新显示到OLED 	
}

static void Time_Menu(void)
{
	if(number6==0)
	{
	OLED_ShowString(33,0,"20",1);//“20”
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Year);
	OLED_ShowString(49,0,disp_buf,0);//“17年”
	OLED_China(65,0,8,1);//年
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Month);
	OLED_ShowString(33,24,disp_buf,1);//”06月“
  OLED_China(49,24,9,1);//月
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Date);
	OLED_ShowString(65,24,disp_buf,1);//“22日”
  OLED_China(81,24,10,1);//日
	snprintf(disp_buf,sizeof(disp_buf),"%02u",stimestructureget.Hours);
	OLED_ShowString(17,48,disp_buf,1);//“10点”
  OLED_China(33,48,14,1);//时
	snprintf(disp_buf,sizeof(disp_buf),"%02u",stimestructureget.Minutes);
	OLED_ShowString(49,48,disp_buf,1);//“41分”
  OLED_China(65,48,27,1);//分
	snprintf(disp_buf,sizeof(disp_buf),"%02u",stimestructureget.Seconds);
	OLED_ShowString(81,48,disp_buf,1);//“33秒”
  OLED_China(97,48,41,1);//秒
	}
	else if(number6==1)
	{
	OLED_ShowString(33,0,"20",1);//“20”
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Year);
	OLED_ShowString(49,0,disp_buf,1);//“17年”
	OLED_China(65,0,8,1);//年
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Months);
	OLED_ShowString(33,24,disp_buf,0);//”06月“
  OLED_China(49,24,9,1);//月
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Days);
	OLED_ShowString(65,24,disp_buf,1);//“22日”
  OLED_China(81,24,10,1);//日
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Hours);
	OLED_ShowString(17,48,disp_buf,1);//“10点”
  OLED_China(33,48,14,1);//时
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Minutes);
	OLED_ShowString(49,48,disp_buf,1);//“41分”
  OLED_China(65,48,27,1);//分
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Seconds);
	OLED_ShowString(81,48,disp_buf,1);//“33秒”
  OLED_China(97,48,41,1);//秒
	}
	else if(number6==2)
	{
	OLED_ShowString(33,0,"20",1);//“20”
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Year);
	OLED_ShowString(49,0,disp_buf,1);//“17年”
	OLED_China(65,0,8,1);//年
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Months);
	OLED_ShowString(33,24,disp_buf,1);//”06月“
  OLED_China(49,24,9,1);//月
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Days);
	OLED_ShowString(65,24,disp_buf,0);//“22日”
  OLED_China(81,24,10,1);//日
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Hours);
	OLED_ShowString(17,48,disp_buf,1);//“10点”
  OLED_China(33,48,14,1);//时
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Minutes);
	OLED_ShowString(49,48,disp_buf,1);//“41分”
  OLED_China(65,48,27,1);//分
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Seconds);
	OLED_ShowString(81,48,disp_buf,1);//“33秒”
  OLED_China(97,48,41,1);//秒
	}
	else if(number6==3)
	{
	OLED_ShowString(33,0,"20",1);//“20”
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Year);
	OLED_ShowString(49,0,disp_buf,1);//“17年”
	OLED_China(65,0,8,1);//年
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Months);
	OLED_ShowString(33,24,disp_buf,1);//”06月“
  OLED_China(49,24,9,1);//月
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Days);
	OLED_ShowString(65,24,disp_buf,1);//“22日”
  OLED_China(81,24,10,1);//日
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Hours);
	OLED_ShowString(17,48,disp_buf,0);//“10点”
  OLED_China(33,48,14,1);//时
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Minutes);
	OLED_ShowString(49,48,disp_buf,1);//“41分”
  OLED_China(65,48,27,1);//分
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Seconds);
	OLED_ShowString(81,48,disp_buf,1);//“33秒”
  OLED_China(97,48,41,1);//秒
	}
	else if(number6==4)
	{
	OLED_ShowString(33,0,"20",1);//“20”
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Year);
	OLED_ShowString(49,0,disp_buf,1);//“17年”
	OLED_China(65,0,8,1);//年
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Months);
	OLED_ShowString(33,24,disp_buf,1);//”06月“
  OLED_China(49,24,9,1);//月
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Days);
	OLED_ShowString(65,24,disp_buf,1);//“22日”
  OLED_China(81,24,10,1);//日
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Hours);
	OLED_ShowString(17,48,disp_buf,1);//“10点”
  OLED_China(33,48,14,1);//时
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Minutes);
	OLED_ShowString(49,48,disp_buf,0);//“41分”
  OLED_China(65,48,27,1);//分
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Seconds);
	OLED_ShowString(81,48,disp_buf,1);//“33秒”
  OLED_China(97,48,41,1);//秒
	}
	else if(number6==5)
	{
	OLED_ShowString(33,0,"20",1);//“20”
	snprintf(disp_buf,sizeof(disp_buf),"%02u",sdatestructureget.Year);
	OLED_ShowString(49,0,disp_buf,1);//“17年”
	OLED_China(65,0,8,1);//年
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Months);
	OLED_ShowString(33,24,disp_buf,1);//”06月“
  OLED_China(49,24,9,1);//月
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Date.RTC_Days);
	OLED_ShowString(65,24,disp_buf,1);//“22日”
  OLED_China(81,24,10,1);//日
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Hours);
	OLED_ShowString(17,48,disp_buf,1);//“10点”
  OLED_China(33,48,14,1);//时
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Minutes);
	OLED_ShowString(49,48,disp_buf,1);//“41分”
  OLED_China(65,48,27,1);//分
	snprintf(disp_buf,sizeof(disp_buf),"%02u",PCF_DataStruct_Time.RTC_Seconds);
	OLED_ShowString(81,48,disp_buf,0);//“33秒”
  OLED_China(97,48,41,1);//秒
	}
	OLED_Refresh_Gram();		//更新显示到OLED 
}

static void QWDZ(void)
{
	if(number7==0)
		{
			if(a1==0)
				OLED_ShowString(80,0,"+",0);
			else if(a1==1)
				OLED_ShowString(80,0,"-",0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b1);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c1);
			OLED_ShowString(104,0,disp_buf,1);
		}
		else if(number7==1)
			{
				if(a1==0)
					OLED_ShowString(80,0,"+",1);
				else if(a1==1)
					OLED_ShowString(80,0,"-",1);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",b1);
				OLED_ShowString(88,0,disp_buf,0);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",c1);
				OLED_ShowString(104,0,disp_buf,1);
			}
			else if(number7==2)
				{
					if(a1==0)
						OLED_ShowString(80,0,"+",1);
					else if(a1==1)
						OLED_ShowString(80,0,"-",1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",b1);
					OLED_ShowString(88,0,disp_buf,1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",c1);
					OLED_ShowString(104,0,disp_buf,0);
				}
				OLED_Refresh_Gram();		//更新显示到OLED
}
static void SDDZ(void)
{
	if(number8==0)
		{
			if(a2==0)
				OLED_ShowString(80,16,"+",0);
			else if(a2==1)
				OLED_ShowString(80,16,"-",0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b2);
			OLED_ShowString(88,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c2);
			OLED_ShowString(96,16,disp_buf,1);
		}
		else if(number8==1)
			{
				if(a2==0)
					OLED_ShowString(80,16,"+",1);
				else if(a2==1)
					OLED_ShowString(80,16,"-",1);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",b2);
				OLED_ShowString(88,16,disp_buf,0);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",c2);
				OLED_ShowString(96,16,disp_buf,1);
			}
			else if(number8==2)
				{
					if(a2==0)
						OLED_ShowString(80,16,"+",1);
					else if(a2==1)
						OLED_ShowString(80,16,"-",1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",b2);
					OLED_ShowString(88,16,disp_buf,1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",c2);
					OLED_ShowString(96,16,disp_buf,0);
				}
				OLED_Refresh_Gram();		//更新显示到OLED
}
static void FSDZ(void)
{
	if(number9==0)
		{
			if(a3==0)
				OLED_ShowString(72,32,"+",0);
			else if(a3==1)
				OLED_ShowString(72,32,"-",0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b3);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c3);
			OLED_ShowString(96,32,disp_buf,1);
		}
		else if(number9==1)
			{
				if(a3==0)
					OLED_ShowString(72,16,"+",1);
				else if(a3==1)
					OLED_ShowString(72,16,"-",1);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",b3);
				OLED_ShowString(80,16,disp_buf,0);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",c3);
				OLED_ShowString(96,16,disp_buf,1);
			}
			else if(number9==2)
				{
					if(a3==0)
						OLED_ShowString(72,16,"+",1);
					else if(a3==1)
						OLED_ShowString(72,16,"-",1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",b3);
					OLED_ShowString(80,16,disp_buf,1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",c3);
					OLED_ShowString(96,16,disp_buf,0);
				}
				OLED_Refresh_Gram();		//更新显示到OLED
}
static void QYCSDZ(void)
{
	if(number10==0)
		{
			snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
		}
		else if(number10==1)
			{
			snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
			}
			else if(number10==2)
				{
			snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
				}
				else if(number10==3)
				{
			snprintf(disp_buf,sizeof(disp_buf),"%01u",a4);
			OLED_ShowString(80,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b4);
			OLED_ShowString(88,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c4);
			OLED_ShowString(104,0,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",d4);
			OLED_ShowString(112,0,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",e4);
			OLED_ShowString(120,0,disp_buf,1);
				}
				else if(number10==4)
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
			OLED_ShowString(120,0,disp_buf,0);
				}
				OLED_Refresh_Gram();		//更新显示到OLED
}
static void QYDZ(void)
{
	if(number11==0)
		{
			if(a5==0)
				OLED_ShowString(72,16,"+",0);
			else if(a5==1)
				OLED_ShowString(72,16,"-",0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b5);
			OLED_ShowString(80,16,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c5);
			OLED_ShowString(96,16,disp_buf,1);
		}
		else if(number11==1)
			{
				if(a5==0)
					OLED_ShowString(72,16,"+",1);
				else if(a5==1)
					OLED_ShowString(72,16,"-",1);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",b5);
				OLED_ShowString(80,16,disp_buf,0);
				snprintf(disp_buf,sizeof(disp_buf),"%01u",c5);
				OLED_ShowString(96,16,disp_buf,1);
			}
			else if(number11==2)
				{
					if(a5==0)
						OLED_ShowString(72,16,"+",1);
					else if(a5==1)
						OLED_ShowString(72,16,"-",1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",b5);
					OLED_ShowString(80,16,disp_buf,1);
					snprintf(disp_buf,sizeof(disp_buf),"%01u",c5);
					OLED_ShowString(96,16,disp_buf,0);
				}
				OLED_Refresh_Gram();		//更新显示到OLED
}
static void CSDZ(void)
{
	if(number12==0)
		{
			snprintf(disp_buf,sizeof(disp_buf),"%01u",a6);
			OLED_ShowString(72,32,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b6);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c6);
			OLED_ShowString(96,32,disp_buf,1);
		}
		else if(number12==1)
			{
				snprintf(disp_buf,sizeof(disp_buf),"%01u",a6);
			OLED_ShowString(72,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b6);
			OLED_ShowString(80,32,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c6);
			OLED_ShowString(96,32,disp_buf,1);
			}
			else if(number12==2)
				{
					snprintf(disp_buf,sizeof(disp_buf),"%01u",a6);
			OLED_ShowString(72,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b6);
			OLED_ShowString(80,32,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c6);
			OLED_ShowString(96,32,disp_buf,0);
				}
				OLED_Refresh_Gram();		//更新显示到OLED
}
static void WDXSDZ(void)
{
	if(number13==0)
		{
			snprintf(disp_buf,sizeof(disp_buf),"%01u",a7);
			OLED_ShowString(72,48,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b7);
			OLED_ShowString(88,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c7);
			OLED_ShowString(96,48,disp_buf,1);
		}
		else if(number13==1)
			{
		  snprintf(disp_buf,sizeof(disp_buf),"%01u",a7);
			OLED_ShowString(72,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b7);
			OLED_ShowString(88,48,disp_buf,0);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c7);
			OLED_ShowString(96,48,disp_buf,1);
			}
			else if(number13==2)
				{
			snprintf(disp_buf,sizeof(disp_buf),"%01u",a7);
			OLED_ShowString(72,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",b7);
			OLED_ShowString(88,48,disp_buf,1);
			snprintf(disp_buf,sizeof(disp_buf),"%01u",c7);
			OLED_ShowString(96,48,disp_buf,0);
				}
				OLED_Refresh_Gram();		//更新显示到OLED
}
void USART1_IRQHandler(void)
{
  UART_HandleTypeDef *huart=&huart1;    /* use uart3 to get soil moisture voltage data from the adam module */
  uint32_t tmp_flag = 0, tmp_it_source = 0;

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Receive_IT(huart);*/
    uint8_t data=0;
  
    data=huart->Instance->DR;  /* the byte just received  */
    
//    以回车换行为接收完成标志
//    /* use "\r" as a frame end symbol */
//    if(!rx_cplt)
//    {
//      if(cr_begin==true)  /* received '\r' */
//      {
//        cr_begin=false;
//        if(data=='\n')  /* received '\r' and '\n' */
//        {
//          /* Set transmission flag: trasfer complete*/
//          rx_cplt=true;
//        }
//        else
//        {
//          rx_count=0;
//        }
//      }
//      else
//      {
//        if(data=='\r')  /* get '\r' */
//        {
//          cr_begin=true;
//        }
//        else  /* continue saving data */
//        {
//          rx_buffer[rx_count]=data;
//          rx_count++;
//          if(rx_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
//          {
//            /* Set transmission flag: trasfer complete*/
//            rx_cplt=true;
//          } 
//        }
//       }
//      }
    
		//以<为开始接收标志，以>接收完成标志
		   if(!rx_cplt)
    {
      if(data=='<')
      {
        cr_begin=true;
        rx_count=0;
        rx_buffer[rx_count]=data;
        rx_count++; 
      }
      /*else if((data=='>')&&(cr_begin==true))
      {
        rx_cplt=true;
        cr_begin=false;
      }*/
      else if(cr_begin==true)
      {
        rx_buffer[rx_count]=data;
        rx_count++; 
        if(rx_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
        {
          /* Set transmission flag: trasfer complete*/
          rx_cplt=true;
        }
        
        if(data=='>')
        {
          rx_cplt=true;
          cr_begin=false;
        }
      }
      else
      {
        rx_count=0;
      }

    }
    
    /* use receive timeout */
//    if(!rx_cplt)
//    {
//      if(rx_count==0)  /* first byte */
//      {
//        /* start a timeout detection */
//        uart1_rx_timeout.start_receiving=true;
//      }
//      /* clear rx timeout counter */
//      uart1_rx_timeout.timeout_slice=0;
//      
//      /* continue saving data */
//      rx_buffer[rx_count]=data;
//      rx_count++;
//      if(rx_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
//      {
//        /* Set transmission flag: trasfer complete*/
//        rx_cplt=true;
//        uart1_rx_timeout.start_receiving=false;
//      }
//    }
    
    
    
    
//    
//    /* received a data frame */
//    if(rx_cplt==true)
//    {
//      if(semaphore_adam!=NULL)
//      {
//        /* Release the semaphore */
//        osSemaphoreRelease(semaphore_adam);
//      }
//    }
    
    
    
    
    
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Transmit_IT(huart);*/
  }

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter end --------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_EndTransmit_IT(huart);*/
  }  

  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Clear all the error flag at once */
    __HAL_UART_CLEAR_PEFLAG(huart);
    
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;
    
    HAL_UART_ErrorCallback(huart);
  } 
}


void USART2_IRQHandler(void)
{
  UART_HandleTypeDef *huart=&huart1;    /* use uart3 to get soil moisture voltage data from the adam module */
  uint32_t tmp_flag = 0, tmp_it_source = 0;

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Receive_IT(huart);*/
    uint8_t data=0;
  
    data=huart->Instance->DR;  /* the byte just received  */
    
//    以回车换行为接收完成标志
//    /* use "\r" as a frame end symbol */
//    if(!rx_cplt)
//    {
//      if(cr_begin==true)  /* received '\r' */
//      {
//        cr_begin=false;
//        if(data=='\n')  /* received '\r' and '\n' */
//        {
//          /* Set transmission flag: trasfer complete*/
//          rx_cplt=true;
//        }
//        else
//        {
//          rx_count=0;
//        }
//      }
//      else
//      {
//        if(data=='\r')  /* get '\r' */
//        {
//          cr_begin=true;
//        }
//        else  /* continue saving data */
//        {
//          rx_buffer[rx_count]=data;
//          rx_count++;
//          if(rx_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
//          {
//            /* Set transmission flag: trasfer complete*/
//            rx_cplt=true;
//          } 
//        }
//       }
//      }
    
		//以<为开始接收标志，以>接收完成标志
		   if(!rx1_cplt)
    {
      if(data=='<')
      {
        cr1_begin=true;
        rx1_count=0;
        rx1_buffer[rx1_count]=data;
        rx1_count++; 
      }
      /*else if((data=='>')&&(cr_begin==true))
      {
        rx_cplt=true;
        cr_begin=false;
      }*/
      else if(cr1_begin==true)
      {
        rx1_buffer[rx1_count]=data;
        rx1_count++; 
        if(rx1_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
        {
          /* Set transmission flag: trasfer complete*/
          rx1_cplt=true;
        }
        
        if(data=='>')
        {
          rx1_cplt=true;
          cr1_begin=false;
        }
      }
      else
      {
        rx1_count=0;
      }

    }
    
    /* use receive timeout */
//    if(!rx_cplt)
//    {
//      if(rx_count==0)  /* first byte */
//      {
//        /* start a timeout detection */
//        uart1_rx_timeout.start_receiving=true;
//      }
//      /* clear rx timeout counter */
//      uart1_rx_timeout.timeout_slice=0;
//      
//      /* continue saving data */
//      rx_buffer[rx_count]=data;
//      rx_count++;
//      if(rx_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
//      {
//        /* Set transmission flag: trasfer complete*/
//        rx_cplt=true;
//        uart1_rx_timeout.start_receiving=false;
//      }
//    }
    
    
    
    
//    
//    /* received a data frame */
//    if(rx_cplt==true)
//    {
//      if(semaphore_adam!=NULL)
//      {
//        /* Release the semaphore */
//        osSemaphoreRelease(semaphore_adam);
//      }
//    }
    
    
    
    
    
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Transmit_IT(huart);*/
  }

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter end --------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_EndTransmit_IT(huart);*/
  }  

  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Clear all the error flag at once */
    __HAL_UART_CLEAR_PEFLAG(huart);
    
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;
    
    HAL_UART_ErrorCallback(huart);
  } 
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	
	AlarmAFlag=1;
  //printf("Alarm A\r\n");
}
/**
  * 函数功能: 按键外部中断服务函数
  * 输入参数: GPIO_Pin：中断引脚
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==GPIO_PIN_11)
	{
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)==0)
			{
				//rainzhi+=1;//等于rainzhi++;
				rainzhi++;
			}
			else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)==1)
			{
				//printf("无计数数值\r\n");
			}
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);
	}
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
