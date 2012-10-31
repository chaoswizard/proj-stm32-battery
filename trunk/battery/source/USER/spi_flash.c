#include "stm32f10x.h"
#include "spi_flash.h"
#include "global.h"
#include "at45db041.h"
#include "common.h"

void SaveSetCfg(void)
{
    gAdSampConfig.checkcode = calc_checkcode(((unsigned  char*)(&gAdSampConfig )+2), sizeof(gAdSampConfig) - 2);
    write_flash((unsigned char*)(&gAdSampConfig),sizeof(gAdSampConfig), SYS_SET_CFG_ADDRESS);    
}
uint8_t GetSetCfg(void)
{
   read_flash((unsigned char*)(&gAdSampConfig),sizeof(gAdSampConfig), SYS_SET_CFG_ADDRESS);
   gAdSampConfig.checkcode = calc_checkcode(((unsigned  char*)(&gAdSampConfig) ), sizeof(gAdSampConfig) );
   if(0 == gAdSampConfig.checkcode)
   {
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
    
    gAdSampConfig.data_group_valid_line_idx[(g_bkpData.group_idx-1)] =  g_bkpData.last_line_in_group;      //从第一行起

    //保存并备份掉电保护区
    SaveBkpData(TABLE_BKP);
    SaveBkpData(TABLE_BAK_BKP);

    SaveSetCfg();    //将最后一个有效行信息写入配置，以便取数据时通过配置头取得相应的组有效行数信息
    
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
    lineIdx = gAdSampConfig.data_group_valid_line_idx[(g_bkpData.group_idx-1)];//取组中的有效行位置
    L_addr = DATA_GROUP1_BASE_ADDR + (g_bkpData.group_idx-1)*DATA_GROUP_SIZE + (lineIdx)*SYS_AD_CH_MAX*2;
    write_flash(pdata, SYS_AD_CH_MAX*2, L_addr);
    g_bkpData.last_line_in_group++;
}

//取spi flash中的历史数据，在这里不校验其是否有效。
void GetAdData(uint8_t groupIdx,uint8_t lineIdx)
{
    unsigned long L_addr;
    L_addr = DATA_GROUP1_BASE_ADDR + (groupIdx-1)*DATA_GROUP_SIZE + lineIdx*SYS_AD_CH_MAX*2;
    read_flash((uint8_t *)batteryVolAdArray, SYS_AD_CH_MAX*2, L_addr);
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


