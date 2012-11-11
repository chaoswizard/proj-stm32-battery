#include "stm32f10x.h"
#include "spi_flash.h"
#include "global.h"
#include "at45db041.h"
#include "common.h"

void SaveSetCfg(void)
{
    uint32_t tmp;
    if(IS_SYS_WORKING())
    {
        xprintf("working, not modify ad cfg!!\n");
    }
    else
    {
        tmp =gAdSampConfig.mode_vol*DIGIT_16_BITS/FULL_QUANTIFIER;
        g_mode_vol_ad_val = tmp;
        tmp = gAdSampConfig.param_limit*DIGIT_16_BITS/FULL_QUANTIFIER;
        g_param_limit_ad_val = tmp;
        tmp = gAdSampConfig.param_rervse*DIGIT_16_BITS/FULL_QUANTIFIER;
        g_param_rervse_ad_val = tmp;           
        gAdSampConfig.checkcode = calc_checkcode(((unsigned  char*)(&gAdSampConfig )+2), sizeof(gAdSampConfig) - 2);
        write_flash((unsigned char*)(&gAdSampConfig),sizeof(gAdSampConfig), SYS_SET_CFG_ADDRESS);    
    }
}
uint8_t GetSetCfg(void)
{
    uint32_t tmp;
   read_flash((unsigned char*)(&gAdSampConfig),sizeof(gAdSampConfig), SYS_SET_CFG_ADDRESS);
   gAdSampConfig.checkcode = calc_checkcode(((unsigned  char*)(&gAdSampConfig) ), sizeof(gAdSampConfig) );
   if(0 == gAdSampConfig.checkcode)
   {
        tmp =gAdSampConfig.mode_vol*DIGIT_16_BITS/FULL_QUANTIFIER;
        g_mode_vol_ad_val = tmp;
        tmp = gAdSampConfig.param_limit*DIGIT_16_BITS/FULL_QUANTIFIER;
        g_param_limit_ad_val = tmp;
        tmp = gAdSampConfig.param_rervse*DIGIT_16_BITS/FULL_QUANTIFIER;
        g_param_rervse_ad_val = tmp;       
        xprintf("%d\n%d\n",g_param_limit_ad_val,g_param_rervse_ad_val);
        return TRUE;
   }
   else
   {
        return FALSE;
   }
}

void MoveToNextGroup(void)
{
    g_bkpData.group_idx++;               //初始化为0,但实际从1开始,在每次保存行之前调用

    if(g_bkpData.group_idx > AD_DATA_GROUP_NUM_MAX)
    {
        g_bkpData.group_idx = AD_DATA_GROUP_NUM_MIN;
    }
    g_bkpData.last_line_in_group = 0;

}
void MoveToNextLine(void)
{
    g_bkpData.last_line_in_group++;   //初始化为 0
}
void SaveAdData(uint8_t* pdata)
{
    unsigned long L_addr;
    uint16_t lineIdx ;    
    lineIdx = g_bkpData.last_line_in_group;                                                 //取组中的有效行位置
    L_addr = DATA_GROUP1_BASE_ADDR + (g_bkpData.group_idx-1)*DATA_GROUP_SIZE + (lineIdx)*SYS_AD_CH_MAX*2;
    write_flash(pdata, SYS_AD_CH_MAX*2, L_addr);
    //g_bkpData.last_line_in_group++;
}

//取spi flash中的历史数据，在这里不校验其是否有效。
void GetAdData(uint8_t groupIdx,uint8_t lineIdx,uint16_t* buf)
{
    unsigned long L_addr;
    L_addr = DATA_GROUP1_BASE_ADDR + (groupIdx-1)*DATA_GROUP_SIZE + lineIdx*SYS_AD_CH_MAX*2;
    read_flash((uint8_t *)buf, SYS_AD_CH_MAX*2, L_addr);
}

void SpiSysDefCfgInit(void)
{
    uint8_t i = 0;
    
    gAdSampConfig.checkcode = 0;
    for(i=0;i<((SYS_AD_CH_MAX+7)>>3);i++)
    {
         gAdSampConfig.chsel[i] = 0xff;                         //默认全选
    }
   for(i=0;i<AD_DATA_GROUP_NUM_MAX;i++)
    {
         gAdSampConfig.data_group_valid_line_idx[i] = 0;
    }
    gAdSampConfig.mode_min = CFG_DEFAULT_MODE_MIN;
    gAdSampConfig.mode_sel = CFG_DEFAULT_MODE_SEL;
    gAdSampConfig.mode_vol = CFG_DEFAULT_MODE_VOL;
    gAdSampConfig.param_limit = CFG_DEFAULT_PARAM_LIMIT;
    gAdSampConfig.param_rervse = CFG_DEFAULT_PARAM_REVERSE;

   SaveSetCfg();       //保存默认配置，以便下次再进入时读取           

}


