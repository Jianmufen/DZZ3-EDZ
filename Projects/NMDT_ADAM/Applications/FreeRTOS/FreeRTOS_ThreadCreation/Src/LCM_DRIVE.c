/***********************************************************************
* 文件名：LCM_DRIVE.C
* 功能：图形液晶TG12864B-2驱动程序。
* 说明：在LCM_DRIVE.H文件中定义了LCM操作地址，左半屏的写命令操作地址为2004H，写
*      数据操作地址为2005H，右半屏的写命令操作地址为2000H，写数据操作地址为2001H；
*      由于GRAPHICS.C中使用了disp_buf作为作图缓冲区，所以LCM_WriteByte()、LCM_
*      DispFill()均要更新disp_buf。
***********************************************************************/
#include	"LCM_DRIVE.h"
#include        "Font8_16.h"    /* 8*16点阵字符 */



#define LCM_L  	1
#define LCM_R 	2

/* 定义LCM像素数宏 */
#define  LCM_XMAX	(128)
#define  LCM_YMAX	(64)

/*#define DIS_DLY()	do{ \
asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop"); \
  asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop"); \
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop"); \
      asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop"); \
        asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop"); \
          asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop"); \
} while(0)*/


/* 定义LCM操作的命令字 */
#define	LCM_DISPON	0x3f									/* 打开LCM显示 							*/
#define LCM_STARTROW	0xc0									/* 显示起始行0，可以用LCM_STARTROW+x设置起始行。(x<64) 		*/
#define	LCM_ADDRSTRY	0xb8									/* 页起始地址，可以用LCM_ADDRSTRX+x设置当前页(即行)。(x<8) 	*/
#define	LCM_ADDRSTRX	0x40									/* 列起始地址，可以用LCM_ADDRSTRY+x设置当前列(即更)。(x<64) */




/** @addtogroup Private_Functions
  * @{
  */
static void LCD_IO_Init(void);
__STATIC_INLINE void DIS_DLY(void);
__STATIC_INLINE void LCD_IO_Write(unsigned char command);
static void LCM_Wr1Command(unsigned char wrdata);
static void LCM_Wr2Command(unsigned char wrdata);
static void LCM_Wr1Data(unsigned char wrdata);
static void LCM_Wr2Data(unsigned char wrdata);



/***********************************************************************
* 名称：	LCD_IO_Init()
* 功能：	初始化LCD用到的IO口
* 入口参数：
***********************************************************************/
static void LCD_IO_Init(void)
{
  GPIO_InitTypeDef  gpioinitstruct = {0};

  /* LCD_BL Periph clock enable */
  LCD_BL_GPIO_CLK_ENABLE();
  /* Configure LCD_BL_PIN pin */
  gpioinitstruct.Pin    = LCD_BL_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(LCD_BL_PORT, &gpioinitstruct);
  
  
  /* LCD_EN Periph clock enable */
  LCD_EN_GPIO_CLK_ENABLE();
  /* Configure LCD_EN_PIN pin */
  gpioinitstruct.Pin    = LCD_EN_PIN;
  HAL_GPIO_Init(LCD_EN_PORT, &gpioinitstruct);
  
  
  /* LCD_DI Periph clock enable */
  LCD_DI_GPIO_CLK_ENABLE();
  /* Configure LCD_EN_PIN pin */
  gpioinitstruct.Pin    = LCD_DI_PIN;
  HAL_GPIO_Init(LCD_DI_PORT, &gpioinitstruct);
  
  
  /* LCD_RW Periph clock enable */  /* unused */
  //LCD_RW_GPIO_CLK_ENABLE();
  /* Configure LCD_EN_PIN pin */
  //gpioinitstruct.Pin    = LCD_RW_PIN;
  //HAL_GPIO_Init(LCD_RW_PORT, &gpioinitstruct);
  
  
  /* LCD_CS1 Periph clock enable */
  LCD_CS1_GPIO_CLK_ENABLE();
  /* Configure LCD_EN_PIN pin */
  gpioinitstruct.Pin    = LCD_CS1_PIN;
  HAL_GPIO_Init(LCD_CS1_PORT, &gpioinitstruct);
  
  
  /* LCD_CS2 Periph clock enable */
  LCD_CS2_GPIO_CLK_ENABLE();
  /* Configure LCD_EN_PIN pin */
  gpioinitstruct.Pin    = LCD_CS2_PIN;
  HAL_GPIO_Init(LCD_CS2_PORT, &gpioinitstruct);
  
  
  /* LCD_DATA Periph clock enable */
  LCD_DATA_GPIO_CLK_ENABLE();
  /* Configure LCD_DATA_PINS pin */
  gpioinitstruct.Pin    = LCD_DATA_PINS;
  HAL_GPIO_Init(LCD_DATA_PORT, &gpioinitstruct);
  
  
}

__STATIC_INLINE void DIS_DLY(void)
{
  /*asm("nop");
  asm("nop");
  return;*/
  
  //在这里优化速度，延时过小显示不正常，在free rtos中需要更长一点
  volatile uint32_t i=50;
  for(i=0;i<50;i++);
	
//  while(i--)
//  {
//    asm("nop");
//  }
}


/***********************************************************************
* 名称：	LCD_IO_Write()
* 功能：	初始化LCD用到的IO口
* 入口参数：command  	要写入LCM的命令字
***********************************************************************/
__STATIC_INLINE void LCD_IO_Write(unsigned char command)
{
  /*GPIOE->ODR&=0xFF00;
  GPIOE->ODR|=command;
  return;*/
  
  
  /*command=~command;
  uint32_t bsrr=(((uint32_t)command << 16)&0x00FF0000) |
                        ((~((uint32_t)command))&0x00FF);*/
  uint32_t bsrr=((~(uint32_t)command << 16)&0x00FF0000) | (command&0x00FF);
  //printf("cmd:0x%02X,bsrr:0x%08X\r\n",command,bsrr);
  
  LCD_DATA_PORT->BSRR = bsrr;
}

/**
  * @}
  */

/**
  * @}
  */





/***********************************************************************
* 名称：	LCD_Check_Busy()
* 功能：	等待LCD允许操作
* 入口参数：
***********************************************************************/
void LCD_Check_Busy(void)
{
	/*while(1) {
		LCD_EN_LOW();
		LCD_DI_LOW();
		LCD_RW = 1;
		P0 = 0xff;
		LCD_EN_HIGH();
		DIS_DLY();
		if(!LCD_Busy) break;
    }
    LCD_EN = 0;*/
}

/***********************************************************************
* 名称：LCM_Wr1Command()
* 功能：写命令子程序，所选屏为左半屏(CS1)。
* 入口参数：command  	要写入LCM的命令字
***********************************************************************/
static void LCM_Wr1Command(unsigned char wrdata)
{
    LCD_CS1_HIGH();
    LCD_CS2_LOW();
    
    /*DIS_DLY();*/

    LCD_EN_LOW();           										/* 使能信号为Ｌ 					*/
    LCD_DI_LOW();           										/* 置寄存器选择为Ｌ 				*/
    LCD_RW_LOW();           										/* 置读／写选择为Ｌ 				*/
    LCD_IO_Write(wrdata);   										/* 将控制指令代码输出到的数据端口 	        */
    LCD_EN_HIGH();           										/* 使能信号为Ｈ 					*/
    DIS_DLY();          										/* 延时2us 							*/
    LCD_EN_LOW();           										/* 使能信号为Ｌ 					*/
}

/***********************************************************************
* 名称：LCM_Wr2Command()
* 功能：写命令子程序，所选屏为右半屏(CS2)。
* 入口参数：command  	要写入LCM的命令字
***********************************************************************/
static void LCM_Wr2Command(unsigned char wrdata)
{
    LCD_CS1_LOW();
    LCD_CS2_HIGH();
    
    /*DIS_DLY();*/

    LCD_EN_LOW();           										/* 使能信号为Ｌ 					*/
    LCD_DI_LOW();           										/* 置寄存器选择为Ｌ 				*/
    LCD_RW_LOW();           										/* 置读／写选择为Ｌ 				*/
    LCD_IO_Write(wrdata);   										/* 将控制指令代码输出到的数据端口 	        */
    LCD_EN_HIGH();           										/* 使能信号为Ｈ 					*/
    DIS_DLY();          										/* 延时2us 							*/
    LCD_EN_LOW();           										/* 使能信号为Ｌ 					*/
}

/***********************************************************************
* 名称：LCM_Wr1Data()
* 功能：写数据子程序，所选屏为左半屏(CS1)。
* 入口参数：wrdata  	要写入LCM的数据
***********************************************************************/
static void LCM_Wr1Data(unsigned char wrdata)
{
	LCD_CS1_HIGH();
	LCD_CS2_LOW();
        
        /*DIS_DLY();*/

	LCD_Check_Busy();
	LCD_EN_LOW();
	LCD_DI_HIGH();             									/* 置寄存器选择为Ｈ 				*/
	LCD_RW_LOW();             									/* 置读／写选择为Ｌ 				*/
	LCD_IO_Write(wrdata);   									/* 将控制指令代码输出到的数据端口 	        */
	LCD_EN_HIGH();             									/* 使能信号为Ｈ 					*/
	DIS_DLY();            										/* 延时2us 							*/
	LCD_EN_LOW();             									/* 使能信号为Ｌ 					*/
}

/***********************************************************************
* 名称：LCM_Wr2Data()
* 功能：写数据子程序，所选屏为右半屏(CS2)。
* 入口参数：wrdata  	要写入LCM的数据
***********************************************************************/
static void LCM_Wr2Data(unsigned char wrdata)
{
	LCD_CS1_LOW();
	LCD_CS2_HIGH();
        
        /*DIS_DLY();*/

	LCD_Check_Busy();
	LCD_EN_LOW();
	LCD_DI_HIGH();             									/* 置寄存器选择为Ｈ 				*/
	LCD_RW_LOW();             									/* 置读／写选择为Ｌ 				*/
	LCD_IO_Write(wrdata);   									/* 将控制指令代码输出到的数据端口 	        */
	LCD_EN_HIGH();             									/* 使能信号为Ｈ 					*/
	DIS_DLY();            										/* 延时2us 							*/
	LCD_EN_LOW();             									/* 使能信号为Ｌ 					*/
}

/***********************************************************************
* 名称：LCM_WriteByte()
* 功能：向指定点写数据(一字节)。
* 入口参数：x 		x坐标值(0-127)
*	        y       y坐标值(0-8)
*          wrdata	所要写的数据
* 出口参数：无
* 说明：会更新disp_buf相应存储单元
***********************************************************************/
void  LCM_WriteByte(unsigned char x, unsigned char  y, unsigned char wrdata)
{
    x &= 0x7f;                                                  // 参数过滤
    y &= 0x07;
//   y = y>>3;
    if(x<64) {													// 选择液晶控制芯片(即CS1--控制前64个点，CS2--控制后64个点)
        LCM_Wr1Command(LCM_ADDRSTRX+x);                         // 设置当前列地址，即x坐标
        LCM_Wr1Command(LCM_ADDRSTRY+y);							// 设置当前页地址，即y坐标
//        for(x=0; x<5; x++);									// 短延时
        LCM_Wr1Data(wrdata);
    }
    else {
        x = x-64;                                               // 调整x变量值
        LCM_Wr2Command(LCM_ADDRSTRX+x);
        LCM_Wr2Command(LCM_ADDRSTRY+y);
//        for(x=0; x<5; x++);
        LCM_Wr2Data(wrdata);
    }
}

/***********************************************************************
* 名称：LCM_DispFill()
* 功能：向显示屏填充数据
* 入口参数：filldata  	要写入LCM的填充数据
* 出口参数：无
* 说明：会更新disp_buf相应存储单元
***********************************************************************/
void  LCM_DispFill(unsigned char filldata)
{
	unsigned char  x, y;

    LCM_Wr1Command(LCM_STARTROW);								// 设置显示起始行为0
    LCM_Wr2Command(LCM_STARTROW);

    for(y=0; y<8; y++) {
        LCM_Wr1Command(LCM_ADDRSTRY+y);                         // 设置页(行)地址
        LCM_Wr1Command(LCM_ADDRSTRX);							// 设置列地址
        LCM_Wr2Command(LCM_ADDRSTRY+y);
        LCM_Wr2Command(LCM_ADDRSTRX);

        for(x=0; x<64; x++) {
            LCM_Wr1Data(filldata);
            LCM_Wr2Data(filldata);
        }
    }
}
/***********************************************************************
* 名称：LCM_DispInit()
* 功能：LCM显示初始化。使能显示，设置显示起始行为0并清屏。
* 入口参数：无
* 出口参数：无
***********************************************************************/
void  LCM_DispInit(void)
{
	/*LCM_RST = 0;*/									/*	复位驱动芯片						*/
	/*DIS_DLY();*/
	/*LCM_RST = 1;*/
  
        LCD_IO_Init();                                                                          /*      初始化LCD用到的IO口                                      */

	LCM_Wr1Command(LCM_DISPON);								/*	打开显示							*/
	LCM_Wr1Command(LCM_STARTROW);								/*	设置显示起始行为0					*/
	LCM_Wr2Command(LCM_DISPON);
        LCM_Wr2Command(LCM_STARTROW);

	LCM_DispFill(0);									/*	清屏								*/

	LCM_Wr1Command(LCM_ADDRSTRY+0);								/*	设置页(行)地址						*/
	LCM_Wr1Command(LCM_ADDRSTRX+0);								/*	设置列地址，即列					*/
	LCM_Wr2Command(LCM_ADDRSTRY+0);
	LCM_Wr2Command(LCM_ADDRSTRX+0);
}

/* ASCII码对应的点阵数据表 */
unsigned char const  ASCII_TAB20[80] = {
				0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x9e, 0x00, 0x00,
         		0x00, 0x0e, 0x00, 0x0e, 0x00,
         		0x28, 0xfe, 0x28, 0xfe, 0x28,
         		0x48, 0x54, 0xfe, 0x54, 0x24,
         		0x46, 0x26, 0x10, 0xc8, 0xc4,
         		0x6c, 0x92, 0xaa, 0x44, 0xa0,
         		0x00, 0x0a, 0x06, 0x00, 0x00,
			 	0x00, 0x38, 0x44, 0x82, 0x00,
			 	0x00, 0x82, 0x44, 0x38, 0x00,
			 	0x28, 0x10, 0x7c, 0x10, 0x28,
			 	0x10, 0x10, 0x7c, 0x10, 0x10,
			 	0x00, 0xa0, 0x60, 0x00, 0x00,
			 	0x10, 0x10, 0x10, 0x10, 0x10,
			 	0x00, 0xc0, 0xc0, 0x00, 0x00,
			 	0x40, 0x20, 0x10, 0x08, 0x04
		               };

unsigned char const  ASCII_TAB30[80] = {
				0x7C, 0xA2, 0x92, 0x8A, 0x7C,
				0x00, 0x84, 0xFE, 0x80, 0x00,
				0x84, 0xC2, 0xA2, 0x92, 0x8C,
				0x42, 0x82, 0x8A, 0x96, 0x62,
				0x30, 0x28, 0x24, 0xFE, 0x20,
				0x4E, 0x8A, 0x8A, 0x8A, 0x72,
				0x78, 0x94, 0x92, 0x92, 0x60,
				0x02, 0xE2, 0x12, 0x0A, 0x06,
				0x6C, 0x92, 0x92, 0x92, 0x6C,
 				0x0C, 0x92, 0x92, 0x52, 0x3C,
				0x00, 0x6C, 0x6C, 0x00, 0x00,
				0x00, 0xAC, 0x6C, 0x00, 0x00,
				0x10, 0x28, 0x44, 0x82, 0x00,
				0x28, 0x28, 0x28, 0x28, 0x28,
				0x00, 0x82, 0x44, 0x28, 0x10,
				0x04, 0x02, 0xA2, 0x12, 0x0C
		       	       };

unsigned char const  ASCII_TAB40[80] = {
				0x64, 0x92, 0xF2, 0x82, 0x7C,
				0xFC, 0x22, 0x22, 0x22, 0xFC,
		 		0xFE, 0x92, 0x92, 0x92, 0x6C,
				0x7C, 0x82, 0x82, 0x82, 0x44,
				0xFE, 0x82, 0x82, 0x44, 0x38,
				0xFE, 0x92, 0x92, 0x92, 0x82,
				0xFE, 0x12, 0x12, 0x12, 0x02,
				0x7C, 0x82, 0x92, 0x92, 0xF4,
				0xFE, 0x10, 0x10, 0x10, 0xFE,
				0x00, 0x82, 0xFE, 0x82, 0x00,
				0x40, 0x80, 0x82, 0x7E, 0x02,
				0xFE, 0x10, 0x28, 0x44, 0x82,
				0xFE, 0x80, 0x80, 0x80, 0x80,
				0xFE, 0x04, 0x18, 0x04, 0xFE,
				0xFE, 0x08, 0x10, 0x20, 0xFE,
				0x7C, 0x82, 0x82, 0x82, 0x7C
		       	       };

unsigned char const  ASCII_TAB50[80] = {
				0xFE, 0x12, 0x12, 0x12, 0x0C,
				0x7C, 0x82, 0xA2, 0x42, 0xBC,
				0xFE, 0x12, 0x32, 0x52, 0x8C,
				0x8C, 0x92, 0x92, 0x92, 0x62,
				0x02, 0x02, 0xFE, 0x02, 0x02,
				0x7E, 0x80, 0x80, 0x80, 0x7E,
				0x3E, 0x40, 0x80, 0x40, 0x3E,
				0x7E, 0x80, 0x70, 0x80, 0x7E,
				0xC6, 0x28, 0x10, 0x28, 0xC6,
				0x0E, 0x10, 0xE0, 0x10, 0x0E,
				0xC2, 0xA2, 0x92, 0x8A, 0x86,
				0x00, 0xFE, 0x82, 0x82, 0x00,
				0x04, 0x08, 0x10, 0x20, 0x40,
				0x00, 0x82, 0x82, 0xFE, 0x00,
				0x08, 0x04, 0x02, 0x04, 0x08,
				0x80, 0x80, 0x80, 0x80, 0x80
		      	       };

unsigned char const  ASCII_TAB60[80] = {
				0x00, 0x02, 0x04, 0x08, 0x00,
				0x40, 0xA8, 0xA8, 0xA8, 0xF0,
				0xFE, 0x90, 0x88, 0x88, 0x70,
				0x70, 0x88, 0x88, 0x88, 0x40,
				0x70, 0x88, 0x88, 0x90, 0xFE,
				0x70, 0xA8, 0xA8, 0xA8, 0x30,
				0x10, 0xFC, 0x12, 0x02, 0x04,
				0x18, 0xA4, 0xA4, 0xA4, 0x7C,
				0xFE, 0x10, 0x08, 0x08, 0xF0,
				0x00, 0x88, 0xFA, 0x80, 0x00,
				0x40, 0x80, 0x88, 0x7A, 0x00,
				0xFE, 0x20, 0x50, 0x88, 0x00,
				0x00, 0x82, 0xFE, 0x80, 0x00,
				0xF8, 0x08, 0x30, 0x08, 0xF8,
				0xF8, 0x10, 0x08, 0x08, 0xF0,
				0x70, 0x88, 0x88, 0x88, 0x70
		               };

unsigned char const  ASCII_TAB70[80] = {
				0xF8, 0x28, 0x28, 0x28, 0x10,
				0x10, 0x28, 0x28, 0x30, 0xF8,
				0xF8, 0x10, 0x08, 0x08, 0x10,
				0x90, 0xA8, 0xA8, 0xA8, 0x40,
				0x08, 0x7E, 0x88, 0x80, 0x40,
				0x78, 0x80, 0x80, 0x40, 0xF8,
				0x38, 0x40, 0x80, 0x40, 0x38,
				0x78, 0x80, 0x60, 0x80, 0x78,
				0x88, 0x50, 0x20, 0x50, 0x88,
				0x18, 0xA0, 0xA0, 0xA0, 0x78,
				0x88, 0xC8, 0xA8, 0x98, 0x88,
				0x00, 0x10, 0x6C, 0x82, 0x00,
				0x00, 0x00, 0xFE, 0x00, 0x00,
				0x00, 0x82, 0x6C, 0x10, 0x00,
				0x10, 0x10, 0x54, 0x38, 0x10,
				0x10, 0x38, 0x54, 0x10, 0x10
		       	       };
/***********************************************************************
* 名称：LCM_DispChar()
* 功能：指定地址显示字符。
* 入口参数：disp_cy		显示行值(0-7)
*	   disp_cx		显示列值(0-15)
*	   dispdata		所要显示的字符(ASCII码)
* 注：支持显示字符0-9、A-Z、a-z及空格，字符显示格式为5*7，模为8*8，所以
*     屏幕显示为8*16(共8行，每行16个字符)。
***********************************************************************/
void LCM_DispChar(unsigned char disp_cy, unsigned char disp_cx, char dispdata)
{
    unsigned char  const  *pchardata;
    unsigned char  i;

    disp_cy = disp_cy&0x07;										// 参数过滤
    disp_cx = disp_cx&0x0f;

    /*
    *	先要找出显示数据的类型，并设置相应的点阵数据表，然后设置指针，以取得对应的点阵数据。
    */

    switch(dispdata&0xf0) {
        case 0x20:
            dispdata = (dispdata&0x0f)*5;
            pchardata = &ASCII_TAB20[dispdata];
            break;
        case 0x30:
            dispdata = (dispdata&0x0f)*5;
            pchardata = &ASCII_TAB30[dispdata];
            break;
        case 0x40:
            dispdata = (dispdata&0x0f)*5;
            pchardata = &ASCII_TAB40[dispdata];
            break;
        case 0x50:
            dispdata = (dispdata&0x0f)*5;
            pchardata = &ASCII_TAB50[dispdata];
            break;
        case 0x60:
            dispdata = (dispdata&0x0f)*5;
            pchardata = &ASCII_TAB60[dispdata];
            break;
        case 0x70:
            dispdata = (dispdata&0x0f)*5;
            pchardata = &ASCII_TAB70[dispdata];
            break;
        default:
            pchardata = &ASCII_TAB20[0];
            break;
   } 															// end of switch(dispdata&0xf0)...


   if( (disp_cx&0x08) == 0 ) {									// 选择液晶控制芯片(即CS1--控制前8个字符，CS2--控制后8个字符)
        i = disp_cx << 3;
        LCM_Wr1Command(LCM_ADDRSTRX+i);							// 设置当前列地址，即列
        LCM_Wr1Command(LCM_ADDRSTRY+disp_cy);					// 设置当前页地址，即行

        LCM_Wr1Data(0x00);										// 显示一列空格
        for(i=0; i<5; i++) {
            LCM_Wr1Data(*pchardata);							// 发送数据
            pchardata++;
        }

        LCM_Wr1Data(0x00);
        LCM_Wr1Data(0x00);
    }
    else {
        i = (disp_cx&0x07)<<3;									// 若Y>7,则选取用CS2并且地址值要先减去8，再乘以8
        LCM_Wr2Command(LCM_ADDRSTRX+i);
        LCM_Wr2Command(LCM_ADDRSTRY+disp_cy);					// 设置当前页地址，即行

        LCM_Wr2Data(0x00);										// 显示一列空格
        for(i=0; i<5; i++) {
            LCM_Wr2Data(*pchardata);							// 发送数据
            pchardata++;
        }
        LCM_Wr2Data(0x00);
        LCM_Wr2Data(0x00);
   }
}


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
void  LCM_DispStr(unsigned char disp_cy, unsigned char disp_cx, const char *disp_str)
{
    while( *disp_str != '\0') {
        disp_cy = (disp_cy & 0x07);								// 参数过滤
        disp_cx = (disp_cx & 0x3f);
        LCM_DispChar(disp_cy, disp_cx, *disp_str); 				// 显示字符

        disp_str++;												// 指向下一字符数据
        disp_cx++;
        if (disp_cx > 15) {										// 指向下一显示行
            disp_cx = 0;
            disp_cy++;
        }
    }
}
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
void LCM_DispCHS(unsigned char DisCHS_y, unsigned char DisCHS_x,const unsigned char *pStr,unsigned char highlight)
{
    unsigned char i = 0;//, j = 0;
    const unsigned char *pStrTemp = pStr;
    unsigned char disp_cx = DisCHS_x;
    unsigned char disp_cy = DisCHS_y;
    unsigned char disp=0;

    if (DisCHS_x > 7) DisCHS_x = 7;
    if (DisCHS_y > 3) DisCHS_y = 3;

    if (DisCHS_x <= 3) {                                        //  左半屏
        disp_cx <<= 4;
        disp_cy <<= 1;
        LCM_Wr1Command(LCM_ADDRSTRX + disp_cx);                 // 设置当前列地址，即列
        LCM_Wr1Command(LCM_ADDRSTRY + disp_cy);                 // 设置当前页地址，即行
        for(i=0; i<16; i++) {
            disp = highlight ? ~(*(pStrTemp + i)) : (*(pStrTemp + i));  /* 反显 */
          
            LCM_Wr1Data(disp);						// 发送数据
        }

        LCM_Wr1Command(LCM_ADDRSTRX + disp_cx); 				// 设置当前列地址，即列
        LCM_Wr1Command(LCM_ADDRSTRY + disp_cy + 1);				// 设置当前页地址，即行
        for(i=16; i<32; i++) {
            disp = highlight ? ~(*(pStrTemp + i)) : (*(pStrTemp + i));  /* 反显 */
          
            LCM_Wr1Data(disp);						// 发送数据
        }
    }
    else {                             							//  右半屏
        disp_cx -= 4;
        disp_cx <<= 4;
        disp_cy <<= 1;
        LCM_Wr2Command(LCM_ADDRSTRX+disp_cx); 					// 设置当前列地址，即列
        LCM_Wr2Command(LCM_ADDRSTRY+disp_cy);					// 设置当前页地址，即行
        for(i=0; i<16; i++) {
            disp = highlight ? ~(*(pStrTemp + i)) : (*(pStrTemp + i));  /* 反显 */
          
            LCM_Wr2Data(disp);						// 发送数据
        }

        LCM_Wr2Command(LCM_ADDRSTRX+disp_cx); 					// 设置当前列地址，即列
        LCM_Wr2Command(LCM_ADDRSTRY+disp_cy + 1); 				// 设置当前页地址，即行
        for(i=16; i<32; i++) {
            disp = highlight ? ~(*(pStrTemp + i)) : (*(pStrTemp + i));  /* 反显 */
          
            LCM_Wr2Data(disp);						// 发送数据
        }
    }

}
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
void LCM_DispCHSStr(unsigned char DisCHS_y, unsigned char DisCHS_x,unsigned char Num,const unsigned char *pStr,unsigned char highlight)
{
    unsigned char j, ChaNum = Num;
    const unsigned char *pStrTemp = pStr;
    unsigned char disp_cx = DisCHS_x;
    unsigned char disp_cy = DisCHS_y;

    if (DisCHS_x > 7) DisCHS_x = 7;
    if (DisCHS_y > 3) DisCHS_y = 3;

    for(j = 0; j < ChaNum; j++) {
        if(disp_cx >= 8) {
            disp_cx = 0;
            disp_cy++;
            if (disp_cy >= 4) {
                disp_cy = 0;
            }
        }
        LCM_DispCHS(disp_cy, disp_cx, pStrTemp + (j << 5),highlight);
        disp_cx++;
    }
}

// 天(0) 上(1) 下(2) 雨(3) 不(4) 下(5) 水(6)
extern unsigned char const CHSStr1[]=
{
	0x00,0x40,0x42,0x42,0x42,0x42,0x42,0xFE,0x42,0x42,0x42,0x42,0x42,0x42,0x40,0x00,
	0x00,0x80,0x40,0x20,0x10,0x08,0x06,0x01,0x02,0x04,0x08,0x10,0x30,0x60,0x20,0x00,	/*"天",0*/
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x40,0x40,0x40,0x40,0x40,0x00,0x00,0x00,
	0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x60,0x40,0x00,	/*"上",1*/
	
	0x00,0x02,0x02,0x02,0x02,0x02,0x02,0xFE,0x22,0x62,0xC2,0x82,0x02,0x03,0x02,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,	/*"下",2*/
	
	0x02,0x02,0xF2,0x32,0x52,0x92,0x12,0xFE,0x32,0x52,0x92,0x12,0xF2,0x02,0x02,0x00,
	0x00,0x00,0xFF,0x01,0x02,0x04,0x00,0x7F,0x01,0x02,0x44,0x80,0x7F,0x00,0x00,0x00,	/*"雨",3*/
	
	0x00,0x02,0x02,0x02,0x02,0x82,0x42,0xFE,0x06,0x42,0xC2,0x82,0x02,0x03,0x02,0x00,
	0x00,0x08,0x04,0x02,0x01,0x00,0x00,0x7F,0x00,0x00,0x00,0x01,0x07,0x02,0x00,0x00,	/*"不",4*/
	
	0x00,0x02,0x02,0x02,0x02,0x02,0x02,0xFE,0x22,0x62,0xC2,0x82,0x02,0x03,0x02,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,	/*"下",5*/
	
	0x00,0x10,0x10,0x10,0x90,0x70,0x00,0xFF,0x20,0x60,0x90,0x08,0x04,0x00,0x00,0x00	/*"水",6*//
	0x10,0x10,0x08,0x06,0x01,0x40,0x80,0x7F,0x00,0x00,0x01,0x06,0x0C,0x18,0x08,0x0

};	


/***********************************************************************
* 名称：LCM_DispChar8_16()
* 功能：指定地址显示字符。
* 入口参数：disp_cy		显示行值(0-3)
*	   disp_cx		显示列值(0-15)
*	   dispdata		所要显示的字符(ASCII码)
* 注：支持显示字符0-9、A-Z、a-z及空格，字符显示格式为5*7，模为8*16，所以
*     屏幕显示为4*16(共4行，每行16个字符)。支持高亮显示。
***********************************************************************/
void LCM_DispChar8_16(unsigned char disp_cy,unsigned char disp_cx,char dispdata,unsigned char highlight)
{
	unsigned char i;
        unsigned char disp=0;
        
	disp_cy&=0x07;
	disp_cx&=0x0F;
	if(disp_cx<8)
	{
		disp_cx<<=3;
		disp_cy<<=1;
		LCM_Wr1Command(LCM_ADDRSTRX+disp_cx);
		LCM_Wr1Command(LCM_ADDRSTRY+disp_cy);
		for(i=0;i<8;i++)
		{
                  disp = highlight ? ~ASCII_8_16[dispdata-0x20][i] : ASCII_8_16[dispdata-0x20][i];  /* 反显 */
                  
                  LCM_Wr1Data(disp);
		}
		LCM_Wr1Command(LCM_ADDRSTRX+disp_cx);
		LCM_Wr1Command(LCM_ADDRSTRY+disp_cy+1);
		for(i=8;i<16;i++)
		{
                  disp = highlight ? ~ASCII_8_16[dispdata-0x20][i] : ASCII_8_16[dispdata-0x20][i]; /* 反显 */
                  
                  LCM_Wr1Data(disp);
		}
	}
	else
	{
		disp_cx-=8;
		disp_cx<<=3;
		disp_cy<<=1;
		LCM_Wr2Command(LCM_ADDRSTRX+disp_cx);
		LCM_Wr2Command(LCM_ADDRSTRY+disp_cy);
		for(i=0;i<8;i++)
		{
                  disp = highlight ? ~ASCII_8_16[dispdata-0x20][i] : ASCII_8_16[dispdata-0x20][i]; /* 反显 */
                  
                  LCM_Wr2Data(disp);
		}
		LCM_Wr2Command(LCM_ADDRSTRX+disp_cx);
		LCM_Wr2Command(LCM_ADDRSTRY+disp_cy+1);
		for(i=8;i<16;i++)
		{
                  disp = highlight ? ~ASCII_8_16[dispdata-0x20][i] : ASCII_8_16[dispdata-0x20][i]; /* 反显 */
                  
                  LCM_Wr2Data(disp);
		}
	}
}

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
void  LCM_DispStr8_16(unsigned char disp_cy, unsigned char disp_cx, const char *disp_str,unsigned char highlight)
{
    while( *disp_str != '\0') {
        disp_cy = (disp_cy & 0x07);								// 参数过滤
        disp_cx = (disp_cx & 0x3f);
        LCM_DispChar8_16(disp_cy, disp_cx, *disp_str, highlight); 				// 显示字符

        disp_str++;												// 指向下一字符数据
        disp_cx++;
        if (disp_cx > 15) {										// 指向下一显示行
            disp_cx = 0;
            disp_cy++;
        }
    }
}





