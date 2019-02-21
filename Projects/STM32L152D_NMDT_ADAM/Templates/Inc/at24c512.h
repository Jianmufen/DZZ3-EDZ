#ifndef _at24c512_h_ 
#define _at24c512_h_


/**
  ****************************** Support C++ **********************************
**/
#ifdef __cplusplus
	extern "C"{
#endif		
/**
  *****************************************************************************
**/
		

/******************************************************************************
                             外部函数头文件
                  应用到不同的外设头文件请在这里修改即可                        
******************************************************************************/

#include "at_iic.h"

/******************************************************************************
                                 参数宏定义
******************************************************************************/		
#define FM24C256_Check_Data                       (unsigned char)0xFF  //检测用，可用其他数值

#define FM24C256_Write1                            (unsigned char)0xa4  //内存1的写命令
#define FM24C256_Read1                             (unsigned char)0xa5  //内存1的读命令
	
#define FM24C256_Write2                            (unsigned char)0xa0  //内存2的写命令
#define FM24C256_Read2                             (unsigned char)0xa1  //内存2的读命令
	

void FM24C256_Write_NByte(uint8_t FM_ADD,uint16_t RAM_ADD,uint8_t n, char nBuffer[n]);
uint8_t FM24C256_Read_Byte(uint8_t FM_ADD,uint16_t RAM_ADD);
void FM24C256_Write_Byte(uint8_t FM_ADD,uint16_t RAM_ADD, char dat);
uint8_t FM24C256_Check(void);
void FM24C256_Read_MByte(uint8_t FM_ADD,uint16_t RAM_ADD,uint8_t m,char *nBuffer);

void AT24C512_Read(uint8_t FM_ADD,uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
void AT24C512_Write(uint16_t FM_ADD,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);




#ifdef __cplusplus
	}
#endif
/**
  *****************************************************************************
**/


#endif 
	
	