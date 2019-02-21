/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
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
#include "adc.h"

ADC_HandleTypeDef hadc;


/* ADC init function */
void ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;
  
  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance=ADC1;
  hadc.Init.ClockPrescaler=ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution=ADC_RESOLUTION_12B;
  hadc.Init.DataAlign=ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  hadc.Init.EOCSelection=EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait=ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff=ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank=ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  hadc.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
  hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  hadc.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
  hadc.Init.ExternalTrigConv=ADC_SOFTWARE_START;                   /* Trig of conversion start done by software */
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  HAL_ADC_Init(&hadc);
  
  /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  /* default channel : ADC_CHANNEL_1 */
  sConfig.Channel=ADC_CHANNEL_1;
  sConfig.Rank=ADC_REGULAR_RANK_1;
  sConfig.SamplingTime=ADC_SAMPLETIME_96CYCLES;
  HAL_ADC_ConfigChannel(&hadc,&sConfig);
}


/**
  * @brief  Initializes the ADC MSP.
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_OscInitTypeDef RCC_OscInitStructure;
  
  if(hadc->Instance==ADC1)
  {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable clock of GPIO associated to the peripheral channels */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* Enable clock of ADCx peripheral */
    __ADC1_CLK_ENABLE();
    
    /* Note: STM32L1 ADC is using a dedicated asynchronous clock derived        */
    /*       from HSI RC oscillator 16MHz.                                      */
    /*       The clock source has to be enabled at RCC top level using function */
    /*       "HAL_RCC_OscConfig()" (see comments in stm32l1_hal_adc.c header)   */

    /* Enable asynchronous clock source of ADCx */
    HAL_RCC_GetOscConfig(&RCC_OscInitStructure);
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStructure.HSIState = RCC_HSI_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStructure);
    
    
    /*##-2- Configure peripheral GPIO ##########################################*/
    /* Configure GPIO pin of the selected ADC channel */
    /**ADC GPIO Configuration    
    PA1     ------> ADC_IN1
    */
    GPIO_InitStruct.Pin=GPIO_PIN_1;
    GPIO_InitStruct.Mode=GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
  }
}

/**
  * @brief  DeInitializes the ADC MSP.
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    /* Peripheral clock disable */
    __ADC1_CLK_DISABLE();
  
    /**ADC GPIO Configuration    
    PA1     ------> ADC_IN1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
  }
}



/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
