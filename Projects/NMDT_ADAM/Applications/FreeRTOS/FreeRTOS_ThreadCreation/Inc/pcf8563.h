/**
  ******************************************************************************
  * File Name          : PCF8563.h
  * Description        : This file provides code for the configuration
  *                      of the PCF8563 instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PCF8563_H
#define __PCF8563_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "i2c.h"



   
   
HAL_StatusTypeDef PCF8563_Init(void);
HAL_StatusTypeDef PCF8563_SetDateTime(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);
HAL_StatusTypeDef PCF8563_GetDateTime(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);




#ifdef __cplusplus
}
#endif
#endif /*__PCF8563_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/






///* Define to prevent recursive inclusion -------------------------------------*/
//#ifndef __myiic_H
//#define __myiic_H
//#ifdef __cplusplus
// extern "C" {
//#endif

///* Includes ------------------------------------------------------------------*/
//#include "stm32l1xx_hal.h"
//#include "stdio.h"	 

////IO操作函数	 
//#define IIC_SCL_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)        //SCL
//#define IIC_SCL_L    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)     //SCL

//#define IIC_SDA_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)     //SDA	 
//#define IIC_SDA_L 	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)  //SCL
//#define SSDA         HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)
//typedef struct
//{
//	unsigned char RTC_Year;      //年
//	unsigned char RTC_Month;     //月
//	unsigned char RTC_WeekDay;   //星期
//	unsigned char RTC_Day;       //日
//	unsigned char RTC_Hour;      //时
//	unsigned char RTC_Minute;    //分
//	unsigned char RTC_Second;    //秒
//}PCF8563_Time_Typedef;

//extern unsigned char Time_Buffer[7];	
//extern PCF8563_Time_Typedef PCF_DataStruct_Time	 ;

////操作函数
//void SDA_OUT(void);
//void SDA_IN(void);
//void I2C_delay(void);
// 
//void IIC_Start(void);				//发送IIC开始信号
//void IIC_Stop(void);	  			//发送IIC停止信号
//unsigned char IIC_Send_Byte(unsigned char RTC_Cmd);
//unsigned char IIC_Read_Byte(void);//IIC读取一个字节
//void IIC_Ack(void);					//IIC发送ACK信号
//void IIC_NAck(void);				//IIC不发送ACK信号

//HAL_StatusTypeDef IIC_Init(void);	
//HAL_StatusTypeDef IIC_Wait_Ack(void); 				//IIC等待ACK信号
//HAL_StatusTypeDef PCF8563_Read_Time(void);
//HAL_StatusTypeDef PCF8563_Set_Time(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Minute,uint8_t Second);
//	 
//	 
//#ifdef __cplusplus
//}
//#endif
//#endif /*__ pinoutConfig_H */

///**
//  * @}
//  */

///**
//  * @}
//  */

///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
