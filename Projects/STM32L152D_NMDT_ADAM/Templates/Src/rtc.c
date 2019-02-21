/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
 RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  //RTC_TimeTypeDef sTime;
  //RTC_DateTypeDef sDate;
  RTC_AlarmTypeDef sAlarm;

    /**Initialize RTC and set the Time and Date 
    */
	 /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  HAL_RTC_Init(&hrtc);
  
//  设置日期时间   
///* Set Time: 15:25:00 */
//  sTime.Hours = 16;
//  sTime.Minutes = 59;
//  sTime.Seconds = 0;
//	sTime.TimeFormat = RTC_HOURFORMAT12_AM;
//  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
//  
// 
///* Set Date: Tuesday JUNE 20th 2017 */
//  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
//  sDate.Month = RTC_MONTH_JUNE;
//  sDate.Date = 20;
//  sDate.Year = 17;

//  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);


/* Configure the RTC WakeUp Timer to generate a 1s interrupt
    */
 //HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,0,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);   //会导致程序死机，不懂  研究中

    /**Enable the Alarm A 
    */
	/* Set Alarm to 00:00:00 
     RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
  sAlarm.Alarm = RTC_ALARM_A;
	sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	//sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;   //第1秒的时间只产生一次中断，
	sAlarm.AlarmMask =  RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES | RTC_ALARMMASK_SECONDS;   //以后的每一秒钟产生一次中断
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
	sAlarm.AlarmTime.SubSeconds = 0;
  HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
	
    /**Enable the Alarm B 
    */
  sAlarm.Alarm = RTC_ALARM_B;
  sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;    //以后的每过一分钟才能产生一次中断
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
  sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 10;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  HAL_RTC_SetAlarm_IT(&hrtc,&sAlarm,RTC_FORMAT_BIN);

 
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */
   __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    
    
    /*##-2- Configue LSE or LSI as RTC clock source ###############################*/
    /* Enable LSE and LSE Clock */
    RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
#if (defined RTC_CLOCK_SOURCE_LSE)
    RCC_OscInitStruct.LSEState=RCC_LSE_ON;
#endif
    RCC_OscInitStruct.LSIState=RCC_LSI_ON;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct)!=HAL_OK)
    {
      //Error_Handler("OscConfig failed\r\n");
    }
    
    /* select RTC clock source  */
    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection=RTC_CLOCK_SELECTION;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct)!=HAL_OK)
    {
     // Error_Handler("RTC clock source config failed\r\n");
    }
  /* USER CODE END RTC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* Peripheral interrupt init */
   // HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
   // HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */
/* RTC Alarm NVIC Init */
    /* Alarm */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x0A, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
		/* RTC WakeUp NVIC Init */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0A, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
		HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);

  }
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  printf("Wake Up\r\n");
}

/*
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  //printf("Alarm A\r\n");
}
*/

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
  printf("Alarm B\r\n");
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
