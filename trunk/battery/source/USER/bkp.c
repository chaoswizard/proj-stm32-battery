#include "stm32f10x.h"
#include "bkp.h"
#include "global.h"
#include "common.h"

#include "input_output.h"

/***********************************************
基本设计思路: 主要是使用两部分备份
***********************************************/

#define BKP_DATA_SIZE  sizeof(g_bkpData)


static uint16_t  gbkpAddr_table_1[] = {
    BKP_DR2, BKP_DR3, BKP_DR4, BKP_DR5,BKP_DR6,//BKP_DR1, 被用在RTC中了
    BKP_DR7, BKP_DR8, BKP_DR9, BKP_DR10, BKP_DR11,BKP_DR12,
    BKP_DR13, BKP_DR14,BKP_DR15,BKP_DR16, BKP_DR17,BKP_DR18,
    BKP_DR19, BKP_DR20,BKP_DR21/*,BKP_DR22, BKP_DR23,BKP_DR24,
    BKP_DR25, BKP_DR26,BKP_DR27,BKP_DR28, BKP_DR29,BKP_DR30,
    BKP_DR31, BKP_DR32,BKP_DR33,BKP_DR34, BKP_DR35,BKP_DR36,
    BKP_DR37, BKP_DR38,BKP_DR39,BKP_DR40, BKP_DR41,BKP_DR42,*/
};
static uint16_t  gbkpAddr_table_2[] = {
    /*BKP_DR2, BKP_DR3, BKP_DR4, BKP_DR5,BKP_DR6,//BKP_DR1, 被用在RTC中了
    BKP_DR7, BKP_DR8, BKP_DR9, BKP_DR10, BKP_DR11,BKP_DR12,
    BKP_DR13, BKP_DR14,BKP_DR15,BKP_DR16, BKP_DR17,BKP_DR18,
    BKP_DR19, BKP_DR20,BKP_DR21,*/BKP_DR23,BKP_DR24,//BKP_DR22,可挪作它用
    BKP_DR25, BKP_DR26,BKP_DR27,BKP_DR28, BKP_DR29,BKP_DR30,
    BKP_DR31, BKP_DR32,BKP_DR33,BKP_DR34, BKP_DR35,BKP_DR36,
    BKP_DR37, BKP_DR38,BKP_DR39,BKP_DR40, BKP_DR41,BKP_DR42,
};

static uint16_t  *bkpTable[SYS_BKP_TABLE_NUM] = {gbkpAddr_table_1, gbkpAddr_table_2};

void  bkp_write(struct tag_BAK_DATA *data,uint16_t table[])
{
        uint16_t  *pData = (uint16_t*) data;
        uint16_t  i, size;

        //size = ARRAY_SIZE(table);
        size = BKP_DATA_SIZE;
        for (i=0;size>0; size-=2)
        {
               BKP_WriteBackupRegister(table[i], *pData);
                pData++;
                i++;
        }
}

void  bkp_read(struct tag_BAK_DATA *data,uint16_t table[])  //变成普通指针
{
        uint16_t  *pData = (uint16_t*) data;
        uint16_t  i, size;

        //size = ARRAY_SIZE(table);
       size = BKP_DATA_SIZE;
        for (i=0;size>0; size-=2)
        {
               (*pData) = BKP_ReadBackupRegister(table[i] );
                pData++;
                i++;
        }
}

void SaveBkpData(uint8_t idx)
{
    g_bkpData.checkcode = calc_checkcode(((unsigned  char*)(&g_bkpData)+2), sizeof(g_bkpData) - 2);  
    bkp_write(&g_bkpData,bkpTable[idx]);  
}

uint8_t GetBkpData(uint8_t idx)
{
    bkp_read(&g_bkpData,bkpTable[idx]);    //这里倒有备份掉电保护区的索引
    g_bkpData.checkcode = calc_checkcode(((unsigned  char*)(&g_bkpData )), sizeof(g_bkpData) );   
   if(0 == g_bkpData.checkcode)
   {
        return TRUE;
   }
   else
   {
        return FALSE;
   }
}


uint8_t checkBkp(void)         //掉电保护时，才调用该函数，即它在主过程之前的检测环境部分
{
    uint8_t ret = TABLE_INVALID;
     if(GetBkpData(TABLE_BKP))               
     {
          ret = TABLE_BKP;
      }
     else if(GetBkpData(TABLE_BAK_BKP))
     {
         ret = TABLE_BAK_BKP;
     }
     else
     {
        ret = TABLE_INVALID;
     }
    return ret;
}

void bkpSysDefBakInit(void)
{
    uint8_t j;

    for(j=0;j<(SYS_AD_CH_MAX+7)>>3;j++)
    {
        g_bkpData.ch_work_status[j] = 0;   //全部放电结束，显示时，这里的查询都需工作中才有效
    }
    g_bkpData.start_group_time = 0;
    g_bkpData.group_idx = 0;
    g_bkpData.had_used_time = 0;
    g_bkpData.last_line_in_group = 0;
    g_bkpData.rtc_need_update = 0;
    g_bkpData.start_group_time = 0;
    g_bkpData.sysResAndStatus.led_buf= LED_START|LED_NO_U_PAN; //置为开始，即等待开始

    SaveBkpData(TABLE_BKP);
    SaveBkpData(TABLE_BAK_BKP);


}

void bkp_test(void)
{
    uint16_t temp;

     if (BKP_ReadBackupRegister(BKP_DR22) != 0xAAAC)
     {
        g_bkpData.last_line_in_group = 6;
        xprintf("save ...\n" );
        SaveBkpData(0);
        g_bkpData.last_line_in_group = 5;

        BKP_WriteBackupRegister(BKP_DR22, 0xAAAC);
   }
   else
   {
        if(GetBkpData(0))
        {
            xprintf("group = %d\n", g_bkpData.last_line_in_group );
        }
        else
        {
            xprintf("error\n");
        }
   }

while(1);
}


