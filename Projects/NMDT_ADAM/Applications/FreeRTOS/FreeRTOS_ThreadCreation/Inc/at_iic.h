/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __at_iic_H
#define __at_iic_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

//IO操作函数	 
#define FM_IIC_SCL_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)        //SCL
#define FM_IIC_SCL_L    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)     //SCL

#define FM_IIC_SDA_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)     //SDA	 
#define FM_IIC_SDA_L 	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)  //SCL
 
//操作函数
void FM_IIC_Init(void);	 
void FM_SDA_OUT(void);
void FM_SDA_IN(void);	 
void FM_IIC_Start(void);				//发送IIC开始信号
void FM_IIC_Stop(void);	  			//发送IIC停止信号
void FM_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t FM_IIC_Read_Byte(uint8_t ack);//IIC读取一个字节
uint8_t FM_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void FM_IIC_Ack(void);					//IIC发送ACK信号
void FM_IIC_NAck(void);				//IIC不发送ACK信号
void FM_Delay(uint8_t i);

void FM_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t FM_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  	 
	 
	 
#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
