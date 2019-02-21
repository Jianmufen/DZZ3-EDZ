#include "myiic.h"

//模拟IIC驱动PCF8563   PB13=SCL,PB12=SDA,PA0=INT  晶振为32.768KHz.


//初始化IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pins : PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_SET);//PB12,PB13输出高
}

void Delay(uint8_t i)   //us级延时
{
	uint8_t k;
	uint8_t j=32;
	uint8_t temp=i*j;
	for(k=0;k<temp;k++);
}

//配置SDA为输出模式
void SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pins : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//配置SDA为输出模式
void SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pins : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT ;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	Delay(4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	Delay(4);
	IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	Delay(4);
	IIC_SCL_H; 
	IIC_SDA_H;//发送I2C总线结束信号
	Delay(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA_H;
	Delay(1);	   
	IIC_SCL_H;
	Delay(1); 
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	Delay(2);
	IIC_SCL_H;
	Delay(2);
	IIC_SCL_L;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	Delay(2);
	IIC_SCL_H;
	Delay(2);
	IIC_SCL_L;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
	SDA_OUT(); 	    
  IIC_SCL_L;//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
   //IIC_SDA=(txd&0x80)>>7;
	if((txd&0x80)>>7)
		IIC_SDA_H;
	else
		IIC_SDA_L;
		txd<<=1; 	  
		Delay(2);  //对TEA5767这三个延时都是必须的
		IIC_SCL_H;
		Delay(2);
		IIC_SCL_L;	
		Delay(2);
   }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    IIC_SCL_L; 
    Delay(2);
		IIC_SCL_H;
    receive<<=1;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
			receive++;   
		Delay(1);
  }					 
//  if (!ack)
//      IIC_NAck();//发送nACK
//  else
//      IIC_Ack(); //发送ACK   
  return receive;
}


