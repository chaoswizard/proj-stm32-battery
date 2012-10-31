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
    g_bkpData.group_idx++;               //��ʼ��Ϊ0,��ʵ�ʴ�1��ʼ,��ÿ�α�����֮ǰ����

    if(g_bkpData.group_idx > AD_DATA_GROUP_NUM_MAX)
    {
        g_bkpData.group_idx = AD_DATA_GROUP_NUM_MIN;
    }
    
    gAdSampConfig.data_group_valid_line_idx[(g_bkpData.group_idx-1)] =  g_bkpData.last_line_in_group;      //�ӵ�һ����

    //���沢���ݵ��籣����
    SaveBkpData(TABLE_BKP);
    SaveBkpData(TABLE_BAK_BKP);

    SaveSetCfg();    //�����һ����Ч����Ϣд�����ã��Ա�ȡ����ʱͨ������ͷȡ����Ӧ������Ч������Ϣ
    
   g_bkpData.last_line_in_group = 0;

}
void MoveToNextLine(void)
{
    g_bkpData.last_line_in_group++;   //��ʼ��Ϊ 0
}
void SaveAdData(uint8_t* pdata)
{
    unsigned long L_addr;
    uint16_t lineIdx ;    
    lineIdx = gAdSampConfig.data_group_valid_line_idx[(g_bkpData.group_idx-1)];//ȡ���е���Ч��λ��
    L_addr = DATA_GROUP1_BASE_ADDR + (g_bkpData.group_idx-1)*DATA_GROUP_SIZE + (lineIdx)*SYS_AD_CH_MAX*2;
    write_flash(pdata, SYS_AD_CH_MAX*2, L_addr);
    g_bkpData.last_line_in_group++;
}

//ȡspi flash�е���ʷ���ݣ������ﲻУ�����Ƿ���Ч��
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
         gAdSampConfig.chsel[i] = 0xff;                         //Ĭ��ȫѡ
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

   SaveSetCfg();       //����Ĭ�����ã��Ա��´��ٽ���ʱ��ȡ           

}


