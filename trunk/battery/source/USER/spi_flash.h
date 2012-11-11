#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#define SYS_SET_CFG_ADDRESS  0
///////////spi flash相关
#define AD_DATA_GROUP_NUM_MIN  1
#define AD_DATA_GROUP_NUM_MAX  4
#define DATA_GROUP_SIZE   120*1024
#define DATA_GROUP1_BASE_ADDR  12*1024
#define DATA_GROUP2_BASE_ADDR  (DATA_GROUP1_BASE_ADDR+DATA_GROUP_SIZE)
#define DATA_GROUP3_BASE_ADDR  (DATA_GROUP2_BASE_ADDR+DATA_GROUP_SIZE)
#define DATA_GROUP4_BASE_ADDR  (DATA_GROUP3_BASE_ADDR+DATA_GROUP_SIZE)

void SaveSetCfg(void);
uint8_t GetSetCfg(void); //返回是否取得正确的配置

void SaveAdData(uint8_t* pdata);  //自动按全局变量的变化保存数据，紧跟着更新一下备份数据区
void GetAdData(uint8_t groupIdx,uint8_t lineIdx,uint16_t* buf);
void MoveToNextGroup(void);

#endif //SPI_FLASH_H


