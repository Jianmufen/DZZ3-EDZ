/**
  ******************************************************************************
  * @file    sd_io_low.h
  * @author  Application
  * @version V1.0.1
  * @date    16-ecember-2015
  * @brief   This file contains definitions for:
  *          - sd spi low level io init to use \Drivers\BSP\Adafruit_Shield\'s sd driver
  *          - microSD available on Adafruit 1.8 TFT LCD 
  *            shield (reference ID 802)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32L1XX_NUCLEO
  * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_IO_LOW_H
#define __SD_IO_LOW_H

#ifdef __cplusplus
 extern "C" {
#endif
                                              
/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
   
   
/** @defgroup STM32L1XX_SD_Exported_Types Exported Types
  * @{
  */

/* SD SPI selection */
#define __SPI_1    1     /* SPI1 */
#define __SPI_2    2     /* SPI2 */
#define SD_SPI_SELECTION     __SPI_2
   
   
/**
  * @brief  SD Control Interface pins
  */
#define SD_CS_PIN                                 GPIO_PIN_5
#define SD_CS_GPIO_PORT                           GPIOD
#define SD_CS_GPIO_CLK_ENABLE()                   __HAL_RCC_GPIOD_CLK_ENABLE()
#define SD_CS_GPIO_CLK_DISABLE()                  __HAL_RCC_GPIOD_CLK_DISABLE()
   
   
/* @note:must change HAL_SPI_MspInit() and HAL_SPI_MspDeInit() in spi.c when change SPI Pins,or remove spi.c from project */
 
/*###################### SPIx ###################################*/  
   
#if (SD_SPI_SELECTION==__SPI_1)     /* SPI1 */
   
#define SD_SPIx                                 SPI1  
#define SD_SPIx_CLK_ENABLE()                    __HAL_RCC_SPI1_CLK_ENABLE()

#define SD_SPIx_SCK_AF                          GPIO_AF5_SPI1
#define SD_SPIx_SCK_GPIO_PORT                   GPIOA
#define SD_SPIx_SCK_PIN                         GPIO_PIN_5
#define SD_SPIx_SCK_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define SD_SPIx_SCK_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

#define SD_SPIx_MISO_MOSI_AF                    GPIO_AF5_SPI1
#define SD_SPIx_MISO_MOSI_GPIO_PORT             GPIOA
#define SD_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define SD_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define SD_SPIx_MISO_PIN                        GPIO_PIN_6
#define SD_SPIx_MOSI_PIN                        GPIO_PIN_7
   
#elif (SD_SPI_SELECTION==__SPI_2)  /* SPI2 */
   
#define SD_SPIx                                 SPI2  
#define SD_SPIx_CLK_ENABLE()                    __HAL_RCC_SPI2_CLK_ENABLE()

#define SD_SPIx_SCK_AF                          GPIO_AF5_SPI2
#define SD_SPIx_SCK_GPIO_PORT                   GPIOD
#define SD_SPIx_SCK_PIN                         GPIO_PIN_1
#define SD_SPIx_SCK_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define SD_SPIx_SCK_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOD_CLK_DISABLE()

#define SD_SPIx_MISO_MOSI_AF                    GPIO_AF5_SPI2
#define SD_SPIx_MISO_MOSI_GPIO_PORT             GPIOD
#define SD_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define SD_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()
#define SD_SPIx_MISO_PIN                        GPIO_PIN_3
#define SD_SPIx_MOSI_PIN                        GPIO_PIN_4
   
#else
#error "must select the spi used to control sdcard"
#endif  /* SD_SPIx */
   
/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define SD_SPIx_TIMEOUT_MAX                   1000


/**
  * @brief  SD Control Lines management
  */  
#define SD_CS_LOW()       HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)
#define SD_CS_HIGH()      HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)
    

  
   
   
   
   
   
   


/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L1XX_SD_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
