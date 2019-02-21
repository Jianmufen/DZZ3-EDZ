/**
  ******************************************************************************
  * File Name          : DS18B20.c
  * Description        : This file provides code for the configuration
  *                      of the DS18B20 instances.
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
#include "ds18b20.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ON 0x01  //读取成功返回0x00，失败返回0x01
#define OFF 0x00
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
__STATIC_INLINE void Ds18b20Delay(uint16_t k);
__STATIC_INLINE void Ds18b20InputInitial(void);
__STATIC_INLINE void Ds18b20OutputInitial(void);
__STATIC_INLINE void GetDs18b20Data_High(void);
__STATIC_INLINE void GetDs18b20Data_Low(void);
__STATIC_INLINE uint8_t GetDs18b20Data(void);
static uint8_t Ds18b20Initial(void);
static void Ds18b20Write(uint8_t infor);
static uint8_t Ds18b20Read(void);

/**
  * @}
  */


/* Exported functions ---------------------------------------------------------*/


//温度读取函数，上电需要读取1-3次使其稳定，否则输出无效值850
int16_t Temp_Get(void)
{
  uint8_t TL,TH;
  uint16_t temp;
  int16_t ds_t=0;
 
  Ds18b20Initial();    //初始化ds18b20
  Ds18b20Write(0xcc);  //忽略ROM指令，不使用64位ROM编码，只有一个从机时使用
  Ds18b20Write(0x44);  //温度转换指令
    
  Ds18b20Initial();    //初始化ds18b20
  Ds18b20Write(0xcc);
  Ds18b20Write(0xbe);  //读取转换结果指令
  
  TL = Ds18b20Read();  //转换结果读取，先读到低位，再读高位
  TH = Ds18b20Read();
  
  
  //温度计算
  if(TH>7)
  {
    TH=~TH;
    TL=~TL;
    temp=0;  //温度为负
  }
  else
  {
    temp=1;  //温度为正
  }
  ds_t=TH;  //获取高8位
  ds_t<<=8;
  ds_t+=TL; //获取低8位
  ds_t=(int16_t)((float)ds_t*0.625+0.5);  //计算温度值，放大10倍后四舍五入存放
  
  if(temp)
    return ds_t;
  else
    return -ds_t;

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
  * @}
  */

//微秒延时
__STATIC_INLINE void Ds18b20Delay(uint16_t k)
{
  volatile uint32_t i,j;  /* 编译器开最高优化时必须使用volatile，否则优化会导致读写时序不对从而导致读取一直为0 */
  for(i=0;i<k;i++)
  for(j=0;j<2;j++);
}

__STATIC_INLINE void Ds18b20InputInitial(void)//设置端口为输入
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  DS18B20_DATA_GPIO_CLK_ENABLE();
  
  GPIO_InitStruct.Pin=DS18B20_DATA_PIN;
  GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull=GPIO_NOPULL;
  GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;
  
  HAL_GPIO_Init(DS18B20_DATA_GPIO_PORT,&GPIO_InitStruct);
}

__STATIC_INLINE void Ds18b20OutputInitial(void)//设置端口为输出
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  DS18B20_DATA_GPIO_CLK_ENABLE();
  
  GPIO_InitStruct.Pin=DS18B20_DATA_PIN;
  GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull=GPIO_NOPULL;
  GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;
  
  HAL_GPIO_Init(DS18B20_DATA_GPIO_PORT,&GPIO_InitStruct);
}

__STATIC_INLINE void GetDs18b20Data_High(void)//输出高电平
{
  HAL_GPIO_WritePin(DS18B20_DATA_GPIO_PORT,DS18B20_DATA_PIN,GPIO_PIN_SET);
}

__STATIC_INLINE void GetDs18b20Data_Low(void)//输出低电平
{
  HAL_GPIO_WritePin(DS18B20_DATA_GPIO_PORT,DS18B20_DATA_PIN,GPIO_PIN_RESET);
}

__STATIC_INLINE uint8_t GetDs18b20Data(void)//读取输入值
{
  return HAL_GPIO_ReadPin(DS18B20_DATA_GPIO_PORT,DS18B20_DATA_PIN);
}

/**
  * @}
  */




//ds18b20初始化（复位）
//初始化成功返回0x00，失败返回0x01
static uint8_t Ds18b20Initial(void)
{
  uint8_t Status = 0x00;
  uint16_t CONT_1 = 0;
  uint8_t Flag_1 = ON;
  Ds18b20OutputInitial();
  GetDs18b20Data_High();
  Ds18b20Delay(260);
  GetDs18b20Data_Low();
  Ds18b20Delay(750);
  GetDs18b20Data_High();
  Ds18b20InputInitial();
  while((GetDs18b20Data()!= 0)&&(Flag_1 == ON))//等待ds18b20响应，具有防止超时功能
  {                                        //等待约60ms左右
    CONT_1++;
    Ds18b20Delay(10);
    if(CONT_1 > 8000)Flag_1 = OFF;
    Status = GetDs18b20Data();
  }
  Ds18b20OutputInitial();
  GetDs18b20Data_High();
  Ds18b20Delay(100);
  return Status;
}

//写一个字节到ds18b20
static void Ds18b20Write(uint8_t infor)
{
  uint16_t i;
  GetDs18b20Data_High();
  Ds18b20OutputInitial();
  Ds18b20Delay(100);
  for(i=0;i<8;i++)
  {
    if((infor & 0x01))
    {
      GetDs18b20Data_Low();
      Ds18b20Delay(6);
      GetDs18b20Data_High();
      Ds18b20Delay(50);
    }
    else
    {
      GetDs18b20Data_Low();
      Ds18b20Delay(50);
      GetDs18b20Data_High();
      Ds18b20Delay(6);
    }
    infor >>= 1;
  }
  GetDs18b20Data_High();
}

//从ds18b20读取一个字节
static uint8_t Ds18b20Read(void)
{
  uint8_t Value = 0x00;
  uint16_t i;
  GetDs18b20Data_High();
  Ds18b20OutputInitial();
  GetDs18b20Data_High();
  Ds18b20Delay(100);
  for(i=0;i<8;i++)
  {
    Value >>= 1; 
    Ds18b20OutputInitial();
    GetDs18b20Data_Low();
    Ds18b20Delay(3);
    GetDs18b20Data_High();
    Ds18b20Delay(3);
    Ds18b20InputInitial();
    Ds18b20Delay(3);
    if(GetDs18b20Data() == 1) Value |= 0x80;
    Ds18b20Delay(15);
  }
  return Value;
}






/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

