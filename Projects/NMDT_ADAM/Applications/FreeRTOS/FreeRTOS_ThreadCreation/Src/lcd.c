#include "lcd.h"
#include "lcdfont.h"
#include "rtc.h"
#include "stdio.h"
#include "main.h"

//static void GET_RTC_Time(void);
//static char disp_buf[128];//��ʾ����
//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
uint8_t OLED_GRAM[132][8];   //����һ������  ��д����  �ٰ�����д��LCD
int8_t number=0;
int8_t number1=0;
int8_t number2=0;
int8_t number3=0;
int8_t number4=0;
int8_t number5=0;

 int8_t a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0,a8=0;
 int8_t b1=0,b2=0,b3=0,b4=0,b5=0,b6=0,b7=0;
 int8_t c1=0,c2=0,c3=0,c4=0,c5=0,c6=0,c7=0;
 int8_t d4=0,e4=0;
 
char disp_buf[128];

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_W_Command (0xb0+i);    //����ҳ��ַ��0~7��
		OLED_W_Command (0x00);      //������ʾλ�á��е͵�ַ
		OLED_W_Command (0x10);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<132;n++)
		OLED_W_Data(OLED_GRAM[n][i]); 
	}   
}

/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		unsigned char OLED_W_Command(void); 
*��������:		д���OLED�� 
*******************************************************************************/ 
void OLED_W_Command(unsigned char com)
{
	unsigned char j;
	OLED_CS_H();
  for(j=0;j<3;j++);
	OLED_DC_L();
	OLED_Read_H();
	OLED_Write_L();
	com=com&0xFF;
	DATAOUT(com);
  OLED_CS_L();
	for(j=0;j<3;j++);
	OLED_CS_H();
	
} 	    

/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_W_Data(unsigned dat); 
*��������:		д���ݵ�oled��ʾ���� 
*******************************************************************************/ 
void OLED_W_Data(unsigned char dat)
{
	unsigned char j;
	OLED_CS_H();
	for(j=0;j<3;j++);
	OLED_DC_H();
	OLED_Read_H();
	OLED_Write_L();
	dat=dat&0xff;
	DATAOUT(dat)
	OLED_CS_L();
	for(j=0;j<3;j++);
	OLED_CS_H();
} 	    


/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_Clear(void) 
*��������:		���� 
*******************************************************************************/ 
void OLED_Clear(void) 
{ 
	uint8_t i,n; 
	for(i=0;i<8;i++) 
     for(n=0;n<132;n++)
        OLED_GRAM[n][i]=0X00;
   	OLED_Refresh_Gram();//������ʾ
} 


/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t) 
*��������:		��x�� ��y�л�һ����
*��ڲ�����x:0~~127��y:0~~63;t:1��䣬0���
*******************************************************************************/ 
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t) 
{ 
	uint8_t pos,bx,temp=0;
	x=x&0x7f;   //��������  ����x>127  y>63
	y=y&0x3f;
	pos=y/8;   //��Ӧ��OLED_GRAM�е�ҳ
	bx=y%8;
	temp=1<<(7-bx);
	if(t)
		OLED_GRAM[x][pos]|=temp;
	else 
		OLED_GRAM[x][pos]&=~temp;	    
} 


/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_Char(uint8_t x,uint8_t y,unsigned char chr)
*��������:		�ڶ���дһ��8*16��ASCII��
*��ڲ���:x��(0<x<127)  yҳ��0<y<63��  highlight=ture���ԣ�highlight=false����
*******************************************************************************/ 
void OLED_Char(unsigned char x,unsigned char y,unsigned char chr,unsigned char highlight)
{
	uint8_t t,temp,t1;
	uint8_t y0=y;
	x=x&0x7f;   //��������  ����x>127  y>63
	y=y&0x3f;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ
	for(t=0;t<16;t++)             
	{
		temp=asc2_1608[chr][t];
		for(t1=0;t1<8;t1++)
		{
			if(highlight==false)//����
			{
			if(temp&0x01)
				OLED_DrawPoint(x,y,1);
			else 
				OLED_DrawPoint(x,y,0);
			temp>>=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}
			else//����
			{
				if(temp&0x01)
				OLED_DrawPoint(x,y,0);
			else 
				OLED_DrawPoint(x,y,1);
			temp>>=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
				
			}

	}
} 	    
}

/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p)
*��������:		��ʾ�ַ���
*��ڲ���:x��(0<x<127)  yҳ��0<y<63��  highlight=ture���ԣ�highlight=false����
*******************************************************************************/ 
void OLED_ShowString(unsigned char x,unsigned char y,const char *p,unsigned char highlight)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>120)
        {
					x=0;
					y+=16;
				}
        if(y>48)
        {
				  y=x=0;
					OLED_Clear();
				}
        OLED_Char(x,y,*p,highlight);	 
        x+=8;
        p++;
    }  
	
}	   


/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_China(uint8_t x,uint8_t y,unsigned char cha)
*��������:		�ڶ���дһ��16*16�ĺ���
*��ڲ���:x��(0<x<127)  yҳ��0<y<63��  highlight=ture���ԣ�highlight=false����
*******************************************************************************/ 
void OLED_China(unsigned char x,unsigned char y,unsigned char n,unsigned char highlight)
{
	uint8_t t,t1,temp;
	uint8_t y0=y;
	x=x&0x7f;   //��������  ����x>127  y>63
	y=y&0x3f;
//	chr=chr-'��';//�õ�ƫ�ƺ��ֵ
	for(t=0;t<32;t++)             
	{
		temp=china_1616[n][t];
		for(t1=0;t1<8;t1++)    //���㺯��8����һ��
		{
			if(highlight==false)//����
			{
			  if(temp&0x01)
				  OLED_DrawPoint(x,y,1);
			  else 
				  OLED_DrawPoint(x,y,0);
			  temp>>=1;
			  y++;
			  if((y-y0)==16)
			  {
				  y=y0;
				  x++;
				  break;
			  }
		  }
			else
			{
				if(temp&0x01)
				  OLED_DrawPoint(x,y,0);
			  else 
			 	  OLED_DrawPoint(x,y,1);
			  temp>>=1;
			  y++;
			  if((y-y0)==16)
			  {
				  y=y0;
				  x++;
				  break;
		  	}
			}
		}
	}
} 	    

/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_China(uint8_t x,uint8_t y,uint8_t i)
*��������:		�ڶ���дһ��24*24�ĺ���
*��ڲ���:x��(0<x<127)  yҳ��0<y<63��
*******************************************************************************/ 
void OLED_China1(uint8_t x,uint8_t y,uint8_t i)
{
	uint8_t t,t1,temp;
	uint8_t y0=y;
	x=x&0x7f;   //��������  ����x>127  y>63
	y=y&0x3f;
//	chr=chr-'��';//�õ�ƫ�ƺ��ֵ
	for(t=0;t<72;t++)             
	{
		temp=china_2424[i][t];
		for(t1=0;t1<8;t1++)    //���㺯��8����һ��
		{
			if(temp&0x01)
				OLED_DrawPoint(x,y,1);
			else 
				OLED_DrawPoint(x,y,0);
			temp>>=1;
			y++;
			if((y-y0)==24)
			{
				y=y0;
				x++;
				break;
			}
		}
	}
} 	    


/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_Huatu(uint8_t x,uint8_t y,uint8_t i)
*��������:		�ڶ��㻭һ��ͼ��
*��ڲ���:x��(0<x<127)  yҳ��0<y<63��
*******************************************************************************/ 
void OLED_Huatu(uint8_t x,uint8_t y,uint8_t i)
{
	uint8_t t,t1,temp;
	uint8_t y0=y;
	x=x&0x7f;   //��������  ����x>127  y>63
	y=y&0x3f;
//	chr=chr-'��';//�õ�ƫ�ƺ��ֵ
	for(t=0;t<64;t++)             
	{
		temp=DianLiang[i][t];
		for(t1=0;t1<8;t1++)    //���㺯��8����һ��
		{
			if(temp&0x01)
				OLED_DrawPoint(x,y,1);
			else 
				OLED_DrawPoint(x,y,0);
			temp>>=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}
	}
} 	    

/**************************ʵ�ֺ���******************************************** 
*����ԭ��:		void OLED_OpenMenu(void)
*��������:		�����˵�
*��ڲ���:��
*******************************************************************************/ 
void OLED_OpenMenu(void)
{
	  OLED_China1(17,8,0);
		OLED_China1(41,8,1);
		OLED_China1(65,8,2);
		OLED_China1(89,8,3);
	
	  OLED_China(1,40,0,false);
		OLED_China(17,40,1,false);
		OLED_China(33,40,2,false);
		OLED_China(49,40,3,false);
		OLED_China(65,40,4,false);
		OLED_China(81,40,5,false);
		OLED_China(97,40,6,false);
		OLED_China(113,40,7,false);
	
		OLED_Refresh_Gram();		//������ʾ��OLED 
} 	    


//��ʼ��SSD1303				    
void OLED_Init(void)
{
	OLED_W_Command(0xae); //display off��ʾ��
        OLED_W_Command(0x40); //display start line 00000��ʾ��ʼ��00000
	      OLED_W_Command(0xb0);
        OLED_W_Command(0x81); //contrast�Աȶ�
        OLED_W_Command(0xff);
        OLED_W_Command(0x82); //brightness����
        OLED_W_Command(0x20);
        OLED_W_Command(0xa0); //no remap���ػ�
        OLED_W_Command(0xa4); //intire display offȫ����ʾ��
        OLED_W_Command(0xa6); //normal display������ʾ
        OLED_W_Command(0xa8); //39 mux
        OLED_W_Command(0x3f);
        OLED_W_Command(0xad); //DCDC off
        OLED_W_Command(0x8b);
        OLED_W_Command(0xc8); //scan from COM[N-1] to COM0��COM[N-1] �� COM0ɨ��
        OLED_W_Command(0xd3); //row 0->com 62
        OLED_W_Command(0x00);
        OLED_W_Command(0xd5);    //
        OLED_W_Command(0x70);    //
        OLED_W_Command(0xd8); //mono mode,normal power mode monoģʽ,������Դģʽ
        OLED_W_Command(0x00);  //?
        OLED_W_Command(0xda); //alternative COM pin configuration��һ��com��������
        OLED_W_Command(0x12);
        OLED_W_Command(0xaf); //display on��ʾ��
	OLED_Clear();
	
}  
