/**
  ******************************************************************************
  * File Name          : time_related.h
  * Description        : This file provides code for some time related functions
  *                      
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
#ifndef __TIME_RELATED_H
#define __TIME_RELATED_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "rtc.h"
#include "pcf8563.h"



HAL_StatusTypeDef sync_time(void);



//其它时间计算函数
unsigned long
l_mktime(const unsigned int year0, const unsigned int mon0,
const unsigned int day, const unsigned int hour,
const unsigned int min, const unsigned int sec);  //计算从1970年1月1日0时到给定时间经过的秒数

int CalculateWeekDay(int year, int month, int day);  //根据日期判断星期

int isLeapYear(int year);  //判断一个年份是否为闰年

void AddaSecond(int *year, int *month, int *day, int *hour, int *minute, int *second);	//将一个日期增加1秒钟
void AddaMinute(int *year, int *month, int *day, int *hour, int *minute, int *second);  //将一个日期增加1分钟
void Add5Minute(int *year, int *month, int *day, int *hour, int *minute, int *second); //将一个日期时间增加5分钟
void AddaHour(int *year, int *month, int *day, int *hour, int *minute, int *second);  //将一个日期时间增加1小时
void SubaHour(int *year, int *month, int *day, int *hour, int *minute, int *second);  //将一个日期时间减少1小时



#ifdef __cplusplus
}
#endif
#endif /*__TIME_RELATED_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
