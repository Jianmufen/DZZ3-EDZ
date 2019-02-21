/****************************************Copyright (c)****************************************************
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File name:				LCM_DRIVE.h
** Latest modified Date:    2008-03-07
** Latest Version:          1.0
** Descriptions:            控制器：KS0107B；取模方式：纵向字节倒序
**
**--------------------------------------------------------------------------------------------------------
** Created by:              JGW
** Created date:            2008-10-27
** Version:                 1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/

#ifndef __LCD_DRIVE_H
#define __LCD_DRIVE_H


#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "main.h"

   
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


/**
  * @brief  LCD Control Interface pins
  */
/* LCD -- BL:lcd backlight */
#define LCD_BL_PIN                              GPIO_PIN_9
#define LCD_BL_PORT                             GPIOB
#define LCD_BL_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE()
#define LCD_BL_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOB_CLK_DISABLE()

/* LCD -- BUSY */   /* unused */
#define LCD_BUSY_PIN                            GPIO_PIN_7
#define LCD_BUSY_PORT                           GPIOE
#define LCD_BUSY_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOE_CLK_ENABLE()
#define LCD_BUSY_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOE_CLK_DISABLE()
   
/* LCD -- EN */
#define LCD_EN_PIN                              GPIO_PIN_11
#define LCD_EN_PORT                             GPIOE
#define LCD_EN_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOE_CLK_ENABLE()
#define LCD_EN_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOE_CLK_DISABLE()
   
/* LCD -- DI */
#define LCD_DI_PIN                              GPIO_PIN_8
#define LCD_DI_PORT                             GPIOE
#define LCD_DI_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOE_CLK_ENABLE()
#define LCD_DI_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOE_CLK_DISABLE()
   
/* LCD -- RW */  /* unused */
#define LCD_RW_PIN                              GPIO_PIN_11
#define LCD_RW_PORT                             GPIOE
#define LCD_RW_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOE_CLK_ENABLE()
#define LCD_RW_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOE_CLK_DISABLE()
   
/* LCD -- CS1 */
#define LCD_CS1_PIN                             GPIO_PIN_9
#define LCD_CS1_PORT                            GPIOE
#define LCD_CS1_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOE_CLK_ENABLE()
#define LCD_CS1_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOE_CLK_DISABLE()
   
/* LCD -- CS2 */
#define LCD_CS2_PIN                             GPIO_PIN_10
#define LCD_CS2_PORT                            GPIOE
#define LCD_CS2_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOE_CLK_ENABLE()
#define LCD_CS2_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOE_CLK_DISABLE()

   
/* LCD -- DATA */
#define LCD_DATA_PINS                           ( GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 )
#define LCD_DATA_PORT                            GPIOE
#define LCD_DATA_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOE_CLK_ENABLE()
#define LCD_DATA_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOE_CLK_DISABLE()
   
   
/**
  * @brief  LCD Control Lines management
  */  
    
/* BL */
#define LCD_BL_ON()                           HAL_GPIO_WritePin(LCD_BL_PORT, LCD_BL_PIN, GPIO_PIN_RESET)
#define LCD_BL_OFF()                          HAL_GPIO_WritePin(LCD_BL_PORT, LCD_BL_PIN, GPIO_PIN_SET)
    
/* EN */
#define LCD_EN_LOW()                           HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET)
#define LCD_EN_HIGH()                          HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET)
   
/* DI */
#define LCD_DI_LOW()                           HAL_GPIO_WritePin(LCD_DI_PORT, LCD_DI_PIN, GPIO_PIN_RESET)
#define LCD_DI_HIGH()                          HAL_GPIO_WritePin(LCD_DI_PORT, LCD_DI_PIN, GPIO_PIN_SET)

/* RW */  /* unused */
#define LCD_RW_LOW()                           /*HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET)*/
#define LCD_RW_HIGH()                          /*HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_SET)*/
   
/* CS1 */
#define LCD_CS1_LOW()                          HAL_GPIO_WritePin(LCD_CS1_PORT, LCD_CS1_PIN, GPIO_PIN_RESET)
#define LCD_CS1_HIGH()                         HAL_GPIO_WritePin(LCD_CS1_PORT, LCD_CS1_PIN, GPIO_PIN_SET)
   
/* CS2 */
#define LCD_CS2_LOW()                          HAL_GPIO_WritePin(LCD_CS2_PORT, LCD_CS2_PIN, GPIO_PIN_RESET)
#define LCD_CS2_HIGH()                         HAL_GPIO_WritePin(LCD_CS2_PORT, LCD_CS2_PIN, GPIO_PIN_SET)
   
   
/**
  * @}
  */

/**
  * @}
  */

extern unsigned char const CHSStr1[];

/***********************************************************************
* 名称：	LCD_Check_Busy()
* 功能：	等待LCD允许操作
* 入口参数：
***********************************************************************/
void LCD_Check_Busy(void);

/***********************************************************************
* 名称：LCM_WriteByte()
* 功能：向指定点写数据(一字节)。
* 入口参数：x 		x坐标值(0-127)
*	        y       y坐标值(0-8)
*          wrdata	所要写的数据
* 出口参数：无
* 说明：会更新disp_buf相应存储单元
***********************************************************************/
void  LCM_WriteByte(unsigned char x, unsigned char  y, unsigned char wrdata);

/***********************************************************************
* 名称：LCM_DispFill()
* 功能：向显示屏填充数据
* 入口参数：filldata  	要写入LCM的填充数据
* 出口参数：无
* 说明：会更新disp_buf相应存储单元
***********************************************************************/
void  LCM_DispFill(unsigned char filldata);

/***********************************************************************
* 名称：LCM_DispIni()
* 功能：LCM显示初始化。使能显示，设置显示起始行为0并清屏。
* 入口参数：无
* 出口参数：无
***********************************************************************/
void  LCM_DispInit(void);

/***********************************************************************
* 名称：LCM_DispChar()
* 功能：指定地址显示字符。
* 入口参数：disp_cy		显示行值(0-7)
*	   disp_cx		显示列值(0-15)
*	   dispdata		所要显示的字符(ASCII码)
* 注：支持显示字符0-9、A-Z、a-z及空格，字符显示格式为5*7，模为8*8，所以
*     屏幕显示为8*16(共8行，每行16个字符)。
***********************************************************************/
void LCM_DispChar(unsigned char disp_cy, unsigned char disp_cx, char dispdata);

/***********************************************************************
* 名称：LCM_DispStr()
* 功能：字符串显示输出。
* 入口参数：disp_cy 	显示起始行(0-7)
*	   disp_cx      显示起始列(0-15)
*	   disp_str	字串指针
* 出口参数：无
* 注：支持显示字符0-9、A-Z、a-z及空格，字符显示格式为5*7，模为8*8，所以屏幕显示
*     为8*16(共8行，每行16个字符)。
***********************************************************************/
void  LCM_DispStr(unsigned char disp_cy, unsigned char disp_cx, const char *disp_str);

/***********************************************************************
* 名称：LCM_DispCHS()
* 功能：字符串显示输出。
* 入口参数：DisCHS_y    显示起始行(0-3)
*	        DisCHS_x    显示起始列(0-7)
*	        pStr	    字串指针
* 出口参数：无
* 注：支持显示汉字字符，模为16*16，所以屏幕显示
*     为4*8(共4行，每行8个字符)。添加高亮显示。
***********************************************************************/
void LCM_DispCHS(unsigned char DisCHS_y, unsigned char DisCHS_x,const unsigned char *pStr,unsigned char highlight);

/***********************************************************************
* 名称：LCM_DispCHSStr()
* 功能：字符串显示输出。
* 入口参数：DisCHS_y    显示起始行(0-3)
*	        DisCHS_x    显示起始列(0-7)
*	        pStr	    字串指针
* 出口参数：无
* 注：支持显示汉字字符，模为16*16，所以屏幕显示
*     为4*8(共4行，每行8个字符)。添加高亮显示。
***********************************************************************/
void LCM_DispCHSStr(unsigned char DisCHS_y, unsigned char DisCHS_x,unsigned char Num,const unsigned char *pStr,unsigned char highlight);


/***********************************************************************
* 名称：LCM_DispChar8_16()
* 功能：指定地址显示字符。
* 入口参数：disp_cy		显示行值(0-3)
*	   disp_cx		显示列值(0-15)
*	   dispdata		所要显示的字符(ASCII码)
* 注：支持显示字符0-9、A-Z、a-z及空格，字符显示格式为5*7，模为8*16，所以
*     屏幕显示为4*16(共4行，每行16个字符)。支持高亮显示。
***********************************************************************/
void LCM_DispChar8_16(unsigned char disp_cy,unsigned char disp_cx,char dispdata,unsigned char highlight);
/***********************************************************************
* 名称：LCM_DispStr8_16()
* 功能：字符串显示输出。
* 入口参数：disp_cy 	显示起始行(0-3)
*	   disp_cx      显示起始列(0-15)
*	   disp_str	字串指针
* 出口参数：无
* 注：支持显示字符0-9、A-Z、a-z及空格，字符显示格式为5*7，模为8*16，所以屏幕显示
*     为4*16(共4行，每行16个字符)。
***********************************************************************/
void  LCM_DispStr8_16(unsigned char disp_cy, unsigned char disp_cx, const char *disp_str,unsigned char highlight);




#ifdef __cplusplus
}
#endif
#endif  /* __LCD_DRIVE_H */
