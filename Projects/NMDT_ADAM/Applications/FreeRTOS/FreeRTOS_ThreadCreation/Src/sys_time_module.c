/* Includes ------------------------------------------------------------------*/
#include "sys_time_module.h"
#include "cmsis_os.h"

#include "usart_module.h"
#include "storage_module.h"
#include "display_module.h"
#include "iwdg.h"
#include "usart.h"
#include "pcf8563.h"
#include "tim.h"
//#include	"LCM_DRIVE.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define systimeSTACK_SIZE   384//configMINIMAL_STACK_SIZE  /*����ʱ����������ļ�ϵͳ��������Ҫ��ջ�ϴ�  384����ܶ���������󻹲����Ļ���������Ϊ512������ջ��С���������ô�����ʱ���ᵼ��ϵͳ����*/
#define systimePRIORITY     osPriorityHigh
#define P   3.1415926

static FIL file;          /* File object structure (FIL) */
//static uint32_t min_count;//����֮������ۼ�ֵ
//static uint8_t  Channel_min;//ͨ��ģʽʱ���ۼ�ֵ����
//static uint8_t  Sensor_Bad_Sec=0;//�����紫�����𻵻���û�в��ϳ����紫���������
//static uint8_t open_file = 0;


/* RTC Time*/
static RTC_TimeTypeDef sys_time;
static RTC_DateTypeDef sys_date;

struct MIN_DATA min_data;
////д�����ݻ���
//static char data[40];

/* os relative */
static osThreadId SysTimeThreadHandle;
//static osThreadId Tim6ThreadHandle;
static osSemaphoreId semaphore;
//static osSemaphoreId semaphore_tim6;
static osMutexId mutex;
/* Private function prototypes -----------------------------------------------*/
static void SysTime_Thread(void const *argument);


/**
  * @brief  Init System Time Module. 
  * @retval 0:success;-1:failed
  */
int32_t init_sys_time_module(void)
{
  /* Init RTC Internal */
  
	IWDG_Init();
	
  /* Init extern RTC PCF8563 */
  if(PCF8563_Init() != HAL_OK)
  {
    printf("init pcf8563 failed!\r\n");
  }
  else
  {
    /* synchronize internal RTC with pcf8563 */
    sync_time();
		//printf("��ʱ�ɹ�\r\n");
  }
 
  /* Define used semaphore */
  osSemaphoreDef(SEM);
  /* Create the semaphore used by the two threads */
  semaphore=osSemaphoreCreate(osSemaphore(SEM), 1);
  if(semaphore == NULL)
  {
    printf("Create Semaphore failed!\r\n");
    return -1;
  }
  
  /* Create the mutex */
  osMutexDef(Mutex);
  mutex=osMutexCreate(osMutex(Mutex));
  if(mutex == NULL)
  {
    printf("Create Mutex failed!\r\n");
    return -1;
  }
  
	
  /* Create a thread to update system date and time */
  osThreadDef(SysTime, SysTime_Thread, systimePRIORITY, 0, systimeSTACK_SIZE);
  SysTimeThreadHandle=osThreadCreate(osThread(SysTime), NULL);
  if(SysTimeThreadHandle == NULL)
  {
    printf("Create System Time Thread failed!\r\n");
    return -1;
  }
  return 0;
}

/**
  * @brief  get System Date and Time. 
  * @retval 0:success;-1:failed
  */
int32_t get_sys_time(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime)
{
  /* Wait until a Mutex becomes available */
  if(osMutexWait(mutex,500)==osOK)
  {
    if(sDate)
    {
      *sDate=sys_date;
    }
    if(sTime)
    {
      *sTime=sys_time;
    }
    
    /* Release mutex */
    osMutexRelease(mutex);
    
    return 0;
  }
  else
  {
    /* Time */
    if(sTime)
    {
      sTime->Seconds=0;
      sTime->Minutes=0;
      sTime->Hours=0;
    }
    /* Date */
    if(sDate)
    {
      sDate->Date=1;
      sDate->WeekDay=RTC_WEEKDAY_SUNDAY;
      sDate->Month=(uint8_t)RTC_Bcd2ToByte(RTC_MONTH_JANUARY);
      sDate->Year=0;
    }
    
    return -1;
  }
}

int32_t get_sys_time_tm(struct tm *DateTime)
{
  /* Wait until a Mutex becomes available */
  if(osMutexWait(mutex,500)==osOK)
  {
    if(DateTime)
    {
      DateTime->tm_year=sys_date.Year+2000;
      DateTime->tm_mon=sys_date.Month;
      DateTime->tm_mday=sys_date.Date;
      DateTime->tm_hour=sys_time.Hours;
      DateTime->tm_min=sys_time.Minutes;
      DateTime->tm_sec=sys_time.Seconds;
    }
    
    /* Release mutex */
    osMutexRelease(mutex);
    
    return 0;
  }
  else
  {
    if(DateTime)
    {
      DateTime->tm_year=2000;
      DateTime->tm_mon=0;
      DateTime->tm_mday=0;
      DateTime->tm_hour=0;
      DateTime->tm_min=0;
      DateTime->tm_sec=0;
    }
    
    return -1;
  }
}

int32_t set_sys_time(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime)
{
  int32_t res=0;
  
  /* Wait until a Mutex becomes available */
  if(osMutexWait(mutex,500)==osOK)
  {
    if(sDate)
    {
      sys_date=*sDate;
    }
    if(sTime)
    {
      sys_time=*sTime;
    }
    
    /* check param */
    if(IS_RTC_YEAR(sys_date.Year) && IS_RTC_MONTH(sys_date.Month) && IS_RTC_DATE(sys_date.Date) &&
       IS_RTC_HOUR24(sys_time.Hours) && IS_RTC_MINUTES(sys_time.Minutes) && IS_RTC_SECONDS(sys_time.Seconds))
    {
    
      if((HAL_RTC_SetDate(&hrtc,&sys_date,FORMAT_BIN)==HAL_OK)&&  /* internal RTC */
         (HAL_RTC_SetTime(&hrtc,&sys_time,FORMAT_BIN)==HAL_OK)&&
         (PCF8563_SetDateTime(&sys_time,&sys_date)==HAL_OK))      /* PCF8563 */
      {
        res=0;
      }
      else
      {
        res=-1;
      }
    }
    else
    {
      res=-1;
    }
    
    /* Release mutex */
    osMutexRelease(mutex);
    
    return res;
  }
  else
  {
    return -1;
  }
}

/**
  * @brief  System sys_time update
  * @param  thread not used
  * @retval None
  */
static void SysTime_Thread(void const *argument)
{
		uint8_t i=0;
		uint8_t len=0;
		FRESULT  res=FR_OK;
		uint32_t byteswritten1 = 0;
		char pathfile[24]={0};
		uint32_t offset=0;
		/* Init IWDG  */
		IWDG_Init();
  
  while(1)
  {
    /* Try to obtain the semaphore */
    if(osSemaphoreWait(semaphore,osWaitForever)==osOK)
    {
      /* Wait until a Mutex becomes available */
      if(osMutexWait(mutex,500)==osOK)
      {
        HAL_RTC_GetTime(&hrtc,&sys_time,FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc,&sys_date,FORMAT_BIN);
				
				if(download_number)
				{
						if(start_download() == 0)
							{
									if(command.state == true)
									{
											printf("��ʼ��Ҫ����\r\n");
									}
									download_number = 0;
							}
				}
				
				//printf("20%02d%02d%02d%02d%02d%02d,%06.2f,%03d\r\n", sys_date.Year, sys_date.Month, sys_date.Date, sys_time.Hours, sys_time.Minutes, sys_time.Seconds,ws.windspeed, wd.winddirection);
				if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
				{
						printf("<20%02d%02d%02d%02d%02d%02d,%06.2f,%03d>\r\n", sys_date.Year, sys_date.Month, sys_date.Date, sys_time.Hours, sys_time.Minutes, sys_time.Seconds,ws.windspeed, wd.winddirection);
				}
				else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
				{
						printf("<20%02d%02d%02d%02d%02d%02d,%06.2f,///>\r\n", sys_date.Year, sys_date.Month, sys_date.Date, sys_time.Hours, sys_time.Minutes, sys_time.Seconds,ws.windspeed);
				}
				else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
				{
						printf("<20%02d%02d%02d%02d%02d%02d,///.//,///>\r\n", sys_date.Year, sys_date.Month, sys_date.Date, sys_time.Hours, sys_time.Minutes, sys_time.Seconds );
				}
	
				if(sys_time.Seconds == 0)
				{
//						//ʱ��
//						if(sys_time.Minutes == 0)
//						{
//								sys_time.Minutes = 59;
//								sys_time.Hours 	-= 1;
//						}
//						else
//						{
//								sys_time.Minutes -= 1;
//						}
						snprintf(min_data.min_time, sizeof(min_data.min_time) + 1, "<20%02u%02u%02u%02u%02u,", sys_date.Year, sys_date.Month, sys_date.Date, sys_time.Hours, sys_time.Minutes);
//						//ʱ���ڼ�1����
//						if(sys_time.Minutes == 59)
//						{
//								sys_time.Minutes = 0;
//								sys_time.Hours += 1;
//						}
//						else
//						{
//								sys_time.Minutes += 1;
//						}
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_00, sizeof(min_data.data_00) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_00, sizeof(min_data.data_00) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_00, sizeof(min_data.data_00) + 1, "///.//,///,");
						}
						
						//printf("0���ֵ��%s\r\n", min_data.data_00);
				}
				else if(sys_time.Seconds == 01)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_01, sizeof(min_data.data_01) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_01, sizeof(min_data.data_01) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_01, sizeof(min_data.data_01) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 02)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_02, sizeof(min_data.data_02) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_02, sizeof(min_data.data_02) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_02, sizeof(min_data.data_02) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 03)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_03, sizeof(min_data.data_03) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_03, sizeof(min_data.data_03) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_03, sizeof(min_data.data_03) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 04)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_04, sizeof(min_data.data_04) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_04, sizeof(min_data.data_04) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_04, sizeof(min_data.data_04) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 05)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_05, sizeof(min_data.data_05) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_05, sizeof(min_data.data_05) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_05, sizeof(min_data.data_05) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 06)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_06, sizeof(min_data.data_06) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_06, sizeof(min_data.data_06) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_06, sizeof(min_data.data_06) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 07)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_07, sizeof(min_data.data_07) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_07, sizeof(min_data.data_07) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_07, sizeof(min_data.data_07) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 8)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_08, sizeof(min_data.data_08) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_08, sizeof(min_data.data_08) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_08, sizeof(min_data.data_08) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 9)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_09, sizeof(min_data.data_09) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_09, sizeof(min_data.data_09) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_09, sizeof(min_data.data_09) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 10)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_10, sizeof(min_data.data_10) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_10, sizeof(min_data.data_10) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_10, sizeof(min_data.data_10) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 11)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_11, sizeof(min_data.data_11) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_11, sizeof(min_data.data_11) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_11, sizeof(min_data.data_11) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 12)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_12, sizeof(min_data.data_12) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_12, sizeof(min_data.data_12) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_12, sizeof(min_data.data_12) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 13)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_13, sizeof(min_data.data_13) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_13, sizeof(min_data.data_13) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_13, sizeof(min_data.data_13) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 14)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_14, sizeof(min_data.data_14) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_14, sizeof(min_data.data_14) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_14, sizeof(min_data.data_14) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 15)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_15, sizeof(min_data.data_15) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_15, sizeof(min_data.data_15) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_15, sizeof(min_data.data_15) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 16)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_16, sizeof(min_data.data_16) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_16, sizeof(min_data.data_16) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_16, sizeof(min_data.data_16) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 17)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_17, sizeof(min_data.data_17) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_17, sizeof(min_data.data_17) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_17, sizeof(min_data.data_17) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 18)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_18, sizeof(min_data.data_18) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_18, sizeof(min_data.data_18) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_18, sizeof(min_data.data_18) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 19)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_19, sizeof(min_data.data_19) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_19, sizeof(min_data.data_19) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_19, sizeof(min_data.data_19) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 20)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_20, sizeof(min_data.data_20) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_20, sizeof(min_data.data_20) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_20, sizeof(min_data.data_20) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 21)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_21, sizeof(min_data.data_21) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_21, sizeof(min_data.data_21) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_21, sizeof(min_data.data_21) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 22)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_22, sizeof(min_data.data_22) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_22, sizeof(min_data.data_22) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_22, sizeof(min_data.data_22) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 23)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_23, sizeof(min_data.data_23) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_23, sizeof(min_data.data_23) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_23, sizeof(min_data.data_23) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 24)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_24, sizeof(min_data.data_24) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_24, sizeof(min_data.data_24) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_24, sizeof(min_data.data_24) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 25)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_25, sizeof(min_data.data_25) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_25, sizeof(min_data.data_25) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_25, sizeof(min_data.data_25) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 26)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_26, sizeof(min_data.data_26) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_26, sizeof(min_data.data_26) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_26, sizeof(min_data.data_26) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 27)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_27, sizeof(min_data.data_27) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_27, sizeof(min_data.data_27) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_27, sizeof(min_data.data_27) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 28)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_28, sizeof(min_data.data_28) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_28, sizeof(min_data.data_28) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_28, sizeof(min_data.data_28) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 29)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_29, sizeof(min_data.data_29) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_29, sizeof(min_data.data_29) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_29, sizeof(min_data.data_29) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 30)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_30, sizeof(min_data.data_30) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_30, sizeof(min_data.data_30) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_30, sizeof(min_data.data_30) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 31)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_31, sizeof(min_data.data_31) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_31, sizeof(min_data.data_31) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_31, sizeof(min_data.data_31) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 32)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_32, sizeof(min_data.data_32) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_32, sizeof(min_data.data_32) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_32, sizeof(min_data.data_32) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 33)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_33, sizeof(min_data.data_33) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_33, sizeof(min_data.data_33) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_33, sizeof(min_data.data_33) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 34)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_34, sizeof(min_data.data_34) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_34, sizeof(min_data.data_34) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_34, sizeof(min_data.data_34) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 35)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_35, sizeof(min_data.data_35) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_35, sizeof(min_data.data_35) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_35, sizeof(min_data.data_35) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 36)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_36, sizeof(min_data.data_36) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_36, sizeof(min_data.data_36) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_36, sizeof(min_data.data_36) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 37)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_37, sizeof(min_data.data_37) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_37, sizeof(min_data.data_37) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_37, sizeof(min_data.data_37) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 38)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_38, sizeof(min_data.data_38) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_38, sizeof(min_data.data_38) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_38, sizeof(min_data.data_38) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 39)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_39, sizeof(min_data.data_39) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_39, sizeof(min_data.data_39) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_39, sizeof(min_data.data_39) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 40)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_40, sizeof(min_data.data_40) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_40, sizeof(min_data.data_40) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_40, sizeof(min_data.data_40) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 41)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_41, sizeof(min_data.data_41) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_41, sizeof(min_data.data_41) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_41, sizeof(min_data.data_41) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 42)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_42, sizeof(min_data.data_42) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_42, sizeof(min_data.data_42) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_42, sizeof(min_data.data_42) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 43)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_43, sizeof(min_data.data_43) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_43, sizeof(min_data.data_43) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_43, sizeof(min_data.data_43) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 44)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_44, sizeof(min_data.data_44) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_44, sizeof(min_data.data_44) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_44, sizeof(min_data.data_44) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 45)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_45, sizeof(min_data.data_45) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_45, sizeof(min_data.data_45) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_45, sizeof(min_data.data_45) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 46)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_46, sizeof(min_data.data_46) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_46, sizeof(min_data.data_46) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_46, sizeof(min_data.data_46) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 47)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_47, sizeof(min_data.data_47) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_47, sizeof(min_data.data_47) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_47, sizeof(min_data.data_47) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 48)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_48, sizeof(min_data.data_48) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_48, sizeof(min_data.data_48) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_48, sizeof(min_data.data_48) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 49)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_49, sizeof(min_data.data_49) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_49, sizeof(min_data.data_49) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_49, sizeof(min_data.data_49) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 50)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_50, sizeof(min_data.data_50) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_50, sizeof(min_data.data_50) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_50, sizeof(min_data.data_50) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 51)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_51, sizeof(min_data.data_51) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_51, sizeof(min_data.data_51) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_51, sizeof(min_data.data_51) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 52)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_52, sizeof(min_data.data_52) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_52, sizeof(min_data.data_52) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_52, sizeof(min_data.data_52) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 53)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_53, sizeof(min_data.data_53) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_53, sizeof(min_data.data_53) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_53, sizeof(min_data.data_53) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 54)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_54, sizeof(min_data.data_54) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_54, sizeof(min_data.data_54) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_54, sizeof(min_data.data_54) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 55)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_55, sizeof(min_data.data_55) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_55, sizeof(min_data.data_55) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_55, sizeof(min_data.data_55) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 56)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_56, sizeof(min_data.data_56) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_56, sizeof(min_data.data_56) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_56, sizeof(min_data.data_56) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 57)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_57, sizeof(min_data.data_57) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_57, sizeof(min_data.data_57) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_57, sizeof(min_data.data_57) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 58)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_58, sizeof(min_data.data_58) + 1, "%06.2f,%03u,", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_58, sizeof(min_data.data_58) + 1, "%06.2f,///,", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_58, sizeof(min_data.data_58) + 1, "///.//,///,");
						}
				}
				else if(sys_time.Seconds == 59)
				{
						if((wd.wd_1s_qc == QC_R) && (ws.ws_1s_qc == QC_R))
						{
								snprintf(min_data.data_59, sizeof(min_data.data_59) - 1, "%06.2f,%03u>\r\n", ws.windspeed, wd.winddirection);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_R) && (wd.winddirection == 0))
						{
								snprintf(min_data.data_59, sizeof(min_data.data_59) - 1, "%06.2f,///>\r\n", ws.windspeed);
						}
						else if((wd.wd_1s_qc == QC_L) && (ws.ws_1s_qc == QC_L))
						{
								snprintf(min_data.data_59, sizeof(min_data.data_59) - 1, "///.//,///>\r\n");
						}
				}
		

				//����ֵ
				ws.windspeed = 0;
				wd.winddirection = 0;
				wd.wd_1s_qc = QC_L;
				ws.ws_1s_qc = QC_L;
				
				//<2018-03-12 09:05:00,012.33,123,>
				/*ÿ���ӵ�01��ʱ��ʼ��SD����洢һ��������*/
				if(sys_time.Seconds == 59)
				{
						 	/*�洢���ݵ�SD��*/
							offset=680 * sys_time.Minutes;
							snprintf((char *)pathfile,sizeof(pathfile),"/DATA/%02u/%02u_%02u.txt",sys_date.Month,sys_date.Date,sys_time.Hours);
							res = f_open(&file,(const char *)pathfile, FA_OPEN_ALWAYS|FA_WRITE);
							if(res==FR_OK)
								{
									   if(command.state == true)
										 {
											    printf("���ļ��ɹ�\r\n");
										 }
									   res=f_lseek(&file,offset);
									   if(res!=FR_OK)
										  {
											    //printf("�ļ�ѡַ����:%d\r\n",res);
											    if(BSP_SD_Init()!=MSD_OK)
												   {
														    if(command.state == true)
																	{
																		printf("sd init failed!\r\n");
																	}													      
												   }
											    else
												   {
														    if(command.state == true)
																	{
																		printf("sd1 init ok!\r\n");
																	}																		      
												   }
										  }
									  else
										 {
											      if(command.state == true)
																	{
																		printf("�ļ�ѡַ�ɹ�:%d\r\n",res);
																	}														    	  
										 }
										 if(command.state == true)
										 {
													printf("д������ݣ�{%s}\r\n", (char *)&min_data);
										 }
										
									  res=f_write(&file,(uint8_t *)&min_data,676,&byteswritten1);
										if(res!=FR_OK)
										{
											    if(command.state == true)
														{
															printf("д����ʧ�ܣ�%d\r\n",res);
														}																
													if(BSP_SD_Init()!=MSD_OK)
														{
															if(command.state == true)
																{
																	printf("sd1 init failed!\r\n");
																}																	
														}
													else
														{
															if(command.state == true)
																{
																	printf("sd2 init ok!\r\n");
																}																		
														}
										}	
									 else
										{
											    if(command.state == true)
														{
															printf("д���ݳɹ�:%d\r\n",res);
														}				
													
										}	
									 res=f_close(&file);	
									 if(res==FR_OK)
										{
											    if(command.state == true)
														{
															printf("�ر��ļ��ɹ�:%d\r\n",res);
														}																
										}							
							}
							else 
							{
									printf("���ļ�ʧ�ܣ�%d\r\n", res);
						  		if(BSP_SD_Init()!=MSD_OK)
										{
											    if(command.state == true)
														{
															printf("sd2 init failed!\r\n");
														}					
							    				
										}
									else
										{
											if(command.state == true)
												{
													printf("sd3 init ok!\r\n");
												}				
										}
							}
							memset(&min_data,0, sizeof(min_data));
				 }
				
				
					
//				/*�жϳ����紫�����𻵣���һֱ���ղ���������������*/
//				if(hz.Number_Data ==0)
//				{
//					   Sensor_Bad_Sec++;//�����紫����δ������𻵵ĵڶ����ӿ�ʼÿ�����
//				}
//				/*������մ�������Ч���ݸ���Ϊ0����˲ʱ��ȱ��*/
//				if(hz.Number_Data == 0)
//				{
//					   wd.wd_1s_qc  =QC_L;//����ȱ��
//						 ws.ws_1s_qc  =QC_L;//����ȱ��
//				}
//				
//				/*����*/
//				if(command.sample == true)
//				{
//					   printf("�������ֵ��%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d\r\n",wd.wd_buf_3s[0],wd.wd_buf_3s[1],wd.wd_buf_3s[2],wd.wd_buf_3s[3],
//					   wd.wd_buf_3s[4],wd.wd_buf_3s[5],wd.wd_buf_3s[6],wd.wd_buf_3s[7],wd.wd_buf_3s[8],wd.wd_buf_3s[9],wd.wd_buf_3s[10],wd.wd_buf_3s[11]);
//					
//					   printf("���ٲ���ֵ��%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f\r\n",ws.ws_buf_3s[0],ws.ws_buf_3s[1],ws.ws_buf_3s[2],ws.ws_buf_3s[3],
//					   ws.ws_buf_3s[4],ws.ws_buf_3s[5],ws.ws_buf_3s[6],ws.ws_buf_3s[7],ws.ws_buf_3s[8],ws.ws_buf_3s[9],ws.ws_buf_3s[10],ws.ws_buf_3s[11]);
//				}
//				/*˲ʱ���ٷ���*/
//				if(ws.ws_1s_qc == QC_R)
//				{
//					   ws.ws_1s  = avergae(ws.ws_buf_3s ,12);
//				}
//				wd.wd_1s  = avergae_qc_rad(wd.wd_buf_3s ,12,wd.wd_1s_12qc ,&wd.wd_1s_qc );
//				
//				/*������� Сʱ������3��˲ʱ����*/
//				if(ws.ws_1s > ws.ws_ext_1h )
//					{
//						   //ws.ws_ext_h   = ws.windspeed ;
//						   ws.ws_ext_1h  = ws.ws_1s;
//						   wd.wd_ext_1h  = wd.winddirection ;
//						   get_sys_time(&sys_date,&sys_time);/*�õ���Ӧ��ʱ��*/
//						   ws.ws_ext_min    = sys_time.Minutes ;
//						   ws.ws_ext_sec    = sys_time.Seconds ;
//						   //printf("�������%d  %d\r\n",wd.winddirection,wd.wd_ext_1h);
//		  		}
//				
//				/*����*/
//				if(command.instant == true)
//				{
//					printf("˲ʱ����%d  ˲ʱ���٣�%d  ˲ʱ�����ʿ�:%d  ˲ʱ�����ʿأ�%d\r\n",wd.wd_1s,ws.ws_1s,wd.wd_1s_qc,ws.ws_1s_qc);
//				}
//				
//				/*1���Ӳ���ֵ*/
//				for(i=59;i>0;i--)
//				{
//					   ws.ws_buf_1m[i]     = ws.ws_buf_1m [i-1];
//					   ws.ws_1m_60qc[i]    = ws.ws_1m_60qc [i-1] ;
//					   wd.wd_buf_1m[i]     = wd.wd_buf_1m [i-1];
//					   wd.wd_1m_60qc[i]    = wd.wd_1m_60qc [i-1];
//				}
//				ws.ws_buf_1m[0]     = ws.ws_1s ;
//				ws.ws_1m_60qc[0]    = ws.ws_1s_qc ;
//			  wd.wd_buf_1m[0]     = wd.wd_1s  ;
//				wd.wd_1m_60qc[0]    = wd.wd_1s_qc ;
//				
//				/*������ʱ�̼�00��*/
//				if(sys_time.Seconds == 0)
//				{
//					   /*�洢���͵�˲ʱ��*/
//					   ws.res_ws_1s = ws.ws_1s ;
//					   wd.res_wd_1s = wd.wd_1s ;
//					
//					   /*1����ƽ����*/
//					   ws.ws_1m = avergae_qc(ws.ws_buf_1m ,60,ws.ws_1m_60qc ,&ws.ws_1m_qc );
//					   wd.wd_1m = avergae_qc_rad(wd.wd_buf_1m ,60,wd.wd_1m_60qc ,&wd.wd_1m_qc );
//					
//					   /*���60���ӵ�1����ƽ�����ݲ���*/
//					   for(i=59;i>0;i--)
//					   {
//							    ws.ws_buf_60m[i]  = ws.ws_buf_60m[i-1];
//							    ws.ws_60m_60qc[i] = ws.ws_60m_60qc[i-1] ;
//							    wd.wd_buf_60m[i]  = wd.wd_buf_60m[i-1];
//							    wd.wd_60m_60qc[i] = wd.wd_60m_60qc[i-1] ;
//						 }
//						 ws.ws_buf_60m[0]  = ws.ws_1m;
//						 ws.ws_60m_60qc[0] = ws.ws_1m_qc;
//						 wd.wd_buf_60m[0] = wd.wd_1m;
//						 wd.wd_60m_60qc[0] = wd.wd_1m_qc;
//					
//					   /*2����ƽ����*/
//						 if(min_count >=2)
//						 {
//							    ws.ws_2m = avergae_qc(ws.ws_buf_60m ,2,ws.ws_60m_60qc ,&ws.ws_2m_qc );
//					        wd.wd_2m = avergae_qc_rad(wd.wd_buf_60m ,2,wd.wd_60m_60qc ,&wd.wd_2m_qc );
//						 }
//						 else
//						 {
//							    ws.ws_2m_qc = QC_L;
//							    wd.wd_2m_qc = QC_L;
//						 }
//					   
//						 
//						 /*10����ƽ�������*/
//						if(min_count >=8)  //����ֵ����10���ӣ���10*0.75=7.5��Ϊ��Ч����ֵ  ����ȱ��
//						 {
//							    ws.ws_10m = avergae_qc(ws.ws_buf_60m ,10,ws.ws_60m_60qc ,&ws.ws_10m_qc );
//					        wd.wd_10m = avergae_qc_rad(wd.wd_buf_60m ,10,wd.wd_60m_60qc ,&wd.wd_10m_qc );
//						 }
//						 else
//						 {
//							    ws.ws_10m_qc = QC_L;
//							    wd.wd_10m_qc = QC_L;
//						 }
//				
//						 /*Сʱ�����ٲ���ֵ*/
//						 if(ws.ws_10m >ws.ws_max_1h )
//							 {
//     								 ws.ws_max_1h  = ws.ws_10m ;
//			    					 wd.wd_max_1h  = wd.wd_10m ;
//					    			 ws.ws_max_min = sys_time.Minutes ;
//							 }
//						
//						 /*��䷢�����ݻ��棬�������ݵ���λ��*/
//						 len = FillData_Minute(0);
//						 HAL_UART_Transmit(&huart2,(uint8_t *)&cdata_m,len,0xFF);
//							 
//						if(command.state == true)
//						{
//							   printf("���ݳ��ȣ�%d\r\n",len);
//						     printf("�ʿأ�%d-%d-%d-%d-%d-%d-%d-%d\r\n",wd.wd_10m_qc ,ws.ws_10m_qc ,wd.wd_2m_qc ,ws.ws_2m_qc ,wd.wd_1m_qc ,ws.ws_1m_qc ,wd.wd_1s_qc ,ws.ws_1s_qc );
//						}
//				}
//				
//				/*00��02��ʱ���㼫��������*/
//				if((sys_time.Minutes == 0)&&(sys_time.Seconds == 02))
//				{
//					   /*����ֵ����*/
//					   ws.ws_ext_1h  = 0;
//					   wd.wd_ext_1h  = 0;
//					   ws.ws_ext_min = 01;
//					   ws.ws_ext_sec = 01;
//					   /*���ֵ����*/
//					   ws.ws_max_1h  = 0;
//					   wd.wd_max_1h  = 0;
//					   ws.ws_max_min = 01;
//				}
//				
//				/*02��ʱ��ʼ������������*/
//				if(sys_time.Seconds == 03)
//				{
//					   min_count++;
//					   if(command.state == true)
//						 {
//							    printf("����ʱ�䣺%d����\r\n",min_count);
//							    printf("�豸����ʱ��ʱ�䣺%dСʱ\r\n",min_count/60);
//						 }
//						 if(Collector_Mode == true)
//						 {
//							    Channel_min++;
//							    if(Channel_min > 5)
//									{
//										   Channel_min    = 0;
//										   Collector_Mode = false;
//									}
//						 }
//						 else if(Collector_Mode == false)
//						 {
//							    Channel_min = 0;
//						 }
//						 
//				}
				
				/*ÿ���ӵ�01��ʱ��ʼ��SD����洢һ��������*/
//				if(sys_time.Seconds == 01)
//				{
//					   FRESULT  res=FR_OK;
//    				 uint32_t byteswritten=0;
//					   char pathfile[24]={0};
//						 uint32_t offset=0;
//						 
//						 /*�����ʷ����*/
//						 len = FillData_Minute(1);
//						 	/*�洢���ݵ�SD��*/
//							offset=64*sys_time.Minutes;
//							snprintf((char *)pathfile,sizeof(pathfile),"/DATA/%02u/%02u_%02u.txt",sys_date.Month,sys_date.Date,sys_time.Hours);
//							res = f_open(&file,(const char *)pathfile, FA_OPEN_ALWAYS|FA_WRITE);
//							if(res==FR_OK)
//								{
//									   if(command.state == true)
//										 {
//											    printf("���ļ��ɹ�\r\n");
//										 }
//									   res=f_lseek(&file,offset);
//									   if(res!=FR_OK)
//										  {
//											    //printf("�ļ�ѡַ����:%d\r\n",res);
//											    if(BSP_SD_Init()!=MSD_OK)
//												   {
//														    if(command.state == true)
//																	{
//																		printf("sd init failed!\r\n");
//																	}													      
//												   }
//											    else
//												   {
//														    if(command.state == true)
//																	{
//																		printf("sd init ok!\r\n");
//																	}																		      
//												   }
//										  }
//									  else
//										 {
//											      if(command.state == true)
//																	{
//																		printf("�ļ�ѡַ�ɹ�:%d\r\n",res);
//																	}														    	  
//										 }							 
//									  res=f_write(&file,(uint8_t *)&cdata_m,64,&byteswritten);
//										if(res!=FR_OK)
//										{
//											    if(command.state == true)
//														{
//															printf("д����ʧ�ܣ�%d\r\n",res);
//														}																
//													if(BSP_SD_Init()!=MSD_OK)
//														{
//															if(command.state == true)
//																{
//																	printf("sd init failed!\r\n");
//																}																	
//														}
//													else
//														{
//															if(command.state == true)
//																{
//																	printf("sd init ok!\r\n");
//																}																		
//														}
//										}	
//									 else
//										{
//											    if(command.state == true)
//														{
//															printf("д���ݳɹ�:%d\r\n",res);
//														}				
//													
//										}	
//									 res=f_close(&file);	
//									 if(res==FR_OK)
//										{
//											    if(command.state == true)
//														{
//															printf("�ر��ļ��ɹ�:%d\r\n",res);
//														}																
//										}							
//							}
//						else 
//						 {
//						  		if(BSP_SD_Init()!=MSD_OK)
//										{
//											    if(command.state == true)
//														{
//															printf("sd init failed!\r\n");
//														}					
//							    				
//										}
//									else
//										{
//											if(command.state == true)
//												{
//													printf("sd init ok!\r\n");
//												}				
//										}
//					 }
//			}
				
//				/*�жϴ�����������ݵ�Ƶ��*/
//				if(sys_time.Seconds == 59)
//				{
//					  if((min_count >0)&&(Collecter_Key ==false))
//						{
//							if(hz.Number_Data > 200)
//								hz.flag_hz = HZ_8;
//							else if(hz.Number_Data > 200)
//								hz.flag_hz = HZ_4;
//							else if(hz.Number_Data > 100)
//								hz.flag_hz = HZ_2;
//							else if(hz.Number_Data > 40)
//								hz.flag_hz = HZ_1;
//							else if(hz.Number_Data > 10)
//								hz.flag_hz = HZ_0;
//							else
//								hz.flag_hz = HZ_0;
//							hz.Number_Data =0;
//						}
//						else if(Collecter_Key ==true)
//						{
//							hz.Number_Data =0;
//							Collecter_Key = false;
//						}
//						if(command.state == true)
//						{
//							   printf("HZ:%d\r\n",hz.flag_hz );
//						}
//						
//				}
       
				/* synchronize internal RTC with pcf8563��ʱ */
        if((sys_time.Minutes==25) && (sys_time.Seconds==15))
        {
					//printf("��ʱ�ɹ�\r\n");
          sync_time();
        }	
        /* Release mutex */
        osMutexRelease(mutex);
			} 
      else
      {
        //printf("û�еȵ������ź���\r\n");
      }
    
    
    
    if(hiwdg.Instance)
    {
      HAL_IWDG_Refresh(&hiwdg);  /* refresh the IWDG */
			//printf("ι����\r\n");
    }
    
  }
}
}



/**
  * @brief  System sys_time update
  * @param  thread not used
  * @retval None
  */

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  
  /* Release the semaphore every 1 second */
   if(semaphore!=NULL)
  {
		//printf("����A�жϲ�����\r\n");
    osSemaphoreRelease(semaphore);
  }
}


/****************************************************************
 *��������������ĸ���������ƽ��ֵ����
 *data  Ҫ�������ݵ�����
 *len   ��Ҫ��������ݵĸ���
 ****************************************************************/
uint16_t avergae(float *data,uint8_t len)
{
	   if(len == 0)
		 {
			    return data[0];
		 }
		 float sum=0;//����֮��
		 float aver=0;//����ƽ��ֵ
		 uint16_t val;//ƽ��ֵ
		 uint8_t  valid=0;//��ȷֵ�ĸ���������������������� ������������ж������Ƿ�ȱ��
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if((data[i]>0)&&(data[i]<60))
					{
						   sum+=data[i];
						   valid++;
					}
		 }
		 aver = sum/valid;/*ƽ��ֵ*/
		 val  = (uint16_t)((aver+0.05)*10);//��������֮�����10
		 
		 return val;  //��������󷵻�
}

/****************************************************************
 *����������������޷����������ݻ���ƽ��ֵ����
 *data  Ҫ�������ݵ�����
 *len   ��Ҫ��������ݵĸ���
 ****************************************************************/
uint16_t avergae_rad(uint16_t *data,uint8_t len)
{
	   if(len == 0)
		 {
			    return data[0];
		 }
		 float temp=0;
		 float sum_x=0;//X������֮��
		 float sum_y=0;//Y������֮��
		 float aver=0;//����ƽ��ֵ
		 uint8_t  valid=0;//��ȷֵ�ĸ���������������������� ������������ж������Ƿ�ȱ��
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if((data[i]<=360)&&(data[i]>0))
					{
						   temp   = (((float)data[i])*P)/180;/*�Ƕ�ת��Ϊ����*/
						   sum_x += sin(temp);
						   sum_y += cos(temp);
						   valid++;
					}
		 }
		 sum_x /= valid;
		 sum_y /= valid;
		 aver   = atan2(sum_x,sum_y);
		 aver   = (aver*180)/P;
		 aver+=0.5;  //��������
		 if(aver<0)  //����������atan2()�������Ϊ[-�У���]����[-�У�0)֮���ֵ��2��ת����[�У�2��)�����Եõ�[0��2��)������Ҫ�������ֵ
			 {
				    aver+=360;
			 }
			if(aver>=360) aver=0;
		 
		 return (uint16_t)aver;  //��������󷵻�
}

/****************************************************************
 *��������������޷�����������ƽ��ֵ����
 *data  Ҫ�������ݵ�����
 *len   ��Ҫ��������ݵĸ���
 *qc    ��Ҫ�����ֵ���ʿ���
 *qc_res���������ʿ��� 
 ****************************************************************/
uint16_t avergae_qc(uint16_t *data,uint8_t len,uint8_t *qc,uint8_t *qc_res)
{
	   if(len == 0)
		 {
			    return data[0];
			    //*qc_res = QC_L;/*���ȱ��*/
		 }
		 uint32_t sum=0;//����֮��
		 uint16_t aver=0;//����ƽ��ֵ
		 uint8_t  valid=0;//��ȷֵ�ĸ���������������������� ������������ж������Ƿ�ȱ��
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if(qc[i] ==QC_R)
					{
						   sum+=data[i];
						   valid++;
					}
		 }
		 len *=0.75; /*��Ч�������ڵ��ڲ���ֵ��0.75��*/
		 if(valid <len)
		 {
			    *qc_res = QC_L;/*���ȱ��*/
		 }
		 else if(valid >len-1)
		 {
			    *qc_res = QC_R;/*�����ȷ*/
		 }
		 aver  = sum/valid;/*ƽ��ֵ*/
		 
		 return (uint16_t)aver; 
}

/****************************************************************
 *��������������޷����������ݻ���ƽ��ֵ����
 *data  Ҫ�������ݵ�����
 *len   ��Ҫ��������ݵĸ���
 *qc    ��Ҫ�����ֵ���ʿ���
 *qc_res���������ʿ��� 
 ****************************************************************/
uint16_t avergae_qc_rad(uint16_t *data,uint8_t len,uint8_t *qc,uint8_t *qc_res)
{
	   if(len == 0)
		 {
			    return data[0];
		 }
		 float temp=0;
		 float sum_x=0;//X������֮��
		 float sum_y=0;//Y������֮��
		 float aver=0;//����ƽ��ֵ
		 uint8_t  valid=0;//��ȷֵ�ĸ���������������������� ������������ж������Ƿ�ȱ��
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if(qc[i] ==QC_R)
					{
						   temp = (((float)data[i])*P)/180;/*�Ƕ�ת��Ϊ����*/
						   sum_x+=sin(temp);
						   sum_y+=cos(temp);
						   valid++;
					}
		 }
		 sum_x /= valid;
		 sum_y /= valid;
		 aver   = atan2(sum_x,sum_y);
		 aver   = (aver*180)/P;
		 aver+=0.5;  //��������
		 if(aver<0)  //����������atan2()�������Ϊ[-�У���]����[-�У�0)֮���ֵ��2��ת����[�У�2��)�����Եõ�[0��2��)������Ҫ�������ֵ
			 {
				    aver+=360;
			 }
			if(aver>=360) aver=0;
			 
			//len *=0.75;
			if(valid <len*0.75)
			{
					   *qc_res = QC_L;/*���ȱ��*/
			}
		 else if(valid >0.75*len-1)
			{
						 *qc_res = QC_R;/*�����ȷ*/
			}
		 
		 return (uint16_t)aver; 
}

/*********************************************************************************
 *���������ݺ�������
 *����ID   ID==0 ��䵱ǰ���͵ķ�������   ID==1���洢SD������ʷ����
 *�������ݳ���
 *********************************************************************************/
//uint16_t FillData_Minute(uint8_t ID)
//{
//	           uint16_t num=0;/*�������ݳ���*/
//	           /*�����ʼ��*/
//	           if(ID == 0)
//						 {
//							    num = sprintf(cdata_m.sta_id ," {F<");
//						 }
//						 else
//						 {
//							    num = sprintf(cdata_m.sta_id ,"   [");
//						 }
//						 
//						 #if (ARGEEMENT == SPACE)
//						 /*ʱ��*/
//						 num += sprintf(cdata_m.cur_time ,"20%02u-%02u-%02u %02u:%02u:00,",sys_date.Year ,sys_date.Month ,sys_date.Date ,sys_time.Hours ,sys_time.Minutes);
//						 if((Sensor_Bad_Sec != 0)&&(Sensor_Bad_Sec > 55))
//						 {
//							    Sensor_Bad_Sec = 0;//����
//							    /*����ֵ*/
//							    num += sprintf(cdata_m.f_data .ws_ext_1h_data,"///,"  );
//							    num += sprintf(cdata_m.f_data .wd_ext_1h_data ,"///,"  );
//							 num += sprintf(cdata_m.f_data .w_ext_time_data ,"//://,"   );
//							    /*ʮ��*/
//							    num += sprintf(cdata_m.f_data .wd_10m_data ,"///," );
//							    num += sprintf(cdata_m.f_data .ws_10m_data ,"///,");
//							    /*����*/
//						      num += sprintf(cdata_m.f_data .wd_2m_data ,"///," );
//							    num += sprintf(cdata_m.f_data .ws_2m_data ,"///,"); 
//							     /*һ��*/
//						      num += sprintf(cdata_m.f_data .wd_1m_data ,"///," );
//							    num += sprintf(cdata_m.f_data .ws_1m_data ,"///,");
//							     /*˲ʱ*/
//						      num += sprintf(cdata_m.f_data .wd_1s_data ,"///,"  );
//							    num += sprintf(cdata_m.f_data .ws_1s_data ,"///,");
//							     /*���*/
//						      num += sprintf(cdata_m.f_data .ws_max_1h_data ,"///," );
//						      num += sprintf(cdata_m.f_data .wd_max_1h_data ,"///," );
//						      num += sprintf(cdata_m.f_data .w_max_time_data  ,"//"  );
//						 }
//						 else 
//						 {
//						      /*����ֵ*/
//						      num += sprintf(cdata_m.f_data .ws_ext_1h_data,"%03u,",ws.ws_ext_1h  );
//						      num += sprintf(cdata_m.f_data .wd_ext_1h_data,"%03u,",wd.wd_ext_1h  );
//						      num += sprintf(cdata_m.f_data .w_ext_time_data ,"%02u:%02u,",ws.ws_ext_min ,ws.ws_ext_sec   );
//						      /*ʮ��*/
//						      num += sprintf(cdata_m.f_data .wd_10m_data ,"%03u,",wd.wd_10m );
//							    num += sprintf(cdata_m.f_data .ws_10m_data ,"%03u,",ws.ws_10m );
////						     if(wd.wd_10m_qc ==QC_R)
////						     {
////							         num += sprintf(cdata_m.f_data .wd_10m_data ,"%03u,",wd.wd_10m );
////						     }
////						     else
////						     {
////							         num += sprintf(cdata_m.f_data .wd_10m_data ,"///,");
////						     }
////						     if(ws.ws_10m_qc ==QC_R)
////						     {
////							        num += sprintf(cdata_m.f_data .ws_10m_data ,"%03u,",ws.ws_10m );
////						     }
////						     else
////						     {
////							        num += sprintf(cdata_m.f_data .ws_10m_data ,"///,");
////						     }
//						      /*����*/
//						     num += sprintf(cdata_m.f_data .wd_2m_data ,"%03u,",wd.wd_2m );
//								 num += sprintf(cdata_m.f_data .ws_2m_data ,"%03u,",ws.ws_2m );
////						     if(wd.wd_2m_qc ==QC_R)
////						      {
////							        num += sprintf(cdata_m.f_data .wd_2m_data ,"%03u,",wd.wd_2m );
////						      }
////						     else
////						     {  
////							        num += sprintf(cdata_m.f_data .wd_2m_data ,"///,");
////						     }
////						     if(ws.ws_2m_qc ==QC_R)
////						     { 
////							        num += sprintf(cdata_m.f_data .ws_2m_data ,"%03u,",ws.ws_2m );
////						     }
////						     else
////						     {
////							        num += sprintf(cdata_m.f_data .ws_2m_data ,"///,");
////						     }
//						      /*һ��*/
//						     num += sprintf(cdata_m.f_data .wd_1m_data ,"%03u,",wd.wd_1m );
//								 num += sprintf(cdata_m.f_data .ws_1m_data ,"%03u,",ws.ws_1m );
////						     if(wd.wd_1m_qc ==QC_R)
////						      {
////							         num += sprintf(cdata_m.f_data .wd_1m_data ,"%03u,",wd.wd_1m );
////						      }
////						     else
////						     {
////							        num += sprintf(cdata_m.f_data .wd_1m_data ,"///,");
////						      }
////						    if(ws.ws_1m_qc ==QC_R)
////						     {
////							         num += sprintf(cdata_m.f_data .ws_1m_data ,"%03u,",ws.ws_1m );
////						     }
////						   else
////						    {
////							        num += sprintf(cdata_m.f_data .ws_1m_data ,"///,");
////						    }
//						      /*˲ʱ*/
//						   num += sprintf(cdata_m.f_data .wd_1s_data ,"%03u,",wd.res_wd_1s  );
//							 num += sprintf(cdata_m.f_data .ws_1s_data ,"%03u,",ws.res_ws_1s  );
////						    if(wd.wd_1s_qc ==QC_R)
////						    {
////							       num += sprintf(cdata_m.f_data .wd_1s_data ,"%03u,",wd.res_wd_1s  );
////						    }
////						    else 
////						    {
////							        num += sprintf(cdata_m.f_data .wd_1s_data ,"///,");
////						    }
////						  if(ws.ws_1s_qc ==QC_R)
////						  { 
////							    num += sprintf(cdata_m.f_data .ws_1s_data ,"%03u,",ws.res_ws_1s  );
////						  }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .ws_1s_data ,"///,");
////						 }
//						 /*���*/
//						 num += sprintf(cdata_m.f_data .ws_max_1h_data ,"%03u,",ws.ws_max_1h );
//						 num += sprintf(cdata_m.f_data .wd_max_1h_data ,"%03u,",wd.wd_max_1h );
//						 num += sprintf(cdata_m.f_data .w_max_time_data  ,"%02u",ws.ws_max_min  );
//					 }
//				   #elif (ARGEEMENT == NO_SPACE)
//					 /*ʱ��*/
//						num += sprintf(cdata_m.cur_time ,"%02u%02u%02u%02u%02u,",sys_date.Year ,sys_date.Month ,sys_date.Date ,sys_time.Hours ,sys_time.Minutes);
//					 if((Sensor_Bad_Sec != 0)&&(Sensor_Bad_Sec > 55))
//						 {
//							    Sensor_Bad_Sec = 0;//����
//							    /*����ֵ*/
//							    num += sprintf(cdata_m.f_data .ws_ext_1h_data,"///"  );
//							    num += sprintf(cdata_m.f_data .wd_ext_1h_data ,"///"  );
//							    num += sprintf(cdata_m.f_data .w_ext_time_data ,"////"   );
//							    /*ʮ��*/
//							    num += sprintf(cdata_m.f_data .wd_10m_data ,"///" );
//							    num += sprintf(cdata_m.f_data .ws_10m_data ,"///");
//							    /*����*/
//						      num += sprintf(cdata_m.f_data .wd_2m_data ,"///" );
//							    num += sprintf(cdata_m.f_data .ws_2m_data ,"///"); 
//							     /*һ��*/
//						      num += sprintf(cdata_m.f_data .wd_1m_data ,"///" );
//							    num += sprintf(cdata_m.f_data .ws_1m_data ,"///");
//							     /*˲ʱ*/
//						      num += sprintf(cdata_m.f_data .wd_1s_data ,"///"  );
//							    num += sprintf(cdata_m.f_data .ws_1s_data ,"///");
//							     /*���*/
//						      num += sprintf(cdata_m.f_data .ws_max_1h_data ,"///" );
//						      num += sprintf(cdata_m.f_data .wd_max_1h_data ,"///" );
//						      num += sprintf(cdata_m.f_data .w_max_time_data  ,"//"  );
//						 }
//						 else
//						 {
//						 /*����ֵ*/
//						 num += sprintf(cdata_m.f_data .ws_ext_1h_data,"%03u",ws.ws_ext_1h  );
//						 num += sprintf(cdata_m.f_data .wd_ext_1h_data ,"%03u",wd.wd_ext_1h  );
//						 num += sprintf(cdata_m.f_data .w_ext_time_data ,"%02u%02u",ws.ws_ext_min ,ws.ws_ext_sec   );
//						 /*ʮ��*/
//						 num += sprintf(cdata_m.f_data .wd_10m_data ,"%03u",wd.wd_10m );
//						 num += sprintf(cdata_m.f_data .ws_10m_data ,"%03u",ws.ws_10m );
////						 if(wd.wd_10m_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .wd_10m_data ,"%03u",wd.wd_10m );
////						 }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .wd_10m_data ,"///");
////						 }
////						 if(ws.ws_10m_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .ws_10m_data ,"%03u",ws.ws_10m );
////						 }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .ws_10m_data ,"///");
////						 }
//						  /*����*/
//						 num += sprintf(cdata_m.f_data .wd_2m_data ,"%03u",wd.wd_2m );
//						 num += sprintf(cdata_m.f_data .ws_2m_data ,"%03u",ws.ws_2m );
////						 if(wd.wd_2m_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .wd_2m_data ,"%03u",wd.wd_2m );
////						 }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .wd_2m_data ,"///");
////						 }
////						 if(ws.ws_2m_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .ws_2m_data ,"%03u",ws.ws_2m );
////						 }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .ws_2m_data ,"///");
////						 }
//						  /*һ��*/
//						 num += sprintf(cdata_m.f_data .wd_1m_data ,"%03u",wd.wd_1m );
//						 num += sprintf(cdata_m.f_data .ws_1m_data ,"%03u",ws.ws_1m );
////						 if(wd.wd_1m_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .wd_1m_data ,"%03u",wd.wd_1m );
////						 }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .wd_1m_data ,"///");
////						 }
////						 if(ws.ws_1m_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .ws_1m_data ,"%03u",ws.ws_1m );
////						 }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .ws_1m_data ,"///");
////						 }
//						  /*˲ʱ*/
//						 num += sprintf(cdata_m.f_data .wd_1s_data ,"%03u",wd.res_wd_1s  );
//						 num += sprintf(cdata_m.f_data .ws_1s_data ,"%03u",ws.res_ws_1s  );
////						 if(wd.wd_1s_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .wd_1s_data ,"%03u",wd.res_wd_1s  );
////						 }
////						 else 
////						 {
////							    num += sprintf(cdata_m.f_data .wd_1s_data ,"///");
////						 }
////						 if(ws.ws_1s_qc ==QC_R)
////						 {
////							    num += sprintf(cdata_m.f_data .ws_1s_data ,"%03u",ws.res_ws_1s  );
////						 }
////						 else
////						 {
////							    num += sprintf(cdata_m.f_data .ws_1s_data ,"///");
////						 }
//						 /*���*/
//						 num += sprintf(cdata_m.f_data .ws_max_1h_data ,"%03u",ws.ws_max_1h );
//						 num += sprintf(cdata_m.f_data .wd_max_1h_data ,"%03u",wd.wd_max_1h );
//						 num += sprintf(cdata_m.f_data .w_max_time_data  ,"%02u",ws.ws_max_min  );
//					 }
//						 #endif
//						 /*������*/
//						 if(ID == 0)
//						 {
//							    num += sprintf(cdata_m.end_id ,">}");
//						 }
//						 else
//						 {
//							    num += sprintf(cdata_m.end_id ,"] ");
//						 }
//						 
//						 return num;
//}







