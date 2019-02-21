/**
  ******************************************************************************
  * @file    button.h
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    09-October-2015
  * @brief   This file contains definitions for push-buttons
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

/** @addtogroup STM32L152C_DISCOVERY
  * @{
  */ 
  
/** @addtogroup STM32L152C_Discovery_Common
  * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"



typedef enum 
{
  BUTTON_ESCAPE = 0,
	BUTTON_DATA   = 1,
  BUTTON_ENTER  = 2,
  BUTTON_LEFT   = 3,
  BUTTON_RIGHT  = 4,
} ButtonType_TypeDef;


/**
  * @}
  */ 

/** @defgroup STM32L152C_DISCOVERY_Exported_Constants Exported Constants
  * @{
  */ 


  
/** @addtogroup STM32L152C_DISCOVERY_BUTTON BUTTON Constants
  * @{
  */  
#define BUTTONn                          1
/**
 * @brief USER push-button
 */
#define BUTTON_ESCAPE_PIN             GPIO_PIN_4
#define BUTTON_DATA_PIN               GPIO_PIN_5
#define BUTTON_ENTER_PIN              GPIO_PIN_6
#define BUTTON_LEFT_PIN               GPIO_PIN_7
#define BUTTON_RIGHT_PIN              GPIO_PIN_8

#define BUTTONs_PINS                  (BUTTON_ESCAPE_PIN | BUTTON_DATA_PIN | BUTTON_ENTER_PIN | BUTTON_LEFT_PIN | BUTTON_RIGHT_PIN)          /* PE.12-PE.15 */
#define BUTTONs_GPIO_PORT             GPIOA
#define BUTTONs_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUTTONs_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define BUTTONs_EXTI_IRQn             EXTI15_10_IRQn                                                   /* EXTI Line 15..10 */



/**
  * @}
  */ 

/**
  * @}
  */



/** @addtogroup STM32152C_DISCOVERY_BUTTON_Functions
  * @{
  */

void      Button_Init(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif
  
#endif /* __BUTTON_H */

/**
  * @}
  */

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
