/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __pcf8563_H
#define __pcf8563_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "stdio.h"


//IO操作函数	 
#define IIC_SCL_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)        //SCL
#define IIC_SCL_L    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)     //SCL

#define IIC_SDA_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)     //SDA	 
#define IIC_SDA_L 	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)  //SCL
#define SSDA         HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)
	 
typedef struct
{
	unsigned char RTC_Year;      //年
	unsigned char RTC_Month;     //月
	unsigned char RTC_WeekDay;   //星期
	unsigned char RTC_Day;       //日
	unsigned char RTC_Hour;      //时
	unsigned char RTC_Minute;    //分
	unsigned char RTC_Second;    //秒
}PCF8563_Time_Typedef;


extern unsigned char Time_Buffer[7];	
extern PCF8563_Time_Typedef PCF_DataStruct_Time	 ;
//typedef enum
//{
//	FALSE=0,
//	TURE=1,
//}bool;




//操作函数
void SDA_OUT(void);
void SDA_IN(void);
void I2C_delay(void);
void IIC_Init(void);	 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
unsigned char IIC_Send_Byte(unsigned char RTC_Cmd);
unsigned char IIC_Read_Byte(void);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

uint8_t PCF8563_Read_Time(void);
uint8_t PCF8563_Set_Time(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Minute,uint8_t Second);
	 
	 
#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

