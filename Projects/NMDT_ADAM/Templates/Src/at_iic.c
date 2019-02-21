#include "at_iic.h"

//模拟IIC驱动PCF8563   PB13=SCL,PB12=SDA,PA0=INT  晶振为32.768KHz.


//初始化IIC
void FM_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pins : PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);//PB14,PB15输出高
}

void FM_Delay(uint8_t i)   //us级延时
{
	uint8_t k,j;
	for(k=0;k<i;k++)
	{
	 for(j=0;j<32;j++)
		; //延时1us=32*31.25ns=1000ns=1us
	}
}

//配置SDA为输出模式
void FM_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pins : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//配置SDA为输出模式
void FM_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pins : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT ;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


//产生IIC起始信号
void FM_IIC_Start(void)
{
	FM_SDA_OUT();     //sda线输出
	FM_IIC_SDA_H;	  	  
	FM_IIC_SCL_H;
	FM_Delay(4);
 	FM_IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	FM_Delay(4);
	FM_IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void FM_IIC_Stop(void)
{
	FM_SDA_OUT();//sda线输出
	FM_IIC_SCL_L;
	FM_IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
	FM_Delay(4);	
	FM_IIC_SCL_H;
	FM_IIC_SDA_H;//发送I2C总线结束信号
	FM_Delay(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t FM_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	FM_SDA_IN();      //SDA设置为输入  
	FM_IIC_SDA_H; FM_Delay(1);	   
	FM_IIC_SCL_H;	FM_Delay(1); 
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			FM_IIC_Stop();
			return 1;
		}
	}
	FM_IIC_SCL_L;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void FM_IIC_Ack(void)
{
	FM_IIC_SCL_L;
	FM_SDA_OUT();
	FM_IIC_SDA_L;
	FM_Delay(2);
	FM_IIC_SCL_H;
	FM_Delay(2);
	FM_IIC_SCL_L;
}
//不产生ACK应答		    
void FM_IIC_NAck(void)
{
	FM_IIC_SCL_L;
	FM_SDA_OUT();
	FM_IIC_SDA_H;
	FM_Delay(2);
	FM_IIC_SCL_H;
	FM_Delay(2);
	FM_IIC_SCL_L;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void FM_IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
	FM_SDA_OUT(); 	    
  FM_IIC_SCL_L;//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
   //IIC_SDA=(txd&0x80)>>7;
	if((txd&0x80)>>7)
		FM_IIC_SDA_H;
	else
		FM_IIC_SDA_L;
		txd<<=1; 	  
		FM_Delay(2);
		FM_IIC_SCL_H;
		FM_Delay(2);
		FM_IIC_SCL_L;	
		FM_Delay(2);
   }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t FM_IIC_Read_Byte(uint8_t ack)
{
	unsigned char i,receive=0;
	FM_SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    FM_IIC_SCL_L; 
    FM_Delay(2);
		FM_IIC_SCL_H;
    receive<<=1;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14))
			receive++;   
		FM_Delay(1);
  }					 
  if (ack==0)
      FM_IIC_NAck();//发送nACK
  else
      FM_IIC_Ack(); //发送ACK   
  return receive;
}




