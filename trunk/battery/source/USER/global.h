#ifndef GLOBAL_H
#define GLOBAL_H

#define VESION  100  //V1.0�汾
#ifdef GLOBAL
	#define EXT_GLOBAL 
#else
	#define EXT_GLOBAL extern
#endif
#include "bits_types.h" 
#include "rtc_time.h"
#include "common_define.h"
#include "syscom_util.h"
#include "spi_flash.h"

#define  SYS_BASE_TICK                (10)              // 10ms
#define  SYS_AD_CH_MAX             (100)            //AD��ͨ·
#define SYS_AD_PERIOD               (9 )                  //5sΪһ�ֲ�������
#define SYS_BKP_TABLE_NUM               2
#define FULL_QUANTIFIER             5000            //mv������
#define DIGIT_16_BITS                  65536            //16bit������Ϊ    

EXT_GLOBAL struct rtc_time systmtime;       //ϵͳʱ����Ϣ����

#define   CFG_DEFAULT_MODE_MIN    (10)                          //���ʱ������Ϊ10min������ֵΪ0~20        
#define  CFG_MODE_MIN_MIN  0
#define CFG_MODE_MIN_MAX  10

#define   CFG_DEFAULT_MODE_VOL   2800                  
#define CFG_MODE_VOL_MIN  0
#define CFG_MODE_VOL_MAX  5000

#define   CFG_DEFAULT_MODE_SEL    (1)                               //��ΪĬ�ϰ���ѹ

#define   CFG_DEFAULT_PARAM_LIMIT         2     //mv
#define CFG_PARAM_LIMIT_MIN  0
#define CFG_PARAM_LIMIT_MAX 10

#define   CFG_DEFAULT_PARAM_REVERSE   2        //mv
#define CFG_PARAM_REVERSE_MIN 0
#define CFG_PARAM_REVERSE_MAX 10

// ��Ӧ��spi flash�е�ϵͳ����
 struct  tag_SAMP_CONFIG{
    u_int16  checkcode;             //У����
    //u_int8  data_group_Idx;     //������������������һ���������µ��顣�����籣��ʱ����Ҫ�õ���
    u_int8  mode_sel;               //ģʽѡ��
    u_int16 mode_vol;               //����ѹvolģʽʱ�ĵ�ѹֵ,��λΪmv
    u_int16 mode_min;               //��ʱ��ģʽʱʱ�䣬��λΪ min
    u_int16 param_limit;        // ���޵�ѹ(����ֵ)���ȸ�ֵҪС��ֵ������Ϊ���� 
    u_int16 param_rervse;       //����ʱ�ĵ�ѹֵ����ϵͳ����ʱǿ��Ϊ0
    u_int16 data_group_valid_line_idx[AD_DATA_GROUP_NUM_MAX]; //AD_DATA_GROUP_NUM_MAX��������ʾ�����ѱ����˼�����Ч����ֵ��ȡ����ʱ���õ�
    u_int8 chsel[(SYS_AD_CH_MAX+7)>>3 ];    //ͨ��ѡ�����ã���Ϊ���á�ͣ�����֣�����13���ֽھͿ��Ա�ʾ
} ;
EXT_GLOBAL struct  tag_SAMP_CONFIG  gAdSampConfig;

EXT_GLOBAL u_int16 g_mode_vol_ad_val,g_param_limit_ad_val,g_param_rervse_ad_val;

#define NO_U_PAN            0
#define HAVE_U_PAN      1

union tag_SysResAndStatus
{
    uint8_t led_buf; //led��ʾbuff��
    struct
    {
        uint8_t sys_end:1;
        uint8_t auto_n_manual:1;             //�Զ����ֶ�
        uint8_t not_discharged:1;                   // 1��ʾu�̣�0��ʾspi
        uint8_t no_u_pan:1;                         // 1��ʾû��u�̣�1����ʾʹ��u��
        uint8_t sys_working:1;
        uint8_t reversed:1;
        uint8_t impend:1;
        uint8_t sys_start:1;
    }sys;
};
//��Ӧ��Bkp�Ĵ����еı������ݣ���������bkp����ȷ������last_line_in_groupΪ׼
//���򣬸�����ȷ�ĵ�����Ĳ��֣�Ȼ����ȷ������ԭϵͳ��
struct tag_BAK_DATA
{
    uint16_t checkcode; //У���
    uint16_t start_group_time; //��ʼ����ʱ��
    uint8_t rtc_need_update; //�Ƿ���Ҫ����rtcʱ��    
    uint8_t group_idx;              //���
    uint16_t last_line_in_group; //�����Ѿ�������ɵ��кţ���ɹ�����������,��Ϊ��һ�б���λ������
    uint8_t ch_work_status[(SYS_AD_CH_MAX+7)>>3 ]; //0:�ŵ������1:�ŵ��У���������������ʾ������ͣ�ã�����ʾΪ�����ŵ����
    uint16_t had_used_time;  //����ʱ�䣬���������������жϣ������¿�ʼ������
    union tag_SysResAndStatus sysResAndStatus; //ϵͳ��Դ��״̬
}; 
enum{
    TABLE_BKP = 0,
    TABLE_BAK_BKP,
    TABLE_INVALID,
};
EXT_GLOBAL struct  tag_BAK_DATA g_bkpData; //�������������ݽṹ

enum{
    CH_STATUS_NORMAL=0,           //����
    CH_STATUS_IMPEND,                   //����
    CH_STATUS_REVERSED,            //����
    CH_STATUS_NO_DISCHARGED,//���ܷŵ�
};
EXT_GLOBAL uint8_t ch_warn_status[SYS_AD_CH_MAX]; //����״̬��ֻ��Ҫһ����ʱ����������Ҫ�뱸������������ԭ

#define SET_SYS_RUN_STATUS_START() do{\
    g_bkpData.sysResAndStatus.sys.sys_start = 1;\
    g_bkpData.sysResAndStatus.sys.sys_end = 0;\
    g_bkpData.sysResAndStatus.sys.sys_working = 0;\
    syncResAndStatusShow();\
    }while(0)
#define SET_SYS_RUN_STATUS_WORKING() do{\
    g_bkpData.sysResAndStatus.sys.sys_start = 0;\
    g_bkpData.sysResAndStatus.sys.sys_end = 0;\
    g_bkpData.sysResAndStatus.sys.sys_working = 1;\
    syncResAndStatusShow();\
    }while(0)
    
#define SET_SYS_RUN_STATUS_END() do{\
    g_bkpData.sysResAndStatus.sys.sys_start = 0;\
    g_bkpData.sysResAndStatus.sys.sys_end= 1;\
    g_bkpData.sysResAndStatus.sys.sys_working = 0;\
    syncResAndStatusShow();\
    }while(0)

#define CLR_SYS_WARN_STATUS() do{\
    g_bkpData.sysResAndStatus.sys.not_discharged=0;\
    g_bkpData.sysResAndStatus.sys.reversed=0;\
    g_bkpData.sysResAndStatus.sys.impend=0;\
    syncResAndStatusShow();\
    }while(0)
void syncResAndStatusShow(void);   
#define IS_SYS_WORKING()   g_bkpData.sysResAndStatus.sys.sys_working  //�����У�����������
#define IS_SYS_START()   g_bkpData.sysResAndStatus.sys.sys_start  //������ʼ���������ã�׼��װ���µ��
#define IS_SYS_END()   g_bkpData.sysResAndStatus.sys.sys_end  //�����У��������ã������ɣ�δ���µ��

#define IS_SYS_NO_U_PAN()     g_bkpData.sysResAndStatus.sys.no_u_pan
#define IS_SYS_MANUAL()     g_bkpData.sysResAndStatus.sys.auto_n_manual
#define SET_SYS_NO_U_PAN() g_bkpData.sysResAndStatus.sys.no_u_pan = 1
#define SET_SYS_MANUAL() g_bkpData.sysResAndStatus.sys.auto_n_manual = 1
#define SET_SYS_AUTO() g_bkpData.sysResAndStatus.sys.auto_n_manual = 0


EXT_GLOBAL unsigned char g_curAdCh ;    //��ǰad������ͨ·�ţ�1~ 100
EXT_GLOBAL unsigned char g_LastAdCh;    //���Ĳ���ͨ·�ţ�������Բ�ʹ��,����last_line_in_group����
EXT_GLOBAL u_int16  g_sysTimeTickCount ; //ϵͳʱ�䣬��10ms��Ϊһ������

EXT_GLOBAL u_int8 RelayStatus[(SYS_AD_CH_MAX+7)>>3 ]; //�̵���״̬���ɿ�������֣�����13�ֽڼ��ɱ��
EXT_GLOBAL u_int16 batteryVolAdArray[SYS_AD_CH_MAX] ; //��ص�ѹ���ݻ�������100·������洢����ADֵ


EXT_GLOBAL u_int32	curKey;
EXT_GLOBAL u_int32	preKey;
EXT_GLOBAL u_int8	keyDelay; //���ڰ���ʱ�Ĵ������ǽ����ж���ֱ�ӵ��ð�������
EXT_GLOBAL volatile struct {
	unsigned int vv0				:1;
	unsigned int vv1				:1;
	unsigned int vv2				:1;
	unsigned int vv3				:1;
	unsigned int vv4				:1;
	unsigned int vv5				:1;
	unsigned int vv6				:1;
	unsigned int vv7				:1;	
}publicBitValtable;
#define	Flag_keyPressed		publicBitValtable.vv0 //������Ч������ʶ
#define	Flag_10ms			publicBitValtable.vv4  // 10ms�¼�������Ŀǰ��������ɨ��

void   lock_all_int(void);
void   unlock_all_int(void);
void Dump_ChipSerialNum(void);

#if 0
EXT_GLOBAL uint32_t IntDeviceSerial[3];

#define CHECK_VER_RIGHT()  {\
       while ( (*(__IO uint32_t*)(0x1FFFF7E8) != (0x5D6FF31) )|| \
        (*(__IO uint32_t*)(0x1FFFF7EC) != (0x36335933) ) ||\
        (*(__IO uint32_t*)(0x1FFFF7F0)!= (0x43014257) )){\
            xprintf("ErrorCopy\n");\
        };\
}
#else
#define CHECK_VER_RIGHT()
#endif

//����ͨ��ѡ�񻺳�����������λ
#define  TST_AD_SAMP_SEL_TST(num)  UTIL_LBIT_TST((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_SET(num)    UTIL_LBIT_VAL((gAdSampConfig.chsel),  num) = UTIL_LBIT_SET((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_CLR(num)    UTIL_LBIT_VAL((gAdSampConfig.chsel),  num) = UTIL_LBIT_CLR((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_CPL(num)    UTIL_LBIT_VAL((gAdSampConfig.chsel),  num) = UTIL_LBIT_CPL((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_ALL_CLR()  memset(gAdSampConfig.chsel, 0, sizeof(gAdSampConfig.chsel))
#define  TST_AD_SAMP_SEL_ALL_SET()   memset(gAdSampConfig.chsel, 0xFF, sizeof(gAdSampConfig.chsel))

#define  TST_AD_SAMP_SEL_CHK()            util_array_check((gAdSampConfig.chsel),  SYS_AD_CH_MAX)

//////////////�̵�����������������λ
#define  TST_RELAY_SEL_TST(num)  UTIL_LBIT_TST((RelayStatus),  num)
#define  TST_RELAY_SEL_SET(num)    UTIL_LBIT_VAL((RelayStatus),  num) = UTIL_LBIT_SET((RelayStatus),  num)
#define  TST_RELAY_SEL_CLR(num)    UTIL_LBIT_VAL((RelayStatus),  num) = UTIL_LBIT_CLR((RelayStatus),  num)
#define  TST_RELAY_SEL_CPL(num)    UTIL_LBIT_VAL((RelayStatus),  num) = UTIL_LBIT_CPL((RelayStatus),  num)
#define  TST_RELAY_SEL_ALL_CLR()  memset(RelayStatus, 0, sizeof(RelayStatus))
#define  TST_RELAY_ALL_SET()   memset(RelayStatus, 0xFF, sizeof(RelayStatus))

#define  TST_RELAY_SEL_CHK()            util_array_check((RelayStatus),  SYS_AD_CH_MAX)


///////////ͨ·�ķŵ�״̬����ʾΪ1:"�ŵ���"��0:"�ŵ����"��"�ŵ����(����)"�������������·ͣ��
#define  TST_CH_WORK_STATE_TST(num)  UTIL_LBIT_TST((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_SET(num)    UTIL_LBIT_VAL((g_bkpData.ch_work_status),  num) = UTIL_LBIT_SET((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_CLR(num)    UTIL_LBIT_VAL((g_bkpData.ch_work_status),  num) = UTIL_LBIT_CLR((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_CPL(num)    UTIL_LBIT_VAL((g_bkpData.ch_work_status),  num) = UTIL_LBIT_CPL((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_ALL_CLR()  memset(g_bkpData.ch_work_status, 0, sizeof(g_bkpData.ch_work_status))
#define  TST_CH_WORK_STATE_ALL_SET()   memset(g_bkpData.ch_work_status, 0xFF, sizeof(g_bkpData.ch_work_status))

#define  TST_CH_WORK_STATE_SEL_CHK()            util_array_check((g_bkpData.ch_work_status),  SYS_AD_CH_MAX)

void SpiSysDefCfgInit(void);  //����spi_flash��ʵ�֣���Ϊ������spi flash�С�

//5D6FF31,-36335933-43014257
#endif //GLOBAL_H

