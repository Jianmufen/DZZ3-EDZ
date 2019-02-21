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
extern	  int year2  , month2 , day2 , hour2 , minute2  , second2;   //��Ҫ������ʼ����ʱ
extern unsigned int download_flag;
extern uint8_t download_number;
extern uint8_t test;
extern uint8_t Speed_Flag;//0������ٵ���0.05    1������ٴ��ڵ���0.05
extern uint8_t Collector_Mode;
extern uint8_t Sensor_Mode;
extern uint8_t Sensor_Configuration;
extern uint8_t Collecter_Key;

#define 	 NO_SPACE  0    //ͨѶЭ������֮���޼��
#define 	 SPACE     1	  //ͨѶЭ������֮���м��
	 
#define ARGEEMENT  NO_SPACE     //�޼��


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
typedef struct  //�������ݰ��ṹ
 {
   //uint8_t sData[42];   //Ҫ���͵����ݣ�Сʱ����ֵ���ٷ�����룬ʮ�ַ磬2�ַ磬1�ַ磬˲ʱ�磬Сʱ�����
	 char ws_ext_1h_data[4];//Сʱ�������
	 char wd_ext_1h_data[4];//Сʱ�������
	 char w_ext_time_data[6];//Сʱ������ٶ�Ӧ��ʱ�䣺����17:15
	 char wd_10m_data[4];//ʮ��ƽ������
	 char ws_10m_data[4];//ʮ��ƽ������
	 char wd_2m_data[4];//2��ƽ������
	 char ws_2m_data[4];//2��ƽ������
	 char wd_1m_data[4];//1��ƽ������
	 char ws_1m_data[4];//1��ƽ������
	 char wd_1s_data[4];//˲ʱƽ������
	 char ws_1s_data[4];//˲ʱƽ������
	 char ws_max_1h_data[4];//Сʱ������
	 char wd_max_1h_data[4];//Сʱ������
	 char w_max_time_data[2];//Сʱ�����ٶ�Ӧ��ʱ�䣺��
 }SensorDataType_t;
	 
typedef struct   //ͨѶ֡���ݰ�ͷ   �������ݺ���ʷ����
{
  char sta_id[4];   //��ʼ��  �������ݣ�" {F<"  ��ʷ���ݣ�"   ["
  char cur_time[20]; //ʱ�䣺������ʱ��2017-10-07 15:17:00
  SensorDataType_t f_data;
  char end_id[2];   //������  �������ݣ�">}"    ��ʷ���ݣ�"] "
}CommuinicateData_t;
extern CommuinicateData_t cdata_m;

#elif (ARGEEMENT == NO_SPACE)
typedef struct  //�������ݰ��ṹ
 {
   //uint8_t sData[42];   //Ҫ���͵����ݣ�Сʱ����ֵ���ٷ�����룬ʮ�ַ磬2�ַ磬1�ַ磬˲ʱ�磬Сʱ�����
	 char ws_ext_1h_data[3];//Сʱ�������
	 char wd_ext_1h_data[3];//Сʱ�������
	 char w_ext_time_data[4];//Сʱ������ٶ�Ӧ��ʱ�䣺����17:15
	 char wd_10m_data[3];//ʮ��ƽ������
	 char ws_10m_data[3];//ʮ��ƽ������
	 char wd_2m_data[3];//2��ƽ������
	 char ws_2m_data[3];//2��ƽ������
	 char wd_1m_data[3];//1��ƽ������
	 char ws_1m_data[3];//1��ƽ������
	 char wd_1s_data[3];//˲ʱƽ������
	 char ws_1s_data[3];//˲ʱƽ������
	 char ws_max_1h_data[3];//Сʱ������
	 char wd_max_1h_data[3];//Сʱ������
	 char w_max_time_data[2];//Сʱ�����ٶ�Ӧ��ʱ�䣺��
 }SensorDataType_t;
	 
typedef struct   //ͨѶ֡���ݰ�ͷ   �������ݺ���ʷ����
{
  char sta_id[4];   //��ʼ��  �������ݣ�" {F<"  ��ʷ���ݣ�"   ["
  char cur_time[10]; //ʱ�䣺������ʱ��2017-10-07 15:17:00
  SensorDataType_t f_data;
  char end_id[2];   //������  �������ݣ�">}"    ��ʷ���ݣ�"] "
}CommuinicateData_t;
extern CommuinicateData_t cdata_m;
#endif

typedef struct  //��������
 {
   float windspeed;  //�ɼ������紫��������ֵ  
	 float ws_buf_3s[12]; //�����ȥ3��12������ֵ
	 
	 uint16_t ws_1s;      //����˲ʱֵ
	 uint16_t res_ws_1s;   //���ڱ�������ʱ�ķ���˲ʱֵ
	 
	 uint16_t ws_1m;      //1���ӷ���ƽ��ֵ
	 uint16_t ws_buf_1m[60];  //�����ȥ1����60��˲ʱ����ֵ
	 
	 uint16_t ws_2m;      //2���ӷ���ƽ��ֵ
	 //uint16_t ws_buf_2m[120];  //�����ȥ2����120��˲ʱ����ֵ
	 
	 uint16_t ws_10m;      //10���ӷ���ƽ��ֵ
	 //uint16_t ws_buf_10m[10];  //�����ȥ10����10��1����ƽ������ֵ
	 
	 uint16_t ws_buf_60m[60];  //�����ȥһСʱ60��1����ƽ������ֵ
	 
	 //float    ws_ext_h;  //Сʱ�����Ĳ���ֵ Ϊ����������
	 uint16_t ws_ext_1h;  //Сʱ������� ��Ӧ������ֵ���ٵ�ʮ����������
	 uint16_t ws_ext_min;  //Сʱ������� ��Ӧ�ķ���
	 uint16_t ws_ext_sec;  //Сʱ������� ��Ӧ������
	 uint16_t ws_max_1h;  //Сʱ������ ��Ӧ10����ƽ�����ٵ����ֵ
	 uint16_t ws_max_min;  //Сʱ������ ��Ӧ�ķ���
	 
	 uint8_t ws_1s_qc;//˲ʱ�����ʿ�
	 uint8_t ws_1m_qc;//1���ӷ���ƽ��ֵ�ʿ�
	 uint8_t ws_1m_60qc[60];//1������60��˲ʱ�����ʿ�����
	 uint8_t ws_2m_qc;//1���ӷ���ƽ��ֵ�ʿ�
	 //uint8_t ws_2m_120qc[120];//2������120��˲ʱ�����ʿ�����
	 uint8_t ws_10m_qc;//1���ӷ���ƽ��ֵ�ʿ�
	 //uint8_t ws_10m_10qc[10];//10������10��1����ƽ�����ٷ����ʿ�����
	 uint8_t ws_60m_60qc[60];//60������60��һ����ƽ�������ʿ�����
	 
 }WindSpeed_t;
extern WindSpeed_t ws;

typedef struct  //��������
 {
   uint32_t winddirection;  //�ɼ������紫��������ֵ  
	 uint16_t wd_buf_3s[12];//3����12������ֵ
	 
	 uint16_t wd_1s;      //����˲ʱֵ��������ʱ�ķ������ֵ
	 uint16_t res_wd_1s;   //���ڱ�������ʱ�ķ���˲ʱֵ
	 
	 uint16_t wd_1m;      //1���ӷ���ƽ��ֵ
	 uint16_t wd_buf_1m[60];  //�����ȥ1����60��˲ʱ����ֵ
	 
	 uint16_t wd_2m;      //2���ӷ���ƽ��ֵ
	 //uint16_t wd_buf_2m[120];  //�����ȥ2����120��˲ʱ����ֵ
	 
	 uint16_t wd_10m;      //10���ӷ���ƽ��ֵ
	 //uint16_t wd_buf_10m[10];  //�����ȥ10����10��1����ƽ������ֵ
	 
	 uint16_t wd_buf_60m[60];  //�����ȥһСʱ60��1����ƽ������ֵ
	 
	 uint16_t wd_ext_1h;  //Сʱ������� Сʱ�ڼ�����ٶ�Ӧ�ķ���
	 uint16_t wd_max_1h;  //Сʱ������ Сʱ�������ٶ�Ӧ�ķ���
	 
	 uint8_t wd_1s_qc;//˲ʱ�����ʿ�
	 uint8_t wd_1s_12qc[12];//���3����12�������ʿ�
	 uint8_t wd_1m_qc;//1���ӷ���ƽ��ֵ�ʿ�
	 uint8_t wd_1m_60qc[60];//1������60��˲ʱ�����ʿ�����
	 uint8_t wd_2m_qc;//1���ӷ���ƽ��ֵ�ʿ�
	 //uint8_t wd_2m_120qc[120];//2������120��˲ʱ�����ʿ�����
	 uint8_t wd_10m_qc;//1���ӷ���ƽ��ֵ�ʿ�
	 //uint8_t wd_10m_10qc[10];//10������10��1����ƽ�����ٷ����ʿ�����
	 uint8_t wd_60m_60qc[60];//60������60��һ����ƽ�������ʿ�����
	 uint8_t wd_ext_1h_qc;//Сʱ�������ʿ�
	 
 }WindDirection_t;
extern WindDirection_t wd;

#define QC_R 0  //������ȷ
#define QC_L 1  //����ȱ��

/*���������Ƶ�ʱ��*/
typedef struct  //�������ݰ��ṹ
 {
   uint32_t Number_Data;//1���������ɼ�������1���յ����������ݵĸ���
	 uint8_t  flag_hz;    //���Ƶ�ʱ�־
 }SensorHZ_t;
extern  SensorHZ_t hz;
#define HZ_8  8 //1����4������
#define HZ_4  4 //1����2������
#define HZ_2  2 //1����1������
#define HZ_1  1 //2����1������
#define HZ_0  0 //4����1������
 
int32_t init_usart_module(void);
int32_t start_storage(void);

	 
#ifdef __cplusplus
}
#endif
#endif /*__STORAGE_MODULE_H */
