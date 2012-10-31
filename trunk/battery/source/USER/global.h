#ifndef GLOBAL_H
#define GLOBAL_H

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
#define  SYS_AD_CH_MAX             (100)            //AD的通路
#define SYS_AD_PERIOD               (5 )                  //5s为一轮采样周期
#define SYS_BKP_TABLE_NUM               2

EXT_GLOBAL struct rtc_time systmtime;       //系统时间信息变量

#define   CFG_DEFAULT_MODE_MIN    (20)
#define   CFG_DEFAULT_MODE_VOL    (2300)
#define   CFG_DEFAULT_MODE_SEL    (0)
#define   CFG_DEFAULT_PARAM_LIMIT         (200)
#define   CFG_DEFAULT_PARAM_REVERSE    (4800)

// 对应于spi flash中的系统配置
 struct  tag_SAMP_CONFIG{
    u_int16  checkcode;             //校验码
    //u_int8  data_group_Idx;     //数据组索引，决定哪一个才是最新的组。而掉电保护时不需要用到它
    u_int8  mode_sel;               //模式选择
    u_int16 mode_vol;               //按电压vol模式时的电压值,单位为mv
    u_int16 mode_min;               //按时间模式时时间，单位为 min
    u_int16 param_limit;        // 极限电压(下限值)，比该值要小的值，可视为悬空 
    u_int16 param_rervse;       //反向时的电压值。本系统反向时强制为0
    u_int16 data_group_valid_line_idx[AD_DATA_GROUP_NUM_MAX]; //AD_DATA_GROUP_NUM_MAX，用来表示该组已保存了几行有效采样值，取数据时会用到
    u_int8 chsel[(SYS_AD_CH_MAX+7)>>3 ];    //通道选择配置，分为可用、停用两种，故用13个字节就可以表示
} ;
EXT_GLOBAL struct  tag_SAMP_CONFIG  gAdSampConfig;



#define NO_U_PAN            0
#define HAVE_U_PAN      1
union tag_SysResAndStatus
{
    uint8_t led_buf; //led显示buff，
    struct
    {
        uint8_t sys_end:1;
        uint8_t read_cfg_fail:1;
        uint8_t writed_protected:1;
        uint8_t no_u_pan:1;
        uint8_t sys_working:1;
        uint8_t reversed:1;
        uint8_t impend:1;
        uint8_t sys_start:1;
    }sys;
};
//对应于Bkp寄存器中的备份数据，若两部分bkp都正确，则以last_line_in_group为准
//否则，复制正确的到错误的部分，然后按正确的来还原系统。
struct tag_BAK_DATA
{
    uint16_t checkcode; //校验和
    uint16_t start_group_time; //开始新组时间
    uint8_t rtc_need_update; //是否需要更新rtc时钟    
    uint8_t group_idx;              //组号
    uint16_t last_line_in_group; //组内已经保存完成的行号，存成功后自增该数,作为下一行保存位置依据
    uint8_t ch_work_status[(SYS_AD_CH_MAX+7)>>3 ]; //0:放电结束；1:放电中，这在主界面是显示，对于停用，则显示为高亮放电结束
    uint8_t ch_warn_status[(SYS_AD_CH_MAX+7)>>3 ]; //警告，0:悬空,1:反向
    uint16_t had_used_time;  //已用时间，可用作结束条件判断，在重新开始后清零
    union tag_SysResAndStatus sysResAndStatus; //系统资源和状态
}; //这里共38个字节
enum{
    TABLE_BKP = 0,
    TABLE_BAK_BKP,
    TABLE_INVALID,
};
//EXT_GLOBAL uint8_t g_usingBkpIdx ;   //TABLE_BKP,TABLE_BAK_BKP
EXT_GLOBAL struct  tag_BAK_DATA g_bkpData; //备份数据区数据结构
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
void syncResAndStatusShow(void);   
#define IS_SYS_WORKING()   g_bkpData.sysResAndStatus.sys.sys_working  //工作中，不允许配置
#define IS_SYS_NO_U_PAN()     g_bkpData.sysResAndStatus.sys.no_u_pan
#define SET_SYS_NO_U_PAN() g_bkpData.sysResAndStatus.sys.no_u_pan = 1

EXT_GLOBAL unsigned char g_curAdCh ;    //当前ad采样的通路号，1~ 100
EXT_GLOBAL unsigned char g_LastAdCh;    //最后的采样通路号，这个可以不使用,可由last_line_in_group代替
EXT_GLOBAL u_int16  g_sysTimeTickCount ; //系统时间，以10ms作为一个步长

EXT_GLOBAL u_int8 RelayStatus[(SYS_AD_CH_MAX+7)>>3 ]; //继电器状态，由开与合两种，故用13字节即可表达
EXT_GLOBAL u_int16 batteryVolAdArray[SYS_AD_CH_MAX] ; //电池电压数据缓冲区，100路，这里存储的是AD值


EXT_GLOBAL u_int32	curKey;
EXT_GLOBAL u_int32	preKey;
EXT_GLOBAL u_int8	keyDelay; //用于按键时的处理，我们将在中断中直接调用按键处理
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
#define	Flag_keyPressed		publicBitValtable.vv0 //产生有效按键标识
#define	Flag_10ms			publicBitValtable.vv4  // 10ms事件产生，目前用作按键扫描

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

//采样通道选择缓冲区操作，按位
#define  TST_AD_SAMP_SEL_TST(num)  UTIL_LBIT_TST((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_SET(num)    UTIL_LBIT_VAL((gAdSampConfig.chsel),  num) = UTIL_LBIT_SET((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_CLR(num)    UTIL_LBIT_VAL((gAdSampConfig.chsel),  num) = UTIL_LBIT_CLR((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_CPL(num)    UTIL_LBIT_VAL((gAdSampConfig.chsel),  num) = UTIL_LBIT_CPL((gAdSampConfig.chsel),  num)
#define  TST_AD_SAMP_SEL_ALL_CLR()  memset(gAdSampConfig.chsel, 0, sizeof(gAdSampConfig.chsel))
#define  TST_AD_SAMP_SEL_ALL_SET()   memset(gAdSampConfig.chsel, 0xFF, sizeof(gAdSampConfig.chsel))

#define  TST_AD_SAMP_SEL_CHK()            util_array_check((gAdSampConfig.chsel),  SYS_AD_CH_MAX)

//////////////继电器缓冲区操作，按位
#define  TST_RELAY_SEL_TST(num)  UTIL_LBIT_TST((RelayStatus),  num)
#define  TST_RELAY_SEL_SET(num)    UTIL_LBIT_VAL((RelayStatus),  num) = UTIL_LBIT_SET((RelayStatus),  num)
#define  TST_RELAY_SEL_CLR(num)    UTIL_LBIT_VAL((RelayStatus),  num) = UTIL_LBIT_CLR((RelayStatus),  num)
#define  TST_RELAY_SEL_CPL(num)    UTIL_LBIT_VAL((RelayStatus),  num) = UTIL_LBIT_CPL((RelayStatus),  num)
#define  TST_RELAY_SEL_ALL_CLR()  memset(RelayStatus, 0, sizeof(RelayStatus))
#define  TST_RELAY_ALL_SET()   memset(RelayStatus, 0xFF, sizeof(RelayStatus))

#define  TST_RELAY_SEL_CHK()            util_array_check((RelayStatus),  SYS_AD_CH_MAX)


///////////通路的放电状态，显示为1:"放电中"，0:"放电结束"，"放电结束(高亮)"，这高亮表明该路停用
#define  TST_CH_WORK_STATE_TST(num)  UTIL_LBIT_TST((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_SET(num)    UTIL_LBIT_VAL((g_bkpData.ch_work_status),  num) = UTIL_LBIT_SET((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_CLR(num)    UTIL_LBIT_VAL((g_bkpData.ch_work_status),  num) = UTIL_LBIT_CLR((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_CPL(num)    UTIL_LBIT_VAL((g_bkpData.ch_work_status),  num) = UTIL_LBIT_CPL((g_bkpData.ch_work_status),  num)
#define  TST_CH_WORK_STATE_ALL_CLR()  memset(g_bkpData.ch_work_status, 0, sizeof(g_bkpData.ch_work_status))
#define  TST_CH_WORK_STATE_ALL_SET()   memset(g_bkpData.ch_work_status, 0xFF, sizeof(g_bkpData.ch_work_status))

#define  TST_CH_WORK_STATE_SEL_CHK()            util_array_check((g_bkpData.ch_work_status),  SYS_AD_CH_MAX)

///////////通路的警告状态，显示为0:悬空，1:反向
#define  TST_CH_WARN_STATE_TST(num)  UTIL_LBIT_TST((g_bkpData.ch_warn_status),  num)
#define  TST_CH_WARN_STATE_SET(num)    UTIL_LBIT_VAL((g_bkpData.ch_warn_status),  num) = UTIL_LBIT_SET((g_bkpData.ch_warn_status),  num)
#define  TST_CH_WARN_STATE_CLR(num)    UTIL_LBIT_VAL((g_bkpData.ch_warn_status),  num) = UTIL_LBIT_CLR((g_bkpData.ch_warn_status),  num)
#define  TST_CH_WARN_STATE_CPL(num)    UTIL_LBIT_VAL((g_bkpData.ch_warn_status),  num) = UTIL_LBIT_CPL((g_bkpData.ch_warn_status),  num)
#define  TST_CH_WARN_STATE_ALL_CLR()  memset(g_bkpData.ch_warn_status, 0, sizeof(g_bkpData.ch_warn_status))
#define  TST_CH_WARN_STATE_ALL_SET()   memset(g_bkpData.ch_warn_status, 0xFF, sizeof(g_bkpData.ch_warn_status))

#define  TST_CH_WARN_STATE_SEL_CHK()            util_array_check((g_bkpData.ch_warn_status),  SYS_AD_CH_MAX)
void SpiSysDefCfgInit(void);  //放在spi_flash中实现，因为其存放在spi flash中。

//5D6FF31,-36335933-43014257
#endif //GLOBAL_H

