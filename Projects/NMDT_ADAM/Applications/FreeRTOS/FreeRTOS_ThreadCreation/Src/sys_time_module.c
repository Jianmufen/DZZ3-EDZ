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
#define systimeSTACK_SIZE   384//configMINIMAL_STACK_SIZE  /*由于时间任务调用文件系统，所以需要堆栈较大  384满足很多任务的需求还不够的话可以设置为512，若堆栈大小不够，调用次任务时，会导致系统重启*/
#define systimePRIORITY     osPriorityHigh
#define P   3.1415926

static FIL file;          /* File object structure (FIL) */
//static uint32_t min_count;//开机之后分钟累计值
//static uint8_t  Channel_min;//通道模式时间累计值分钟
//static uint8_t  Sensor_Bad_Sec=0;//超声风传感器损坏或者没有插上超声风传感器秒计数
//static uint8_t open_file = 0;


/* RTC Time*/
static RTC_TimeTypeDef sys_time;
static RTC_DateTypeDef sys_date;

struct MIN_DATA min_data;
////写入数据缓存
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
		//printf("对时成功\r\n");
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
											printf("开始补要数据\r\n");
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
//						//时间
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
//						//时间在加1分钟
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
						
						//printf("0秒的值：%s\r\n", min_data.data_00);
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
		

				//清零值
				ws.windspeed = 0;
				wd.winddirection = 0;
				wd.wd_1s_qc = QC_L;
				ws.ws_1s_qc = QC_L;
				
				//<2018-03-12 09:05:00,012.33,123,>
				/*每分钟的01秒时开始向SD卡里存储一分钟数据*/
				if(sys_time.Seconds == 59)
				{
						 	/*存储数据到SD卡*/
							offset=680 * sys_time.Minutes;
							snprintf((char *)pathfile,sizeof(pathfile),"/DATA/%02u/%02u_%02u.txt",sys_date.Month,sys_date.Date,sys_time.Hours);
							res = f_open(&file,(const char *)pathfile, FA_OPEN_ALWAYS|FA_WRITE);
							if(res==FR_OK)
								{
									   if(command.state == true)
										 {
											    printf("打开文件成功\r\n");
										 }
									   res=f_lseek(&file,offset);
									   if(res!=FR_OK)
										  {
											    //printf("文件选址错误:%d\r\n",res);
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
																		printf("文件选址成功:%d\r\n",res);
																	}														    	  
										 }
										 if(command.state == true)
										 {
													printf("写入的内容：{%s}\r\n", (char *)&min_data);
										 }
										
									  res=f_write(&file,(uint8_t *)&min_data,676,&byteswritten1);
										if(res!=FR_OK)
										{
											    if(command.state == true)
														{
															printf("写数据失败：%d\r\n",res);
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
															printf("写数据成功:%d\r\n",res);
														}				
													
										}	
									 res=f_close(&file);	
									 if(res==FR_OK)
										{
											    if(command.state == true)
														{
															printf("关闭文件成功:%d\r\n",res);
														}																
										}							
							}
							else 
							{
									printf("打开文件失败：%d\r\n", res);
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
				
				
					
//				/*判断超声风传感器损坏，即一直接收不到传感器的数据*/
//				if(hz.Number_Data ==0)
//				{
//					   Sensor_Bad_Sec++;//超声风传感器未插或者损坏的第二分钟开始每秒计数
//				}
//				/*如果接收传感器有效数据个数为0，则瞬时风缺测*/
//				if(hz.Number_Data == 0)
//				{
//					   wd.wd_1s_qc  =QC_L;//风向缺测
//						 ws.ws_1s_qc  =QC_L;//风速缺测
//				}
//				
//				/*调试*/
//				if(command.sample == true)
//				{
//					   printf("风向采样值：%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d\r\n",wd.wd_buf_3s[0],wd.wd_buf_3s[1],wd.wd_buf_3s[2],wd.wd_buf_3s[3],
//					   wd.wd_buf_3s[4],wd.wd_buf_3s[5],wd.wd_buf_3s[6],wd.wd_buf_3s[7],wd.wd_buf_3s[8],wd.wd_buf_3s[9],wd.wd_buf_3s[10],wd.wd_buf_3s[11]);
//					
//					   printf("风速采样值：%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f\r\n",ws.ws_buf_3s[0],ws.ws_buf_3s[1],ws.ws_buf_3s[2],ws.ws_buf_3s[3],
//					   ws.ws_buf_3s[4],ws.ws_buf_3s[5],ws.ws_buf_3s[6],ws.ws_buf_3s[7],ws.ws_buf_3s[8],ws.ws_buf_3s[9],ws.ws_buf_3s[10],ws.ws_buf_3s[11]);
//				}
//				/*瞬时风速风向*/
//				if(ws.ws_1s_qc == QC_R)
//				{
//					   ws.ws_1s  = avergae(ws.ws_buf_3s ,12);
//				}
//				wd.wd_1s  = avergae_qc_rad(wd.wd_buf_3s ,12,wd.wd_1s_12qc ,&wd.wd_1s_qc );
//				
//				/*极大风速 小时内最大的3秒瞬时风速*/
//				if(ws.ws_1s > ws.ws_ext_1h )
//					{
//						   //ws.ws_ext_h   = ws.windspeed ;
//						   ws.ws_ext_1h  = ws.ws_1s;
//						   wd.wd_ext_1h  = wd.winddirection ;
//						   get_sys_time(&sys_date,&sys_time);/*得到对应的时间*/
//						   ws.ws_ext_min    = sys_time.Minutes ;
//						   ws.ws_ext_sec    = sys_time.Seconds ;
//						   //printf("极大风向：%d  %d\r\n",wd.winddirection,wd.wd_ext_1h);
//		  		}
//				
//				/*调试*/
//				if(command.instant == true)
//				{
//					printf("瞬时风向：%d  瞬时风速：%d  瞬时风向质控:%d  瞬时风速质控：%d\r\n",wd.wd_1s,ws.ws_1s,wd.wd_1s_qc,ws.ws_1s_qc);
//				}
//				
//				/*1分钟采样值*/
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
//				/*整分钟时刻即00秒*/
//				if(sys_time.Seconds == 0)
//				{
//					   /*存储发送的瞬时风*/
//					   ws.res_ws_1s = ws.ws_1s ;
//					   wd.res_wd_1s = wd.wd_1s ;
//					
//					   /*1分钟平均风*/
//					   ws.ws_1m = avergae_qc(ws.ws_buf_1m ,60,ws.ws_1m_60qc ,&ws.ws_1m_qc );
//					   wd.wd_1m = avergae_qc_rad(wd.wd_buf_1m ,60,wd.wd_1m_60qc ,&wd.wd_1m_qc );
//					
//					   /*最近60分钟的1分钟平均数据采样*/
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
//					   /*2分钟平均风*/
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
//						 /*10分钟平均风计算*/
//						if(min_count >=8)  //采样值不够10分钟，按10*0.75=7.5次为有效采样值  否则缺测
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
//						 /*小时最大风速采样值*/
//						 if(ws.ws_10m >ws.ws_max_1h )
//							 {
//     								 ws.ws_max_1h  = ws.ws_10m ;
//			    					 wd.wd_max_1h  = wd.wd_10m ;
//					    			 ws.ws_max_min = sys_time.Minutes ;
//							 }
//						
//						 /*填充发送数据缓存，发送数据到上位机*/
//						 len = FillData_Minute(0);
//						 HAL_UART_Transmit(&huart2,(uint8_t *)&cdata_m,len,0xFF);
//							 
//						if(command.state == true)
//						{
//							   printf("数据长度：%d\r\n",len);
//						     printf("质控：%d-%d-%d-%d-%d-%d-%d-%d\r\n",wd.wd_10m_qc ,ws.ws_10m_qc ,wd.wd_2m_qc ,ws.ws_2m_qc ,wd.wd_1m_qc ,ws.ws_1m_qc ,wd.wd_1s_qc ,ws.ws_1s_qc );
//						}
//				}
//				
//				/*00分02秒时清零极大、最大风速*/
//				if((sys_time.Minutes == 0)&&(sys_time.Seconds == 02))
//				{
//					   /*极大值清零*/
//					   ws.ws_ext_1h  = 0;
//					   wd.wd_ext_1h  = 0;
//					   ws.ws_ext_min = 01;
//					   ws.ws_ext_sec = 01;
//					   /*最大值清零*/
//					   ws.ws_max_1h  = 0;
//					   wd.wd_max_1h  = 0;
//					   ws.ws_max_min = 01;
//				}
//				
//				/*02秒时开始计数开机分钟*/
//				if(sys_time.Seconds == 03)
//				{
//					   min_count++;
//					   if(command.state == true)
//						 {
//							    printf("开机时间：%d分钟\r\n",min_count);
//							    printf("设备工作时间时间：%d小时\r\n",min_count/60);
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
				
				/*每分钟的01秒时开始向SD卡里存储一分钟数据*/
//				if(sys_time.Seconds == 01)
//				{
//					   FRESULT  res=FR_OK;
//    				 uint32_t byteswritten=0;
//					   char pathfile[24]={0};
//						 uint32_t offset=0;
//						 
//						 /*填充历史数据*/
//						 len = FillData_Minute(1);
//						 	/*存储数据到SD卡*/
//							offset=64*sys_time.Minutes;
//							snprintf((char *)pathfile,sizeof(pathfile),"/DATA/%02u/%02u_%02u.txt",sys_date.Month,sys_date.Date,sys_time.Hours);
//							res = f_open(&file,(const char *)pathfile, FA_OPEN_ALWAYS|FA_WRITE);
//							if(res==FR_OK)
//								{
//									   if(command.state == true)
//										 {
//											    printf("打开文件成功\r\n");
//										 }
//									   res=f_lseek(&file,offset);
//									   if(res!=FR_OK)
//										  {
//											    //printf("文件选址错误:%d\r\n",res);
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
//																		printf("文件选址成功:%d\r\n",res);
//																	}														    	  
//										 }							 
//									  res=f_write(&file,(uint8_t *)&cdata_m,64,&byteswritten);
//										if(res!=FR_OK)
//										{
//											    if(command.state == true)
//														{
//															printf("写数据失败：%d\r\n",res);
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
//															printf("写数据成功:%d\r\n",res);
//														}				
//													
//										}	
//									 res=f_close(&file);	
//									 if(res==FR_OK)
//										{
//											    if(command.state == true)
//														{
//															printf("关闭文件成功:%d\r\n",res);
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
				
//				/*判断传感器输出数据的频率*/
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
       
				/* synchronize internal RTC with pcf8563对时 */
        if((sys_time.Minutes==25) && (sys_time.Seconds==15))
        {
					//printf("对时成功\r\n");
          sync_time();
        }	
        /* Release mutex */
        osMutexRelease(mutex);
			} 
      else
      {
        //printf("没有等到互斥信号量\r\n");
      }
    
    
    
    if(hiwdg.Instance)
    {
      HAL_IWDG_Refresh(&hiwdg);  /* refresh the IWDG */
			//printf("喂狗了\r\n");
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
		//printf("闹钟A中断产生了\r\n");
    osSemaphoreRelease(semaphore);
  }
}


/****************************************************************
 *不带质量控制码的浮点型数据平均值计算
 *data  要计算数据的数组
 *len   需要计算的数据的个数
 ****************************************************************/
uint16_t avergae(float *data,uint8_t len)
{
	   if(len == 0)
		 {
			    return data[0];
		 }
		 float sum=0;//数据之和
		 float aver=0;//计算平均值
		 uint16_t val;//平均值
		 uint8_t  valid=0;//正确值的个数，即参与计算的数组个数 根据这个可以判断数据是否缺测
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if((data[i]>0)&&(data[i]<60))
					{
						   sum+=data[i];
						   valid++;
					}
		 }
		 aver = sum/valid;/*平均值*/
		 val  = (uint16_t)((aver+0.05)*10);//四舍五入之后乘以10
		 
		 return val;  //四舍五入后返回
}

/****************************************************************
 *不带质量控制码的无符号整形数据弧度平均值计算
 *data  要计算数据的数组
 *len   需要计算的数据的个数
 ****************************************************************/
uint16_t avergae_rad(uint16_t *data,uint8_t len)
{
	   if(len == 0)
		 {
			    return data[0];
		 }
		 float temp=0;
		 float sum_x=0;//X轴数据之和
		 float sum_y=0;//Y轴数据之和
		 float aver=0;//计算平均值
		 uint8_t  valid=0;//正确值的个数，即参与计算的数组个数 根据这个可以判断数据是否缺测
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if((data[i]<=360)&&(data[i]>0))
					{
						   temp   = (((float)data[i])*P)/180;/*角度转换为弧度*/
						   sum_x += sin(temp);
						   sum_y += cos(temp);
						   valid++;
					}
		 }
		 sum_x /= valid;
		 sum_y /= valid;
		 aver   = atan2(sum_x,sum_y);
		 aver   = (aver*180)/P;
		 aver+=0.5;  //四舍五入
		 if(aver<0)  //风向修正，atan2()函数结果为[-π，π]，将[-π，0)之间的值加2π转换到[π，2π)，可以得到[0，2π)的满足要求的修正值
			 {
				    aver+=360;
			 }
			if(aver>=360) aver=0;
		 
		 return (uint16_t)aver;  //四舍五入后返回
}

/****************************************************************
 *带质量控制码的无符号整型数据平均值计算
 *data  要计算数据的数组
 *len   需要计算的数据的个数
 *qc    需要计算的值得质控码
 *qc_res计算结果的质控码 
 ****************************************************************/
uint16_t avergae_qc(uint16_t *data,uint8_t len,uint8_t *qc,uint8_t *qc_res)
{
	   if(len == 0)
		 {
			    return data[0];
			    //*qc_res = QC_L;/*标记缺测*/
		 }
		 uint32_t sum=0;//数据之和
		 uint16_t aver=0;//计算平均值
		 uint8_t  valid=0;//正确值的个数，即参与计算的数组个数 根据这个可以判断数据是否缺测
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if(qc[i] ==QC_R)
					{
						   sum+=data[i];
						   valid++;
					}
		 }
		 len *=0.75; /*有效样本大于等于采样值的0.75倍*/
		 if(valid <len)
		 {
			    *qc_res = QC_L;/*标记缺测*/
		 }
		 else if(valid >len-1)
		 {
			    *qc_res = QC_R;/*标记正确*/
		 }
		 aver  = sum/valid;/*平均值*/
		 
		 return (uint16_t)aver; 
}

/****************************************************************
 *带质量控制码的无符号整型数据弧度平均值计算
 *data  要计算数据的数组
 *len   需要计算的数据的个数
 *qc    需要计算的值得质控码
 *qc_res计算结果的质控码 
 ****************************************************************/
uint16_t avergae_qc_rad(uint16_t *data,uint8_t len,uint8_t *qc,uint8_t *qc_res)
{
	   if(len == 0)
		 {
			    return data[0];
		 }
		 float temp=0;
		 float sum_x=0;//X轴数据之和
		 float sum_y=0;//Y轴数据之和
		 float aver=0;//计算平均值
		 uint8_t  valid=0;//正确值的个数，即参与计算的数组个数 根据这个可以判断数据是否缺测
		 uint8_t i=0;
		 
		 for(i=0;i<len;i++)
		 {
			    if(qc[i] ==QC_R)
					{
						   temp = (((float)data[i])*P)/180;/*角度转换为弧度*/
						   sum_x+=sin(temp);
						   sum_y+=cos(temp);
						   valid++;
					}
		 }
		 sum_x /= valid;
		 sum_y /= valid;
		 aver   = atan2(sum_x,sum_y);
		 aver   = (aver*180)/P;
		 aver+=0.5;  //四舍五入
		 if(aver<0)  //风向修正，atan2()函数结果为[-π，π]，将[-π，0)之间的值加2π转换到[π，2π)，可以得到[0，2π)的满足要求的修正值
			 {
				    aver+=360;
			 }
			if(aver>=360) aver=0;
			 
			//len *=0.75;
			if(valid <len*0.75)
			{
					   *qc_res = QC_L;/*标记缺测*/
			}
		 else if(valid >0.75*len-1)
			{
						 *qc_res = QC_R;/*标记正确*/
			}
		 
		 return (uint16_t)aver; 
}

/*********************************************************************************
 *填充分钟数据函数数据
 *参数ID   ID==0 填充当前发送的分钟数据   ID==1填充存储SD卡的历史数据
 *返回数据长度
 *********************************************************************************/
//uint16_t FillData_Minute(uint8_t ID)
//{
//	           uint16_t num=0;/*填充的数据长度*/
//	           /*填充起始符*/
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
//						 /*时间*/
//						 num += sprintf(cdata_m.cur_time ,"20%02u-%02u-%02u %02u:%02u:00,",sys_date.Year ,sys_date.Month ,sys_date.Date ,sys_time.Hours ,sys_time.Minutes);
//						 if((Sensor_Bad_Sec != 0)&&(Sensor_Bad_Sec > 55))
//						 {
//							    Sensor_Bad_Sec = 0;//清零
//							    /*极大值*/
//							    num += sprintf(cdata_m.f_data .ws_ext_1h_data,"///,"  );
//							    num += sprintf(cdata_m.f_data .wd_ext_1h_data ,"///,"  );
//							 num += sprintf(cdata_m.f_data .w_ext_time_data ,"//://,"   );
//							    /*十分*/
//							    num += sprintf(cdata_m.f_data .wd_10m_data ,"///," );
//							    num += sprintf(cdata_m.f_data .ws_10m_data ,"///,");
//							    /*二分*/
//						      num += sprintf(cdata_m.f_data .wd_2m_data ,"///," );
//							    num += sprintf(cdata_m.f_data .ws_2m_data ,"///,"); 
//							     /*一分*/
//						      num += sprintf(cdata_m.f_data .wd_1m_data ,"///," );
//							    num += sprintf(cdata_m.f_data .ws_1m_data ,"///,");
//							     /*瞬时*/
//						      num += sprintf(cdata_m.f_data .wd_1s_data ,"///,"  );
//							    num += sprintf(cdata_m.f_data .ws_1s_data ,"///,");
//							     /*最大*/
//						      num += sprintf(cdata_m.f_data .ws_max_1h_data ,"///," );
//						      num += sprintf(cdata_m.f_data .wd_max_1h_data ,"///," );
//						      num += sprintf(cdata_m.f_data .w_max_time_data  ,"//"  );
//						 }
//						 else 
//						 {
//						      /*极大值*/
//						      num += sprintf(cdata_m.f_data .ws_ext_1h_data,"%03u,",ws.ws_ext_1h  );
//						      num += sprintf(cdata_m.f_data .wd_ext_1h_data,"%03u,",wd.wd_ext_1h  );
//						      num += sprintf(cdata_m.f_data .w_ext_time_data ,"%02u:%02u,",ws.ws_ext_min ,ws.ws_ext_sec   );
//						      /*十分*/
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
//						      /*二分*/
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
//						      /*一分*/
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
//						      /*瞬时*/
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
//						 /*最大*/
//						 num += sprintf(cdata_m.f_data .ws_max_1h_data ,"%03u,",ws.ws_max_1h );
//						 num += sprintf(cdata_m.f_data .wd_max_1h_data ,"%03u,",wd.wd_max_1h );
//						 num += sprintf(cdata_m.f_data .w_max_time_data  ,"%02u",ws.ws_max_min  );
//					 }
//				   #elif (ARGEEMENT == NO_SPACE)
//					 /*时间*/
//						num += sprintf(cdata_m.cur_time ,"%02u%02u%02u%02u%02u,",sys_date.Year ,sys_date.Month ,sys_date.Date ,sys_time.Hours ,sys_time.Minutes);
//					 if((Sensor_Bad_Sec != 0)&&(Sensor_Bad_Sec > 55))
//						 {
//							    Sensor_Bad_Sec = 0;//清零
//							    /*极大值*/
//							    num += sprintf(cdata_m.f_data .ws_ext_1h_data,"///"  );
//							    num += sprintf(cdata_m.f_data .wd_ext_1h_data ,"///"  );
//							    num += sprintf(cdata_m.f_data .w_ext_time_data ,"////"   );
//							    /*十分*/
//							    num += sprintf(cdata_m.f_data .wd_10m_data ,"///" );
//							    num += sprintf(cdata_m.f_data .ws_10m_data ,"///");
//							    /*二分*/
//						      num += sprintf(cdata_m.f_data .wd_2m_data ,"///" );
//							    num += sprintf(cdata_m.f_data .ws_2m_data ,"///"); 
//							     /*一分*/
//						      num += sprintf(cdata_m.f_data .wd_1m_data ,"///" );
//							    num += sprintf(cdata_m.f_data .ws_1m_data ,"///");
//							     /*瞬时*/
//						      num += sprintf(cdata_m.f_data .wd_1s_data ,"///"  );
//							    num += sprintf(cdata_m.f_data .ws_1s_data ,"///");
//							     /*最大*/
//						      num += sprintf(cdata_m.f_data .ws_max_1h_data ,"///" );
//						      num += sprintf(cdata_m.f_data .wd_max_1h_data ,"///" );
//						      num += sprintf(cdata_m.f_data .w_max_time_data  ,"//"  );
//						 }
//						 else
//						 {
//						 /*极大值*/
//						 num += sprintf(cdata_m.f_data .ws_ext_1h_data,"%03u",ws.ws_ext_1h  );
//						 num += sprintf(cdata_m.f_data .wd_ext_1h_data ,"%03u",wd.wd_ext_1h  );
//						 num += sprintf(cdata_m.f_data .w_ext_time_data ,"%02u%02u",ws.ws_ext_min ,ws.ws_ext_sec   );
//						 /*十分*/
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
//						  /*二分*/
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
//						  /*一分*/
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
//						  /*瞬时*/
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
//						 /*最大*/
//						 num += sprintf(cdata_m.f_data .ws_max_1h_data ,"%03u",ws.ws_max_1h );
//						 num += sprintf(cdata_m.f_data .wd_max_1h_data ,"%03u",wd.wd_max_1h );
//						 num += sprintf(cdata_m.f_data .w_max_time_data  ,"%02u",ws.ws_max_min  );
//					 }
//						 #endif
//						 /*结束符*/
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







