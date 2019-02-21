/**
  ******************************************************************************
  * File Name          : PCF8563.c
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

/* Includes ------------------------------------------------------------------*/
#include "pcf8563.h"


/* Private typedef -----------------------------------------------------------*/

/** 
  * @brief  PCF8563 registers defination  
  */  

/*
 *   The PCF8563 contains sixteen 8-bit registers with an auto-incrementing address
 * register, an on-chip 32.768 kHz oscillator with an integrated capacitor, a frequency
 * divider which provides the source clock for the Real-Time Clock (RTC), a
 * programmable clock output, a timer, an alarm, a voltage-low detector and a 400 kHz
 * I2C-bus interface.
 *   All 16 registers are designed as addressable 8-bit parallel registers although not all
 * bits are implemented. The first two registers (memory address 00H and 01H) are
 * used as control and/or status registers. The memory addresses 02H through 08H are
 * used as counters for the clock function (seconds up to year counters). Address
 * locations 09H through 0CH contain alarm registers which define the conditions for an
 * alarm. Address 0DH controls the CLKOUT output frequency. 0EH and 0FH are the
 * timer control and timer registers, respectively.
 */
/* control and/or status registers 0x00-0x01 */
typedef struct
{
  __IO uint8_t CSR1;
  __IO uint8_t CSR2;
}PCR8563_CSR_TypeDef;

/* clock registers 0x02-0x08 */
typedef struct
{
  __IO uint8_t Seconds;
  __IO uint8_t Minutes;
  __IO uint8_t Hours;
  __IO uint8_t Days;
  __IO uint8_t Weekdays;
  __IO uint8_t Months;
  __IO uint8_t Years;
}PCR8563_CLK_TypeDef;

/* other registers */
typedef struct
{
  /* alarm registers 0x09-0x0C */
  __IO uint8_t MinuteAlarm;
  __IO uint8_t HourAlarm;
  __IO uint8_t DayAlarm;
  __IO uint8_t WeekdayAlarm;
  
  /* CLKOUT , timer control and timer registers 0x0D-0x0F */
  __IO uint8_t CLKOUT;
  __IO uint8_t TCON;
  __IO uint8_t TCNT;
  
}PCR8563_AT_TypeDef;

/* PCF8563 Registers */
typedef struct
{
  PCR8563_CSR_TypeDef CSR;
  PCR8563_CLK_TypeDef CLK;
  PCR8563_AT_TypeDef   AT;
}PCR8563_TypeDef;
/**
  * @}
  */

/** 
  * @brief  PCF8563 date&time structure defination  
  */  


/**
  * @}
  */

/* Private define ------------------------------------------------------------*/

#define PCF8563_ADDRESS (0xA2)      /* PCF8563 I2C Address */

#define PCF8563_CON_REG_ADDR   (0x00)  /* control/status register start address 0x00-0x01 */
#define PCF8563_CLK_REG_ADDR   (0x02)  /* clock register start address 0x02-0x08 */
#define PCF8563_ALM_REG_ADDR   (0x09)  /* alarm register start address 0x09-0x0F */

#define PCF8563_I2C_SELECTION  (hi2c1) /* i2c handler used to communicate with pcf8563 */
#define PCF8563_I2C_TIMEOUT    (300)   /* Maximum Timeout values for I2C R/W */
#define PCF8563_MAX_TRIALS     (300)   /* Maximum number of trials for HAL_I2C_IsDeviceReady() function */

//PCF8563�Ĵ�����ַ���趨�����
#define PCF8563_ON_DAT          0x00    //����RTC����
#define PCF8563_IRQ_ON_DAT      0x13    //�ж���������
#define PCF8563_ALARM_OFF_DAT   0x80    //�رձ�������
#define PCF8563_OUT_OFF_DAT     0x00    //�ر�CLKOUT�������
#define PCF8563_TIM_ON_DAT      0x82    //����������ʱ�����趨����Ƶ������
#define PCF8563_TIM_COUNT_DAT   0x01    //��������ʱ��װ��ֵ�趨

//��ʱ������Ƶ��ѡ��
#define PCF8563_TIM_ON_4096HZ   0x80    //4096Hz
#define PCF8563_TIM_ON_64HZ     0x81    //64Hz
#define PCF8563_TIM_ON_1HZ      0x82    //1Hz
#define PCF8563_TIM_ON_1_60HZ   0x83    //1/60Hz

/**
  * @}
  */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static I2C_HandleTypeDef *pcf8563_hi2c=&PCF8563_I2C_SELECTION;
/* Private function prototypes -----------------------------------------------*/

static void PCF8563_MspInit(void);
static HAL_StatusTypeDef PCF8563_WriteAddress(uint8_t Address,uint8_t *pData,uint16_t Size);
static HAL_StatusTypeDef PCF8563_ReadAddress(uint8_t Address,uint8_t *pData,uint16_t Size);
/**
  * @}
  */


/* Exported functions ---------------------------------------------------------*/




/* PCF8563 init function */
HAL_StatusTypeDef PCF8563_Init(void)
{
  HAL_StatusTypeDef status=HAL_OK;
  PCR8563_TypeDef RegInit={0};
  
  /* Low Level Interface Init */
  PCF8563_MspInit();
  
  /* control and/or status registers */
  RegInit.CSR.CSR1=PCF8563_ON_DAT;
  RegInit.CSR.CSR2=PCF8563_IRQ_ON_DAT;
  status=PCF8563_WriteAddress(PCF8563_CON_REG_ADDR,(uint8_t *)&RegInit.CSR,sizeof(RegInit.CSR));
  if(status!=HAL_OK)
  {
    return status;
  }
  
  /* Alarm , clock out and timer config */
  RegInit.AT.MinuteAlarm=PCF8563_ALARM_OFF_DAT;
  RegInit.AT.HourAlarm=PCF8563_ALARM_OFF_DAT;
  RegInit.AT.DayAlarm=PCF8563_ALARM_OFF_DAT;
  RegInit.AT.WeekdayAlarm=PCF8563_ALARM_OFF_DAT;
  RegInit.AT.CLKOUT=PCF8563_OUT_OFF_DAT;
  RegInit.AT.TCON=PCF8563_TIM_ON_1HZ;
  RegInit.AT.TCNT=PCF8563_TIM_COUNT_DAT;
  status=PCF8563_WriteAddress(PCF8563_ALM_REG_ADDR,(uint8_t *)&RegInit.AT,sizeof(RegInit.AT));
  if(status!=HAL_OK)
  {
    return status;
  }
  
  return HAL_OK;
}


/**
  * @brief  Sets PCF8563 current data&time.
  * @param  sTime: Pointer to Time structure
  * @param  sDate: Pointer to date structure
  * Format:  Binary data format in 24-hour time system
  * @retval HAL status
  */
HAL_StatusTypeDef PCF8563_SetDateTime(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
  PCR8563_CLK_TypeDef clock={0};
  HAL_StatusTypeDef status=HAL_OK;
  
  /* Check the parameters */
  assert_param(IS_RTC_YEAR(sDate->Year));
  assert_param(IS_RTC_MONTH(sDate->Month));
  assert_param(IS_RTC_DATE(sDate->Date));
  
  assert_param(IS_RTC_HOUR24(sTime->Hours));
  assert_param(IS_RTC_MINUTES(sTime->Minutes));
  assert_param(IS_RTC_SECONDS(sTime->Seconds));
  
  if(sDate->WeekDay==RTC_WEEKDAY_SUNDAY)  /* In PCF8563 0-6 stand for SUNDAY-SATURDAY */
  {
    sDate->WeekDay=0;
  }
  
  /* Convert to BCD2 format */
  clock.Seconds=RTC_ByteToBcd2(sTime->Seconds);
  clock.Minutes=RTC_ByteToBcd2(sTime->Minutes);
  clock.Hours=RTC_ByteToBcd2(sTime->Hours);
  clock.Days=RTC_ByteToBcd2(sDate->Date);
  clock.Weekdays=RTC_ByteToBcd2(sDate->WeekDay);
  clock.Months=RTC_ByteToBcd2(sDate->Month);
  clock.Years=RTC_ByteToBcd2(sDate->Year);
  
  /* set date & time */
  status=PCF8563_WriteAddress(PCF8563_CLK_REG_ADDR,(uint8_t *)&clock,sizeof(clock));
  if(status!=HAL_OK)
  {
    return status;
  }
  
  return HAL_OK;
}


/**
  * @brief  Gets PCF8563 current data&time.
  * @param  sTime: Pointer to Time structure
  * @param  sDate: Pointer to date structure
  * Format:  Binary data format in 24-hour time system
  * @retval HAL status
  */
HAL_StatusTypeDef PCF8563_GetDateTime(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
  PCR8563_CLK_TypeDef clock={0};
  HAL_StatusTypeDef status=HAL_OK;
  
  /* Read data & time */
  status=PCF8563_ReadAddress(PCF8563_CLK_REG_ADDR,(uint8_t *)&clock,sizeof(clock));
  if(status!=HAL_OK)
  {
    return status;
  }
  /* Convert to Binary format */
  sTime->Seconds = (uint8_t) RTC_Bcd2ToByte( clock.Seconds&0x7F );
  sTime->Minutes = (uint8_t) RTC_Bcd2ToByte( clock.Minutes&0x7F );
  sTime->Hours = (uint8_t) RTC_Bcd2ToByte( clock.Hours&0x3F );
  sDate->Date = (uint8_t) RTC_Bcd2ToByte( clock.Days&=0x3F );
  sDate->WeekDay = (uint8_t) RTC_Bcd2ToByte( clock.Weekdays&0x07 );
  sDate->Month = (uint8_t) RTC_Bcd2ToByte( clock.Months&0x1F );
  sDate->Year = (uint8_t) RTC_Bcd2ToByte( clock.Years&0xFF );
  
  if(sDate->WeekDay==0)  /* In PCF8563 0-6 stand for SUNDAY-SATURDAY */
  {
    sDate->WeekDay=RTC_WEEKDAY_SUNDAY;
  }
  
  /* check legitimacy */
  /* Time */
  if(!IS_RTC_SECONDS(sTime->Seconds))
  {
    sTime->Seconds=0;
  }
  if(!IS_RTC_MINUTES(sTime->Minutes))
  {
    sTime->Minutes=0;
  }
  if(!IS_RTC_HOUR24(sTime->Hours))
  {
    sTime->Hours=0;
  }
  /* Date */
  if(!IS_RTC_DATE(sDate->Date))
  {
    sDate->Date=1;
  }
  if(!IS_RTC_WEEKDAY(sDate->WeekDay))
  {
    sDate->WeekDay=RTC_WEEKDAY_SUNDAY;
  }
  if(!IS_RTC_MONTH(sDate->Month))
  {
    sDate->Month=(uint8_t)RTC_Bcd2ToByte(RTC_MONTH_JANUARY);
  }
  if(!IS_RTC_YEAR(sDate->Year))
  {
    sDate->Year=0;
  }
  
  return HAL_OK;
}


/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup Private_Functions
  * @{
  */


/**
  * @brief PCF8563 MSP Init.
  * @param  None
  * @retval None
  */
static void PCF8563_MspInit(void)
{
  /* Init I2C */
  I2C1_Init();
  pcf8563_hi2c=&PCF8563_I2C_SELECTION;
  
  /* Init GPIOs used */
  
}


/**
  * @brief  Write an amount of data in blocking mode to a specific memory address.
  * @param  Address: device write address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
static HAL_StatusTypeDef PCF8563_WriteAddress(uint8_t Address,uint8_t *pData,uint16_t Size)
{
  HAL_StatusTypeDef status;
  
  /* Checks if target device is ready for communication. */
  status=HAL_I2C_IsDeviceReady(pcf8563_hi2c,PCF8563_ADDRESS,PCF8563_MAX_TRIALS,PCF8563_I2C_TIMEOUT);
  if(status!=HAL_OK)
  {
    if(status==HAL_BUSY)
    {
      HAL_I2C_DeInit(pcf8563_hi2c);
      HAL_I2C_Init(pcf8563_hi2c);
    }
    return status;
  }
  
  /* Write an amount of data in blocking mode to a specific memory address */
  status=HAL_I2C_Mem_Write(pcf8563_hi2c,PCF8563_ADDRESS,Address,I2C_MEMADD_SIZE_8BIT,
                           pData,Size,PCF8563_I2C_TIMEOUT);
  if(status!=HAL_OK)
  {
    if(status==HAL_BUSY)
    {
      HAL_I2C_DeInit(pcf8563_hi2c);
      HAL_I2C_Init(pcf8563_hi2c);
    }
    return status;
  }
  
  return HAL_OK;
}

/**
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param  Address: device read address
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
static HAL_StatusTypeDef PCF8563_ReadAddress(uint8_t Address,uint8_t *pData,uint16_t Size)
{
  HAL_StatusTypeDef status;
  
  /* Checks if target device is ready for communication. */
  status=HAL_I2C_IsDeviceReady(pcf8563_hi2c,PCF8563_ADDRESS,PCF8563_MAX_TRIALS,PCF8563_I2C_TIMEOUT);
  if(status!=HAL_OK)
  {
    if(status==HAL_BUSY)
    {
      HAL_I2C_DeInit(pcf8563_hi2c);
      HAL_I2C_Init(pcf8563_hi2c);
    }
    return status;
  }
  
  /* Read an amount of data in blocking mode from a specific memory address */
  status=HAL_I2C_Mem_Read(pcf8563_hi2c,PCF8563_ADDRESS,Address,I2C_MEMADD_SIZE_8BIT,pData,Size,PCF8563_I2C_TIMEOUT);
  if(status!=HAL_OK)
  {
    if(status==HAL_BUSY)
    {
      HAL_I2C_DeInit(pcf8563_hi2c);
      HAL_I2C_Init(pcf8563_hi2c);
    }
    return status;
  }
  
  
  return HAL_OK;
}




/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/







////#include "pcf8563.h"

//////ģ��IIC����PCF8563   PB6=SCL,PB7=SDA,PA0=INT  ����Ϊ32.768KHz.
////PCF8563_Time_Typedef PCF_DataStruct_Time;
////unsigned char Time_Buffer[7];

//////��ʼ��IIC
////HAL_StatusTypeDef IIC_Init(void)
////{					     
////	IIC_Start(); //I2C��������
////	IIC_Send_Byte(0xA2);//����д����
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;//�������е�����  �ͽ�����
////	}
////	IIC_Send_Byte(0x00);//ѡ���ַ0x00����״̬�Ĵ���1��֮���д��ַ�Զ���1
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x00); //����STOP=0��оƬʱ������
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x13);//�򿪱����жϺͶ�ʱ���жϣ����巽ʽ
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Start(); //I2C��������
////	IIC_Send_Byte(0xA2);//����д����
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x09);//ѡ���ַ0x09���ӱ����Ĵ���
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x80);//�رշ��ӱ���
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x80);//�ر�Сʱ����
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x80);//�ر��ձ���
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x80);//�ر����ڱ���
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x00);//����CLKOUT�������Ϊ���迹
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x82);//��ʱ����Ч��Ƶ����Ϊ1Hz
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////	}
////	IIC_Send_Byte(0x01);//��ʱ����Ч��Ƶ����Ϊ1Hz
////	if(IIC_Wait_Ack()==HAL_ERROR)//�ȴ�I2C���߻ظ�
////	{
////		
////		return HAL_ERROR;
////		
////	}
////	IIC_Stop(); //I2C����ֹͣ
////	return HAL_OK;
////}


//////����SDAΪ���ģʽ
////void SDA_OUT(void)
////{
////	GPIO_InitTypeDef GPIO_InitStruct;
////	/* GPIO Ports Clock Enable */
////	__HAL_RCC_GPIOB_CLK_ENABLE();
////	
////	/*Configure GPIO pins : PB7 */
////  GPIO_InitStruct.Pin = GPIO_PIN_7;
////  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
////  GPIO_InitStruct.Pull = GPIO_NOPULL;
////  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
////  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
////}

//////����SDAΪ���ģʽ
////void SDA_IN(void)
////{
////	GPIO_InitTypeDef GPIO_InitStruct;
////	/* GPIO Ports Clock Enable */
////	__HAL_RCC_GPIOB_CLK_ENABLE();
////	
////	/*Configure GPIO pins : PB7 */
////  GPIO_InitStruct.Pin = GPIO_PIN_7;
////  GPIO_InitStruct.Mode = GPIO_MODE_INPUT ;
////  GPIO_InitStruct.Pull = GPIO_NOPULL;
////  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
////  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
////}


////void I2C_delay(void)  //PCF8563��I2C����Ƶ��Ϊ400kHz�����ݲ�ͬ�������趨�ʵ��ȴ���
////{
////	unsigned char i=5;
////	while(i)
////	{
////		i--;
////	}
////}

//////����IIC��ʼ�ź�
////void IIC_Start(void)
////{
////	SDA_OUT();     //sda�����
////	IIC_SDA_H;	  	  
////	IIC_SCL_H;
////	I2C_delay();
//// 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
////	I2C_delay();
////	IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
////	I2C_delay();
////}	  
//////����IICֹͣ�ź�
////void IIC_Stop(void)
////{
////	SDA_OUT();     //sda�����
////	IIC_SCL_H;
////	IIC_SDA_L;
//// 	I2C_delay();
////	IIC_SDA_H;
////	I2C_delay();	
////  IIC_SCL_L; 	
////	I2C_delay();
////}
//////�ȴ�Ӧ���źŵ���
//////����ֵ��1������Ӧ��ʧ��
//////        0������Ӧ��ɹ�
////HAL_StatusTypeDef IIC_Wait_Ack(void)
////{
////	SDA_IN();      //SDA����Ϊ����  
////	IIC_SCL_L;
//// 	I2C_delay();
////	IIC_SDA_H;//�ڱ�־λʱ���������ʱ SDA ��Ӧ���ֵ͵�ƽ
//// 	I2C_delay();   
////	IIC_SCL_H;
////	I2C_delay();
////	if(SSDA)//���SDA�ĵ�ƽΪ�ͣ��ʹ�����մ�PCF8563������Ӧ���ź�
////	{
////		I2C_delay();  
////		IIC_SCL_L;
////		return HAL_ERROR;
////	}
////	I2C_delay();  
////	IIC_SCL_L;//ʱ�����0 	   
////	return HAL_OK;;  
////} 
//////����ACKӦ��
////void IIC_Ack(void)
////{
////	SDA_OUT();     //sda�����
////	IIC_SCL_L;
////	IIC_SDA_L;
////	I2C_delay();
////	IIC_SCL_H;
////	I2C_delay();
////	IIC_SCL_L;
////}
//////������ACKӦ��		    
////void IIC_NAck(void)
////{
////	SDA_OUT();     //sda�����
////	IIC_SCL_L;
////	IIC_SDA_H;//
////	I2C_delay();
////	IIC_SCL_H;
////	I2C_delay();
////	IIC_SCL_L;
////}					 				     
//////IIC����һ���ֽ�
//////���شӻ�����Ӧ��
//////1����Ӧ��
//////0����Ӧ��			  
////unsigned char IIC_Send_Byte(unsigned char RTC_Cmd)
////{                        
////  unsigned char i=8;  
////SDA_OUT();     //sda�����  
////  while(i--)
////	{
////		IIC_SCL_L;
////		if(RTC_Cmd&0x80)
////		{
////			IIC_SDA_H;
////		}
////		else
////		{
////			IIC_SDA_L;
////		}
////		RTC_Cmd<<=1;
////		I2C_delay();
////		IIC_SCL_H;
////		I2C_delay();
////		IIC_SCL_L;
////		I2C_delay();
////	}
////	IIC_SCL_L;
////	return SSDA;//ÿ���յ�8λ����λ��PCF8563����һ��Ӧ���źţ���SDA���յ�
////   	 
////} 	    
//////��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
////unsigned char IIC_Read_Byte(void)
////{
////	unsigned char  i=8;
////	unsigned char Read_Byte=0;
////	SDA_IN();//SDA����Ϊ����
////  IIC_SDA_H;
////	while(i--)
////	{
////		Read_Byte<<=1;
////		IIC_SCL_L;
////		I2C_delay();
////		IIC_SCL_H;
////		I2C_delay();
////		if(SSDA)//���SDA�Ž��յ��ߵ�ƽ
////		{
////			Read_Byte|=0x01;
////		}
////	}
////	IIC_SCL_L;
////	return Read_Byte;
////	
////}



////HAL_StatusTypeDef PCF8563_Set_Time(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Minute,uint8_t Second)
////{
////	//��������������Чλ
////	if (Year > 99)      Year    = 0;  //�ָ�00��
////	if (Month > 12)     Month   = 1;  //�ָ�1��
////	if (Day > 31)       Day     = 1;  //�ָ�1��
////	if (Hour > 23)      Hour    = 0;  //�ָ�0ʱ
////	if (Minute > 59)    Minute  = 0;  //�ָ�0����
////	if (Second > 59)    Second  = 0;  //�ָ�0��
////	PCF_DataStruct_Time.RTC_Year   =  RTC_ByteToBcd2(Year);
////	PCF_DataStruct_Time.RTC_Month  =  RTC_ByteToBcd2(Month);
////	PCF_DataStruct_Time.RTC_Day    =  RTC_ByteToBcd2(Day);
////	PCF_DataStruct_Time.RTC_Hour   =  RTC_ByteToBcd2(Hour);
////	PCF_DataStruct_Time.RTC_Minute =  RTC_ByteToBcd2(Minute);
////	PCF_DataStruct_Time.RTC_Second =  RTC_ByteToBcd2(Second);
////	IIC_Start(); //I2C��������
////	IIC_Send_Byte(0xA2);//����д����
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�1\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(0x02);//ѡ����Ĵ�����ַ0x02
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�2\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(PCF_DataStruct_Time.RTC_Second);//д����  ��0x02��
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�3\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(PCF_DataStruct_Time.RTC_Minute);//д���  ��0x03��
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�4\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(PCF_DataStruct_Time.RTC_Hour);//д��ʱ  ��0x04��
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�5\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(PCF_DataStruct_Time.RTC_Day);//д����  ��0x05��
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�6\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(3);//д������  ��0x06��
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�7\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(PCF_DataStruct_Time.RTC_Month);//д����  ��0x07��
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�8\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Send_Byte(PCF_DataStruct_Time.RTC_Year);//д����  ��0x08��
////	if(IIC_Wait_Ack()==HAL_ERROR)
////  {
////		//printf("û�еȵ�Ӧ���ź�9\r\n");
////		return  HAL_ERROR;
////	}//�ȴ�I2C���߻ظ�
////	
////	IIC_Stop();
////	return HAL_OK;
////}

////HAL_StatusTypeDef PCF8563_Read_Time(void)
////{
////	IIC_Start(); //I2C��������
////	IIC_Send_Byte(0xA2);//����д����
////	if(IIC_Wait_Ack()==HAL_ERROR)
////	{
////		//printf("û�еȵ�Ӧ���ź�10\r\n");
////		return HAL_ERROR;
////	}
////	//IIC_Wait_Ack();
////	IIC_Send_Byte(0x02);//������ʼ��ַ
////	if(IIC_Wait_Ack()==HAL_ERROR)
////	{
////		//printf("û�еȵ�Ӧ���ź�11\r\n");
////		return HAL_ERROR;
////	}
////	IIC_Start();
////	IIC_Send_Byte(0xA3);//���Ͷ�����
////	if(IIC_Wait_Ack()==HAL_ERROR)
////	{
////		//printf("û�еȵ�Ӧ���ź�12\r\n");
////		return HAL_ERROR;
////	}
////	Time_Buffer[0]=IIC_Read_Byte();//��ȡ��
////	IIC_Ack();//�������ݵ�ʱ��Ҫ����Ӧ���źţ���PCF8563���յ���ȷ���ѽ��յ�����
////	
////	Time_Buffer[1]=IIC_Read_Byte();//��ȡ����
////	IIC_Ack();
////	
////	Time_Buffer[2]=IIC_Read_Byte();//��ȡСʱ
////	IIC_Ack();
////	
////	Time_Buffer[3]=IIC_Read_Byte();//��ȡ��
////	IIC_Ack();
////	
////	Time_Buffer[4]=IIC_Read_Byte();//��ȡ����
////	IIC_Ack();
////	
////	Time_Buffer[5]=IIC_Read_Byte();//��ȡ��
////	IIC_Ack();
////	
////	Time_Buffer[6]=IIC_Read_Byte();//��ȡ��
////	IIC_NAck();
////	IIC_Stop();
////	
////	//���β���
////	Time_Buffer[6] &= 0xff;//��
////	Time_Buffer[5] &= 0x1f;//��
////	Time_Buffer[3] &= 0x3f;//��
////	Time_Buffer[2] &= 0x3f;//ʱ
////	Time_Buffer[1] &= 0x7f;//��
////	Time_Buffer[0] &= 0x7f;//��
////	//ת����ʮ����
////	PCF_DataStruct_Time.RTC_Year   =  RTC_Bcd2ToByte(Time_Buffer[6]);//��
////	PCF_DataStruct_Time.RTC_Month  =  RTC_Bcd2ToByte(Time_Buffer[5]);//��
////	PCF_DataStruct_Time.RTC_Day    =  RTC_Bcd2ToByte(Time_Buffer[3]);//��
////	PCF_DataStruct_Time.RTC_Hour   =  RTC_Bcd2ToByte(Time_Buffer[2]);//ʱ
////	PCF_DataStruct_Time.RTC_Minute =  RTC_Bcd2ToByte(Time_Buffer[1]);//��
////	PCF_DataStruct_Time.RTC_Second =  RTC_Bcd2ToByte(Time_Buffer[0]);//��
////	return HAL_OK;
////}

