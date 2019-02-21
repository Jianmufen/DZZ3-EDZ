/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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

/* Includes ------------------------------------------------------------------*/
#include "tim.h"


//TIM_HandleTypeDef htim2;
//TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;



///* TIM2 init function */
//void TIM2_Init(void)
//{
//  TIM_ClockConfigTypeDef sClockSourceConfig;

//  htim2.Instance = TIM2;
//  htim2.Init.Prescaler = 0;
//  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim2.Init.Period = 65535;
//  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  HAL_TIM_Base_Init(&htim2);

//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
//  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
//  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
//  sClockSourceConfig.ClockFilter = 0;
//  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

//}

///* TIM3 init function */
//void TIM3_Init(void)
//{
//  TIM_IC_InitTypeDef sConfigIC;

//  htim3.Instance = TIM3;
//  htim3.Init.Prescaler = 32000-1;
//  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim3.Init.Period = 1000-1;
//  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  HAL_TIM_IC_Init(&htim3);

//  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
//  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
//  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
//  sConfigIC.ICFilter = 0;
//  HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1);

//}

/* TIM6 init function   定时器溢出时间=（32000*5000/32=5000000us=5000ms=5s）*/
void TIM6_Init(void)
{
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 32000-1;   /* frequence:1kHz */
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 5000-1;
  HAL_TIM_Base_Init(&htim6);

}

/* TIM7 init function */
void TIM7_Init(void)
{

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 32000-1;  /* frequence:1kHz */
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1000-1;
  HAL_TIM_Base_Init(&htim7);

}

/**
  * @brief  Initializes the TIM Base MSP.
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

  //GPIO_InitTypeDef GPIO_InitStruct;
//  if(htim_base->Instance==TIM2)
//  {

//    /*##-1- Enable peripherals and GPIO Clocks #################################*/
//    /* TIMx Peripheral clock enable */
//    __TIM2_CLK_ENABLE();
//    
//    /* GPIO Clock Enable */
//    __GPIOA_CLK_ENABLE();
//  
//    /**TIM2 GPIO Configuration    
//    PA0-WKUP1     ------> TIM2_ETR 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_0;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//  }
   if(htim_base->Instance==TIM6)
  {

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* TIMx Peripheral clock enable */
    __TIM6_CLK_ENABLE();
    
    /*##-2- Configure the NVIC for TIMx ########################################*/
    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(TIM6_IRQn, 0x0E, 0);

    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIM6_IRQn);

  }
  else if(htim_base->Instance==TIM7)
  {

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* TIMx Peripheral clock enable */
    __TIM7_CLK_ENABLE();
    
    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(TIM7_IRQn, 0x0E, 0);

    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIM7_IRQn);


  }
}

/**
  * @brief  Initializes the TIM Input Capture MSP.
  * @param  htim: TIM handle
  * @retval None
  */
//void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim_ic)
//{

//  GPIO_InitTypeDef GPIO_InitStruct;
//  if(htim_ic->Instance==TIM3)
//  {

//    /*##-1- Enable peripherals and GPIO Clocks #################################*/
//    /* TIMx Peripheral clock enable */
//    __TIM3_CLK_ENABLE();
//    
//    /* GPIO Clock Enable */
//    __GPIOB_CLK_ENABLE();
//  
//    /**TIM3 GPIO Configuration    
//    PB4     ------> TIM3_CH1 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_4;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    /* Set the TIMx priority */
//    HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
//    /* Enable the TIMx global Interrupt */
//    HAL_NVIC_EnableIRQ(TIM3_IRQn);
//    
//  }
//}

/**
  * @brief  DeInitializes TIM Base MSP.
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

//  if(htim_base->Instance==TIM2)
//  {

//    /* Peripheral clock disable */
//    __TIM2_CLK_DISABLE();
//  
//    /**TIM2 GPIO Configuration    
//    PA0-WKUP1     ------> TIM2_ETR 
//    */
//    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
//    
//  }
   if(htim_base->Instance==TIM6)
  {

    /* Peripheral clock disable */
    __TIM6_CLK_DISABLE();
    
    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(TIM6_IRQn);

  }
  else if(htim_base->Instance==TIM7)
  {

    /* Peripheral clock disable */
    __TIM7_CLK_DISABLE();
    
    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(TIM7_IRQn);

  }
}

/**
  * @brief  DeInitializes TIM Input Capture MSP.
  * @param  htim: TIM handle
  * @retval None
  */
//void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* htim_ic)
//{

//  if(htim_ic->Instance==TIM3)
//  {

//    /* Peripheral clock disable */
//    __TIM3_CLK_DISABLE();
//  
//    /**TIM3 GPIO Configuration    
//    PB4     ------> TIM3_CH1 
//    */
//    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);

//    /* Peripheral interrupt Deinit*/
//    HAL_NVIC_DisableIRQ(TIM3_IRQn);

//  }

//} 



/**
  * @}
  */

/**
  * @}
  */




/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
