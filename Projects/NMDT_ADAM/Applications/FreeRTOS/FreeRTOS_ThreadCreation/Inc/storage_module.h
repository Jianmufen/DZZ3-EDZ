/**
  ******************************************************************************
  * File Name          : storage_module.h
  * Description        : This file provides a module to store/read system data. 
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
#ifndef __STORAGE_MODULE_H
#define __STORAGE_MODULE_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "main.h"
#include "time.h"

#include "spi.h"
#include "stm32_adafruit_sd.h"
//#include "data_eeprom.h"
/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"

   
/* SD Save Parameters */
#define DATA_BLOCK_SIZE        ((uint32_t)256)  /* indicate space usage of a block of data */
/* Exported types ------------------------------------------------------------*/
  


/* Exported constants --------------------------------------------------------*/
int start_download(void);

int32_t init_storage_module(void);
FRESULT save_sensor_data(const uint8_t *pData,uint32_t Size,uint32_t DataType,uint32_t *bw);
FRESULT read_sensor_data(const struct tm *sDateTime,uint8_t *pData,uint32_t Size,uint32_t DataType,uint32_t *br);
int32_t start_download_data(struct tm *sDateTime,uint32_t Count,uint32_t DataType);
int32_t Processing_FileManagement_Command(const uint8_t *str,uint32_t len);
HAL_StatusTypeDef save_sensor_parameter(uint32_t Address,uint8_t *Data,uint32_t Count);
HAL_StatusTypeDef read_sensor_parameter(uint32_t Address,uint8_t *Data,uint32_t Count);

FRESULT WriteFile(const uint8_t *Filename,uint32_t Offset,const uint8_t *pData,uint32_t Size,uint32_t *bw);
FRESULT ReadFile(const uint8_t *Filename,uint32_t Offset,uint8_t *pData,uint32_t Size,uint32_t *br);

#ifdef __cplusplus
}
#endif
#endif /*__STORAGE_MODULE_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
