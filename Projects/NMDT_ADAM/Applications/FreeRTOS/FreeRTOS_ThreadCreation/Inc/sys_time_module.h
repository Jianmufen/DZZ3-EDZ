/**
  ******************************************************************************
  * File Name          : sys_time_module.h
  * Description        : This file provides a module updating system data&time. 
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
#ifndef __SYS_TIME_MODULE_H
#define __SYS_TIME_MODULE_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "main.h"
#include "rtc.h"
#include "pcf8563.h"
#include "time_related.h"
#include "time.h"

#include "stm32_adafruit_sd.h"
//#include "data_eeprom.h"
/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"
   
/* Exported types ------------------------------------------------------------*/


int32_t init_sys_time_module(void);
int32_t get_sys_time(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime);
int32_t get_sys_time_tm(struct tm *DateTime);
int32_t set_sys_time(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime);
uint16_t avergae(float *data,uint8_t len);
uint16_t avergae_rad(uint16_t *data,uint8_t len);
uint16_t avergae_qc(uint16_t *data,uint8_t len,uint8_t *qc,uint8_t *qc_res);
uint16_t avergae_qc_rad(uint16_t *data,uint8_t len,uint8_t *qc,uint8_t *qc_res);
uint16_t FillData_Minute(uint8_t ID);

#ifdef __cplusplus
}
#endif
#endif /*__SYS_TIME_MODULE_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
