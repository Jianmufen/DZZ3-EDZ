#include "at24c512.h"
#include "at_iic.h"

/******************************************************************************
                            定义相关的变量函数
******************************************************************************/

//unsigned char buffer[4];



/**
  *****************************************************************************
  * @Name   : 向FM24C256写一个字节数据
  *
  * @Brief  : none
  *
  * @Input  : FM_ADD：FM24C256的地址和写命令   0xa4或者0xa0
	            RAM_ADD:内存地址  范围0x0000---0x7fff
  *           dat：    要写入的数据
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void FM24C256_Write_Byte(uint8_t FM_ADD,uint16_t RAM_ADD, char dat)
{
	FM_IIC_Start();                //启动IIC总线
	FM_IIC_Send_Byte(FM_ADD);      //发送写命令以及器件的选择  0xa0=器件1的写  0xa4器件2的写
	FM_IIC_Wait_Ack();               //等待应答
	FM_IIC_Send_Byte(RAM_ADD>>8);  //发送内存地址的高8位
	FM_IIC_Wait_Ack();
	FM_IIC_Send_Byte(RAM_ADD%256); //发送内存地址的低8位
	FM_IIC_Wait_Ack();
	FM_IIC_Send_Byte(dat);         //发送一字节数据
	FM_IIC_Wait_Ack(); 
	FM_IIC_Stop();                //停止  
	HAL_Delay(10);
}


/**
  *****************************************************************************
  * @Name   : 从AT24C512随机读一个字节数据
  *
  * @Brief  : none
  *
  * @Input  : FM_ADD：FM24C256的地址和写命令   0xa0或者0xa4
	            RAM_ADD:内存地址  范围0x0000---0x7fff
  *           
  *
  * @Output : none
  *
  * @Return : dat：    读出的数据
  *****************************************************************************
**/
uint8_t FM24C256_Read_Byte(uint8_t FM_ADD,uint16_t RAM_ADD)
{
	uint8_t temp=0;
	
	FM_IIC_Start();                //启动IIC总线
	FM_IIC_Send_Byte(FM_ADD);      //发送读命令以及器件的选择  0xa0=器件1的写  0xa4器件2的写
	FM_IIC_Wait_Ack();               //等待应答
	FM_IIC_Send_Byte(RAM_ADD>>8);  //发送内存地址的高8位
	FM_IIC_Wait_Ack();
	FM_IIC_Send_Byte(RAM_ADD%256); //发送内存地址的低8位
	FM_IIC_Wait_Ack();
	FM_IIC_Start();                //启动IIC总线
	FM_IIC_Send_Byte(FM_ADD+1);      //发送读指令   0xa1=0xa0+1器件1的读   0xa5=0x24+1器件2的读 
	FM_IIC_Wait_Ack();
	temp=FM_IIC_Read_Byte(0);       //读数据
	FM_IIC_Stop();                //停止  
	return temp;                  //将读出的数据返回
}

/**
  *****************************************************************************
  * @Name   : 向AT24C512的写入N个字节   Page Write 页写
  *
  * @Brief  : none
  *
  * @Input  : FM_ADD：FM24C256的地址和写命令   0xa0或者0xa4
	            RAM_ADD:开始写入的内存地址  范围0x0000---0x7fff
  *           dat：    要写入的数据16位或者32位
*             len:     要写入数据的长度 2或4
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void FM24C256_Write_NByte(uint8_t FM_ADD,uint16_t RAM_ADD,uint8_t n, char nBuffer[n])
{
	uint8_t i;
  FM_IIC_Start();                //启动IIC总线
	FM_IIC_Send_Byte(FM_ADD);      //发送写命令以及器件的选择  0xa0=器件1的写  0xa4器件2的写
	FM_IIC_Wait_Ack();               //等待应答
	FM_IIC_Send_Byte(RAM_ADD>>8);  //发送内存地址1的高8位
	FM_IIC_Wait_Ack();
	FM_IIC_Send_Byte(RAM_ADD%256); //发送内存地址1的低8位
	FM_IIC_Wait_Ack();
	for(i=0;i<n;i++)
	{
		FM_IIC_Send_Byte(nBuffer[i]);         //发送一字节数据
		FM_IIC_Wait_Ack(); 
		
	}
	FM_IIC_Stop();                //停止  
	HAL_Delay(10);
}


/**
  *****************************************************************************
  * @Name   : 从FM24C256连续读M个字节数据  Sequential   Read
  *
  * @Brief  : none
  *
  * @Input  : FM_ADD：FM24C256的地址和写命令   0xa7或者0xa3
	            RAM_ADD:开始读出的内存地址  范围0x0000---0x7fff
  *           dat：    要读出的数据16位或者32位
*             len:     要写入数据的长度 2或4
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void FM24C256_Read_MByte(uint8_t FM_ADD,uint16_t RAM_ADD,uint8_t m,char *nBuffer)
{
	uint8_t i;
	FM_IIC_Start();                //启动IIC总线
	FM_IIC_Send_Byte(FM_ADD+1);      //发送读指令   0xa1=0xa0+1器件1的读   0xa5=0xa4+1器件2的读 
	FM_IIC_Wait_Ack();
	for(i=0;i<m;i++)
	{
		*nBuffer=FM_IIC_Read_Byte(0);       //读数据(FM_ADD);		//读数据
		FM_IIC_Wait_Ack();
		FM_ADD++;
		nBuffer++;
		
	}
	FM_IIC_NAck();
	FM_IIC_Stop(); 		//停止  
	
}
/**
  *****************************************************************************
  * @Name   : 检查FM24C256是否正常
  *
  * @Brief  : none
  *
  * @Input  : FM_ADD：FM24C256的地址和写命令   0xa7或者0xa3
	            RAM_DD: 用最后一个地址0x7fff存储0xff数据，然后在读出地址
                      0x7fff的数据和0xff对比
  * @Output : none
  *
  * @Return : 3俩内存都成功；2内存1成功，内存2失败；  1内存2成功，内存1失败；0都失败
  *****************************************************************************
**/
int8_t FM24C256_Check(void)
{
	uint8_t temp,data;
	temp=FM24C256_Read_Byte(0xa0,0x7fff);
	data=FM24C256_Read_Byte(0xa4,0x7fff);
	if(temp==0xff&data==0xff)
		return 3;
	else
	{
		FM24C256_Write_Byte(0xa0,0x7fff,0xff);
		FM24C256_Write_Byte(0xa4,0x7fff,0xff);
		
		temp=FM24C256_Read_Byte(0xa0,0x7fff);
		data=FM24C256_Read_Byte(0xa4,0x7fff);
		if(temp==0xff&data==0xff)
			return 0;
		
		else if((temp==0xff)&(data!=0xff))
				return -1;
		else if((data==0xff)&(temp!=0xff))
				return -1;
		else if((data!=0xff)&(temp!=0xff))
				return -1;
		}
	return 0;
}


//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24C512_Read(uint8_t FM_ADD,uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=FM24C256_Read_Byte(FM_ADD,ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24C512_Write(uint16_t FM_ADD,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		FM24C256_Write_Byte(FM_ADD,WriteAddr,*pBuffer);
		//WriteAddr++;//读写地址自动加1
		pBuffer++;
	}
}



