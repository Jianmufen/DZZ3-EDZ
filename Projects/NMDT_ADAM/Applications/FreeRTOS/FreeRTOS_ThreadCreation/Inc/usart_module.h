/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_MODULE_H
#define __USART_MODULE_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "main.h"
#include "time.h"
	 
#include "ff_gen_drv.h"
#include "sd_diskio.h"
extern	  int year2  , month2 , day2 , hour2 , minute2  , second2;   //补要数据起始月日时
extern unsigned int download_flag;
extern uint8_t download_number;
extern uint8_t test;
extern uint8_t Speed_Flag;//0代表风速低于0.05    1代表风速大于等于0.05
extern uint8_t Collector_Mode;
extern uint8_t Sensor_Mode;
extern uint8_t Sensor_Configuration;
extern uint8_t Collecter_Key;

#define 	 NO_SPACE  0    //通讯协议数据之间无间隔
#define 	 SPACE     1	  //通讯协议数据之间有间隔
	 
#define ARGEEMENT  NO_SPACE     //无间隔


struct MIN_DATA
{
		char min_time[14];  //<201803120920,
		char data_00[11];		//012.34,234,
		char data_01[11];		//012.34-234,
		char data_02[11];		//012.34-234,
		char data_03[11];		//012.34-234,
		char data_04[11];		//012.34-234,
		char data_05[11];		//012.34-234,
		char data_06[11];		//012.34-234,
		char data_07[11];		//012.34-234,
		char data_08[11];		//012.34-234,
		char data_09[11];		//012.34,234,
		char data_10[11];		//012.34,234,
		char data_11[11];		//012.34,234,
		char data_12[11];		//012.34,234,
		char data_13[11];		//012.34,234,
		char data_14[11];		//012.34,234,
		char data_15[11];		//012.34,234,
		char data_16[11];		//012.34,234,
		char data_17[11];		//012.34,234,
		char data_18[11];		//012.34,234,
		char data_19[11];		//012.34,234,
		char data_20[11];		//012.34,234,
		char data_21[11];		//012.34,234,
		char data_22[11];		//012.34,234,
		char data_23[11];		//012.34,234,
		char data_24[11];		//012.34,234,
		char data_25[11];		//012.34,234,
		char data_26[11];		//012.34,234,
		char data_27[11];		//012.34,234,
		char data_28[11];		//012.34,234,
		char data_29[11];		//012.34,234,
		char data_30[11];		//012.34,234,
		char data_31[11];		//012.34,234,
		char data_32[11];		//012.34,234,
		char data_33[11];		//012.34,234,
		char data_34[11];		//012.34,234,
		char data_35[11];		//012.34,234,
		char data_36[11];		//012.34,234,
		char data_37[11];		//012.34,234,
		char data_38[11];		//012.34,234,
		char data_39[11];		//012.34,234,
		char data_40[11];		//012.34,234,
		char data_41[11];		//012.34,234,
		char data_42[11];		//012.34,234,
		char data_43[11];		//012.34,234,
		char data_44[11];		//012.34,234,
		char data_45[11];		//012.34,234,
		char data_46[11];		//012.34,234,
		char data_47[11];		//012.34,234,
		char data_48[11];		//012.34,234,
		char data_49[11];		//012.34,234,
		char data_50[11];		//012.34,234,
		char data_51[11];		//012.34,234,
		char data_52[11];		//012.34,234,
		char data_53[11];		//012.34,234,
		char data_54[11];		//012.34,234,
		char data_55[11];		//012.34,234,
		char data_56[11];		//012.34,234,
		char data_57[11];		//012.34,234,
		char data_58[11];		//012.34,234,
		char data_59[14];		//012.34,234>\r\n
};

	 
#if (ARGEEMENT == SPACE)
typedef struct  //发送数据包结构
 {
   //uint8_t sData[42];   //要发送的数据：小时极大值风速风向分秒，十分风，2分风，1分风，瞬时风，小时最大风分
	 char ws_ext_1h_data[4];//小时极大风速
	 char wd_ext_1h_data[4];//小时极大风向
	 char w_ext_time_data[6];//小时极大风速对应的时间：分秒17:15
	 char wd_10m_data[4];//十分平均风向
	 char ws_10m_data[4];//十分平均风速
	 char wd_2m_data[4];//2分平均风向
	 char ws_2m_data[4];//2分平均风速
	 char wd_1m_data[4];//1分平均风向
	 char ws_1m_data[4];//1分平均风速
	 char wd_1s_data[4];//瞬时平均风向
	 char ws_1s_data[4];//瞬时平均风速
	 char ws_max_1h_data[4];//小时最大风速
	 char wd_max_1h_data[4];//小时最大风向
	 char w_max_time_data[2];//小时最大风速对应的时间：分
 }SensorDataType_t;
	 
typedef struct   //通讯帧数据包头   分钟数据和历史数据
{
  char sta_id[4];   //起始符  分钟数据：" {F<"  历史数据："   ["
  char cur_time[20]; //时间：年月日时分2017-10-07 15:17:00
  SensorDataType_t f_data;
  char end_id[2];   //结束符  分钟数据：">}"    历史数据："] "
}CommuinicateData_t;
extern CommuinicateData_t cdata_m;

#elif (ARGEEMENT == NO_SPACE)
typedef struct  //发送数据包结构
 {
   //uint8_t sData[42];   //要发送的数据：小时极大值风速风向分秒，十分风，2分风，1分风，瞬时风，小时最大风分
	 char ws_ext_1h_data[3];//小时极大风速
	 char wd_ext_1h_data[3];//小时极大风向
	 char w_ext_time_data[4];//小时极大风速对应的时间：分秒17:15
	 char wd_10m_data[3];//十分平均风向
	 char ws_10m_data[3];//十分平均风速
	 char wd_2m_data[3];//2分平均风向
	 char ws_2m_data[3];//2分平均风速
	 char wd_1m_data[3];//1分平均风向
	 char ws_1m_data[3];//1分平均风速
	 char wd_1s_data[3];//瞬时平均风向
	 char ws_1s_data[3];//瞬时平均风速
	 char ws_max_1h_data[3];//小时最大风速
	 char wd_max_1h_data[3];//小时最大风向
	 char w_max_time_data[2];//小时最大风速对应的时间：分
 }SensorDataType_t;
	 
typedef struct   //通讯帧数据包头   分钟数据和历史数据
{
  char sta_id[4];   //起始符  分钟数据：" {F<"  历史数据："   ["
  char cur_time[10]; //时间：年月日时分2017-10-07 15:17:00
  SensorDataType_t f_data;
  char end_id[2];   //结束符  分钟数据：">}"    历史数据："] "
}CommuinicateData_t;
extern CommuinicateData_t cdata_m;
#endif

typedef struct  //风速数据
 {
   float windspeed;  //采集超声风传感器的数值  
	 float ws_buf_3s[12]; //保存过去3秒12个采样值
	 
	 uint16_t ws_1s;      //风速瞬时值
	 uint16_t res_ws_1s;   //用于保存整分时的风速瞬时值
	 
	 uint16_t ws_1m;      //1分钟风速平均值
	 uint16_t ws_buf_1m[60];  //保存过去1分钟60个瞬时风速值
	 
	 uint16_t ws_2m;      //2分钟风速平均值
	 //uint16_t ws_buf_2m[120];  //保存过去2分钟120个瞬时风速值
	 
	 uint16_t ws_10m;      //10分钟风速平均值
	 //uint16_t ws_buf_10m[10];  //保存过去10分钟10个1分钟平均风速值
	 
	 uint16_t ws_buf_60m[60];  //保存过去一小时60个1分钟平均风速值
	 
	 //float    ws_ext_h;  //小时内最大的采样值 为浮点型数据
	 uint16_t ws_ext_1h;  //小时极大风速 对应最大采样值风速的十倍四舍五入
	 uint16_t ws_ext_min;  //小时极大风速 对应的分钟
	 uint16_t ws_ext_sec;  //小时极大风速 对应的秒钟
	 uint16_t ws_max_1h;  //小时最大风速 对应10分钟平均风速的最大值
	 uint16_t ws_max_min;  //小时最大风速 对应的分钟
	 
	 uint8_t ws_1s_qc;//瞬时风速质控
	 uint8_t ws_1m_qc;//1分钟风速平均值质控
	 uint8_t ws_1m_60qc[60];//1分钟内60个瞬时风速质控数组
	 uint8_t ws_2m_qc;//1分钟风速平均值质控
	 //uint8_t ws_2m_120qc[120];//2分钟内120个瞬时风速质控数组
	 uint8_t ws_10m_qc;//1分钟风速平均值质控
	 //uint8_t ws_10m_10qc[10];//10分钟内10个1分钟平均风速风速质控数组
	 uint8_t ws_60m_60qc[60];//60分钟内60个一分钟平均风速质控数组
	 
 }WindSpeed_t;
extern WindSpeed_t ws;

typedef struct  //风向数据
 {
   uint32_t winddirection;  //采集超声风传感器的数值  
	 uint16_t wd_buf_3s[12];//3秒内12个采样值
	 
	 uint16_t wd_1s;      //风向瞬时值等于整秒时的风向采样值
	 uint16_t res_wd_1s;   //用于保存整分时的风向瞬时值
	 
	 uint16_t wd_1m;      //1分钟风向平均值
	 uint16_t wd_buf_1m[60];  //保存过去1分钟60个瞬时风向值
	 
	 uint16_t wd_2m;      //2分钟风向平均值
	 //uint16_t wd_buf_2m[120];  //保存过去2分钟120个瞬时风向值
	 
	 uint16_t wd_10m;      //10分钟风向平均值
	 //uint16_t wd_buf_10m[10];  //保存过去10分钟10个1分钟平均风向值
	 
	 uint16_t wd_buf_60m[60];  //保存过去一小时60个1分钟平均风向值
	 
	 uint16_t wd_ext_1h;  //小时极大风向 小时内极大风速对应的风向
	 uint16_t wd_max_1h;  //小时最大风向 小时内最大风速对应的风向
	 
	 uint8_t wd_1s_qc;//瞬时风向质控
	 uint8_t wd_1s_12qc[12];//最近3秒内12个风向质控
	 uint8_t wd_1m_qc;//1分钟风速平均值质控
	 uint8_t wd_1m_60qc[60];//1分钟内60个瞬时风速质控数组
	 uint8_t wd_2m_qc;//1分钟风速平均值质控
	 //uint8_t wd_2m_120qc[120];//2分钟内120个瞬时风速质控数组
	 uint8_t wd_10m_qc;//1分钟风速平均值质控
	 //uint8_t wd_10m_10qc[10];//10分钟内10个1分钟平均风速风速质控数组
	 uint8_t wd_60m_60qc[60];//60分钟内60个一分钟平均风向质控数组
	 uint8_t wd_ext_1h_qc;//小时最大风向质控
	 
 }WindDirection_t;
extern WindDirection_t wd;

#define QC_R 0  //数据正确
#define QC_L 1  //数据缺测

/*传感器输出频率标记*/
typedef struct  //发送数据包结构
 {
   uint32_t Number_Data;//1分钟内主采集器串口1接收到传感器数据的个数
	 uint8_t  flag_hz;    //输出频率标志
 }SensorHZ_t;
extern  SensorHZ_t hz;
#define HZ_8  8 //1秒钟4条数据
#define HZ_4  4 //1秒钟2条数据
#define HZ_2  2 //1秒钟1条数据
#define HZ_1  1 //2秒钟1条数据
#define HZ_0  0 //4秒钟1条数据
 
int32_t init_usart_module(void);
int32_t start_storage(void);

	 
#ifdef __cplusplus
}
#endif
#endif /*__STORAGE_MODULE_H */
