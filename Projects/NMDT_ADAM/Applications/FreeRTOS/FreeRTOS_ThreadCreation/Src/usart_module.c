/* Includes ------------------------------------------------------------------*/
#include "usart_module.h"
#include "cmsis_os.h"

#include "sys_time_module.h"
#include "storage_module.h"
#include "display_module.h"
#include "usart.h"
#include "gpio.h"
#include "pcf8563.h"
#include "string.h"
#include "stdlib.h"
#include "tim.h"

uint8_t Collector_Mode = false;//���ɼ�������ģʽ��־false:����ģʽ    true:ͨ��ģʽ
uint8_t Sensor_Mode =false;   //�����紫��������ģʽ  false:�ڲ���ģʽ  true:������ģʽ�л�������ģʽ
uint8_t Sensor_Configuration =false; //�������������õı�־  false:û�� ���룬true����
uint8_t Collecter_Key =false;//���ɼ�����������Ƶ��  falseû������   true�Ѿ�����
/* Private define ------------------------------------------------------------*/
#define UART_RX_BUF_SIZE  (512) 	
#define usart1PRIORITY     osPriorityNormal
#define usart1STACK_SIZE   (128)
#define usart2PRIORITY     osPriorityNormal
#define usart2STACK_SIZE   (384)

//#define P1   (3.1415926f)
/*���ݽṹ������*/
WindDirection_t wd;
WindSpeed_t     ws;
SensorHZ_t      hz;
//CommuinicateData_t cdata_m;
///*��������*/
//uint8_t test ;
//uint8_t Speed_Flag;//0������ٵ���0.05    1������ٴ��ڵ���0.05


/* RTC Timeͨ����������ʱ��*/
static RTC_TimeTypeDef Usart_Time;
static RTC_DateTypeDef Usart_Date;

static char rx1_buffer[UART_RX_BUF_SIZE]={0};  /* USART1 receiving buffer */
static char rx2_buffer[UART_RX_BUF_SIZE]={0};  /*USART2 receiving buffer */
static uint32_t rx1_count=0;     /* receiving counter */
static uint32_t rx2_count=0;
static uint8_t cr1_begin=false;        /* '\r'  received */ 
static uint8_t cr2_begin=false;        /* '\r'  received */ 
static uint8_t rx1_cplt=false;   /* received a frame of data ending with '\r'and'\n' */
static uint8_t rx2_cplt=false;   /* received a frame of data ending with '\r'and'\n' */


/*��Ҫ���ݱ���*/
unsigned int download_flag = 0;
uint8_t download_number = 0;
int year2   = 0, month2  = 0, day2  = 0, hour2  = 0, minute2  = 0, second2 = 0;   //��Ҫ������ʼ����ʱ
int year1  = 0, month1 = 0, day1 = 0, hour1 = 0, minute1 = 0;//��Ҫ������ֹ����ʱ

/* os relative */
static osThreadId    Usart1ThreadHandle;
static osThreadId    Usart2ThreadHandle;
static osSemaphoreId semaphore_usart1;
static osSemaphoreId semaphore_usart2;
static void Usart1_Thread(void const *argument);
static void Usart2_Thread(void const *argument);

/**
  * @brief  Init Storage Module. 
  * @retval 0:success;-1:failed
  */
int32_t init_usart_module(void)
{
	/*������Ĭ��ֵ*/
	hz.flag_hz      = HZ_2;
	command.sample  = false;
	command.state   = false ;
	command.instant = false;
	Sensor_Configuration =false;
	Collecter_Key =false;
	download_flag = 0;
	download_number = 0;
	
	wd.wd_1s_qc = QC_L;
	ws.ws_1s_qc = QC_L;
	
	/* Define used semaphore ��������1���ź���*/
  osSemaphoreDef(SEM_USART1);
  /* Create the semaphore used by the two threads */
  semaphore_usart1=osSemaphoreCreate(osSemaphore(SEM_USART1), 1);
  if(semaphore_usart1 == NULL)
  {
    printf("Create Semaphore_USART1 failed!\r\n");
    return -1;
  }
	
	/* Define used semaphore ��������2���ź���*/
  osSemaphoreDef(SEM_USART2);
  /* Create the semaphore used by the two threads */
  semaphore_usart2=osSemaphoreCreate(osSemaphore(SEM_USART2), 1);
  if(semaphore_usart2 == NULL)
  {
    printf("Create Semaphore_USART2 failed!\r\n");
    return -1;
  }
	

	/* Create a thread to read historical data��������2����洢�������� */
  osThreadDef(Usart2, Usart2_Thread, usart2PRIORITY, 0, usart2STACK_SIZE);
  Usart2ThreadHandle=osThreadCreate(osThread(Usart2), NULL);
  if(Usart2ThreadHandle == NULL)
  {
    printf("Create Usart2 Thread failed!\r\n");
    return -1;
  }
	
	/* Create a thread to read historical data��������1����洢�������� */
  osThreadDef(Usart1, Usart1_Thread, usart1PRIORITY, 0, usart1STACK_SIZE);
  Usart1ThreadHandle=osThreadCreate(osThread(Usart1), NULL);
  if(Usart1ThreadHandle == NULL)
  {
    printf("Create Usart1 Thread failed!\r\n");
    return -1;
  }
	
  return 0;
	
	
}





/*����1������������*/
static void Usart1_Thread(void const *argument)
{
	uint16_t i=0;
	while(1)
	{
		    if(osSemaphoreWait(semaphore_usart1, osWaitForever)==osOK)
				{
					   //printf("���������ݣ�%s\r\n",rx1_buffer);
					   if((strstr(rx1_buffer,"Q,")!=0)&&(strlen(rx1_buffer) >= 22))
						 {
							    ws.ws_1s_qc  =QC_R;//������ȷ
									wd.wd_1s_qc = QC_R;
							 
							    sscanf(rx1_buffer+3,"%03u",&wd.winddirection);
							    sscanf(rx1_buffer+7,"%f",&ws.windspeed);
							}
						 else if((strstr(rx1_buffer,"Q,")!=0)&&(strlen(rx1_buffer) == 19))
						 {
							    ws.ws_1s_qc = QC_R;//������ȷ
									wd.wd_1s_qc = QC_L;
							 
							    sscanf(rx1_buffer+4,"%f",&ws.windspeed);
									wd.winddirection = 0;
						 }
						else if((strstr(rx1_buffer,"Q,")!=0)&&(strlen(rx1_buffer) == 13))
						 {
							    wd.wd_1s_qc  =QC_L;//����ȱ��
							    ws.ws_1s_qc  =QC_L;//����ȱ��
						 }									
						  rx1_count=0;//������1���ձ�־
							rx1_cplt=false;                                              
							memset(rx1_buffer,0,sizeof(rx1_buffer)); 
				}
		    else 
				{				
			     //printf("δ�ͷ��ź���\r\n");
				}
		
		
		}
}

/*����2������������*/
static void Usart2_Thread(void const *argument)
{
	unsigned long tatal_seconds2 = 0, tatal_seconds1 = 0;
	/* Init LCD12864 */
  LCM_DispInit();
  /* LOGO */
  LCM_DispStr8_16(1,0,"    Welcome!    ",false);
	
	if(init_sys_time_module()<0)
  {
    printf("init sys_time module failed!\r\n");
  }
  else
  {
    //printf("init sys_time module ok!\r\n");
  }
  
	
	if(init_storage_module()<0)
  {
    printf("init storage module failed!\r\n");
  }
  else
  {
    //printf("init storage module ok!\r\n");
  }
	
	if(init_display_module()<0)
  {
    printf("init display module failed!\r\n");
  }
  else
  {
    //printf("init storage module ok!\r\n");
  }
	
	
	while(1)
	{
		uint8_t i=0,j=0;
		char disp_buf1[20];
		if(osSemaphoreWait(semaphore_usart2,osWaitForever)==osOK)
		{
			if(strstr(rx2_buffer,"<TIME>") !=0)//�鿴ʱ��
				{
					memset(&disp_buf1, 0, sizeof(disp_buf1));
					get_sys_time(&Usart_Date,&Usart_Time);
					i = snprintf(disp_buf1,sizeof(disp_buf1),"<TIME %02u%02u%02u%02u%02u%02u>\r\n",Usart_Date.Year,Usart_Date.Month,Usart_Date.Date,Usart_Time.Hours,Usart_Time.Minutes,Usart_Time.Seconds);
					HAL_UART_Transmit(&huart2,(uint8_t *)disp_buf1,i - 1,0xFFFF);
				}
				else if((strstr(rx2_buffer,"<TIME ") != 0)	&&(strlen(rx2_buffer) == 19))	// <TIME 180313143940>
				{
					uint8_t year=0;
					sscanf(rx2_buffer+6,"%02u",&Usart_Date.Year);
					year = Usart_Date.Year;
					//printf("1��;%d\r\n",Usart_Date.Year);
					sscanf(rx2_buffer+8,"%02u",&Usart_Date.Month);
					sscanf(rx2_buffer+10,"%02u",&Usart_Date.Date);
					sscanf(rx2_buffer+12,"%02u",&Usart_Time.Hours);
					sscanf(rx2_buffer+14,"%02u",&Usart_Time.Minutes);
					sscanf(rx2_buffer+16,"%02u",&Usart_Time.Seconds);
					Usart_Date.Year = year;
					//printf("20%02u-%02u-%02u %02u:%02u:%02u\r\n",Usart_Date.Year,Usart_Date.Month,Usart_Date.Date,Usart_Time.Hours,Usart_Time.Minutes,Usart_Time.Seconds);
					/*������ò���*/
					Usart_Time.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
					Usart_Time.StoreOperation=RTC_STOREOPERATION_RESET;
					Usart_Time.SubSeconds=0;
					Usart_Time.TimeFormat=RTC_HOURFORMAT12_AM;
					if(set_sys_time(&Usart_Date,&Usart_Time)<0)
					{
						HAL_UART_Transmit(&huart2,(uint8_t *)"F\r\n",strlen("(F\r\n"),0xFF);
					}
					else
					{
						HAL_UART_Transmit(&huart2,(uint8_t *)"T\r\n",strlen("T\r\n"),0xFF);
					}	
				}
		   else if((strstr(rx2_buffer,"<RSTART>") !=0) && (strlen(rx2_buffer) == 8))
			  {
						/*����������ɼ��� */
						//HAL_UART_Transmit(&huart1,"���ɼ���������\r\n",(10),0xFF);
						HAL_NVIC_SystemReset();
				}	
			 else if((strstr(rx2_buffer,"<DATA ") !=0)	&&(strlen(rx2_buffer) == 28))	//<DATA 1803131458 1903131500>
			  {
							printf("T\r\n");
							/*��ʼʱ������ʱ*/
							sscanf(rx2_buffer+6,"%02u",&year2);
							sscanf(rx2_buffer+8,"%02u",&month2);
							sscanf(rx2_buffer+10,"%02u",&day2);
							sscanf(rx2_buffer+12,"%02u",&hour2);
							sscanf(rx2_buffer+14,"%02u",&minute2);
							/*��ֹʱ������ʱ*/
							sscanf(rx2_buffer+17,"%02u",&year1);
							sscanf(rx2_buffer+19,"%02u",&month1);
							sscanf(rx2_buffer+21,"%02u",&day1);
							sscanf(rx2_buffer+23,"%02u",&hour1);
							sscanf(rx2_buffer+25,"%02u",&minute1);

							
							tatal_seconds2 = l_mktime(year2, month2, day2, hour2, minute2, 0);
							tatal_seconds1 = l_mktime(year1, month1, day1, hour1, minute1, 0);
							
							download_flag = (tatal_seconds1 - tatal_seconds2) / 60;
							download_number = 1;
							if(command.state == true)
							{
										printf("%02u%02u%02u%02u   %02u%02u%02u%02u\r\n",month2,day2,hour2,minute2,month1,day1,hour1,minute1);
										printf("��Ҫ�����ݸ���:%d\r\n", download_flag);
							}
							
			}
			 else if(strstr(rx2_buffer,"<STATE>") !=0)
			  {
				   command.state  = true;
				   printf("T");
				}
			 else if(strstr(rx2_buffer,"<0STATE>") !=0)
			  {
				   command.state  = false;
				   printf("T");
			}
			 else if(strstr(rx2_buffer,"<SAMPLE>") !=0)
			  {
				   command.sample  = true;
				   printf("T");
			}
			 else if(strstr(rx2_buffer,"<0SAMPLE>") !=0)
			  {
				   command.sample  = false;
				   printf("T");
			}
			 else if(strstr(rx2_buffer,"<INSTANT>") !=0)
			  {
				   command.instant  = true;
				   printf("T");
			}
			 else if(strstr(rx2_buffer,"<0INSTANT>") !=0)
			  {
				   command.instant  = false;
				   printf("T");
				}
				else
				{
						printf("F\r\n");
				}
			
			rx2_count=0;
			rx2_cplt=false;                                              /* clear the flag of receive */
			memset(rx2_buffer,0,sizeof(rx2_buffer));                      /* clear the register of receive */
			}
   }
}


/**����1�жϺ���*/
void USART1_IRQHandler(void)
{
  UART_HandleTypeDef *huart=&huart1;
  uint32_t tmp_flag = 0, tmp_it_source = 0;

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Receive_IT(huart);*/
    uint8_t data=0;
  
    data=huart->Instance->DR;  /* the byte just received  */
    
		
//		if(!rx1_cplt)
//    {
//      if(data=='<')
//      {
//        cr1_begin=true;
//        rx1_count=0;
//        rx1_buffer[rx1_count]=data;
//        rx1_count++; 
//      }
//     
//      else if(cr1_begin==true)
//      {
//        rx1_buffer[rx1_count]=data;
//        rx1_count++; 
//        if(rx1_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
//        {
//          /* Set transmission flag: trasfer complete*/
//          rx1_cplt=true;
//        }
//        
//        if(data=='>')
//        {
//          rx1_cplt=true;
//          cr1_begin=false;
//        }
//      }
//      else
//      {
//        rx1_count=0;
//      }
//    }

		
		
    if(!rx1_cplt)
    {
      if(cr1_begin==true)  /* received '\r' */
      {
        cr1_begin=false;
        if(data=='\n')  /* received '\r' and '\n' */
        {
          /* Set transmission flag: trasfer complete*/
          rx1_cplt=true;
        }
        else
        {
          rx1_count=0;
        }
      }
      else
      {
        if(data=='\r')  /* get '\r' */
        {
          cr1_begin=true;
        }
        else  /* continue saving data */
        {
          rx1_buffer[rx1_count]=data;
          rx1_count++;
          if(rx1_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
          {
            /* Set transmission flag: trasfer complete*/
            rx1_cplt=true;
          } 
        }
       }
      }
    
  	 /* received a data frame ���ݽ�����ɾ��ͷŻ�����*/
    if(rx1_cplt==true)
    {
      if(semaphore_usart1!=NULL)
      {
         /* Release mutex */
        osSemaphoreRelease(semaphore_usart1);
				//printf("1���յ����ݲ��ͷ����ź���\r\n");
      }
    }

   
    }
  
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Transmit_IT(huart);*/
  }

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter end --------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_EndTransmit_IT(huart);*/
  }  

  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Clear all the error flag at once */
    __HAL_UART_CLEAR_PEFLAG(huart);
    
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;
    
    HAL_UART_ErrorCallback(huart);
  } 
}

/**����2�жϺ���*/
void USART2_IRQHandler(void)
{
  UART_HandleTypeDef *huart=&huart2;
  uint32_t tmp_flag = 0, tmp_it_source = 0;

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Receive_IT(huart);*/
    uint8_t data=0;
  
    data=huart->Instance->DR;  /* the byte just received  */
    
    
  
     
			if(!rx2_cplt)
    {
      if(data=='<')
      {
        cr2_begin=true;
        rx2_count=0;
        rx2_buffer[rx2_count]=data;
        rx2_count++; 
      }
     
      else if(cr2_begin==true)
      {
        rx2_buffer[rx2_count]=data;
        rx2_count++; 
        if(rx2_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
        {
          /* Set transmission flag: trasfer complete*/
          rx2_cplt=true;
        }
        
        if(data=='>')
        {
          rx2_cplt=true;
          cr2_begin=false;
        }
      }
      else
      {
        rx2_count=0;
      }

    }
		
//		if(!rx2_cplt)
//    {
//      if(cr2_begin==true)  /* received '\r' */
//      {
//        cr2_begin=false;
//        if(data=='\n')  /* received '\r' and '\n' */
//        {
//          /* Set transmission flag: trasfer complete*/
//          rx2_cplt=true;
//        }
//        else
//        {
//          rx2_count=0;
//        }
//      }
//      else
//      {
//        if(data=='\r')  /* get '\r' */
//        {
//          cr2_begin=true;
//        }
//        else  /* continue saving data */
//        {
//          rx2_buffer[rx2_count]=data;
//          rx2_count++;
//          if(rx2_count>UART_RX_BUF_SIZE-1)  /* rx buffer full */
//          {
//            /* Set transmission flag: trasfer complete*/
//            rx2_cplt=true;
//          } 
//        }
//       }
//      }
//		
		 /* received a data frame ���ݽ�����ɾ��ͷ��ź���*/
    if(rx2_cplt==true)
    {
			//printf("����2���յ�����\r\n");
      if(semaphore_usart2!=NULL)
      {
        /* Release the semaphore */
				//printf("�ź���2�����ɹ�\r\n");
        osSemaphoreRelease(semaphore_usart2);
				//printf("�ͷŴ���2���ź�\r\n");
      }
    }
		
		
    }
  
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_Transmit_IT(huart);*/
  }

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter end --------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    /*UART_EndTransmit_IT(huart);*/
  }  

  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Clear all the error flag at once */
    __HAL_UART_CLEAR_PEFLAG(huart);
    
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;
    
    HAL_UART_ErrorCallback(huart);
  } 
}



