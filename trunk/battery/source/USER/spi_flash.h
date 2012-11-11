#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#define SYS_SET_CFG_ADDRESS  0
///////////spi flash���
#define AD_DATA_GROUP_NUM_MIN  1
#define AD_DATA_GROUP_NUM_MAX  4
#define DATA_GROUP_SIZE   120*1024
#define DATA_GROUP1_BASE_ADDR  12*1024
#define DATA_GROUP2_BASE_ADDR  (DATA_GROUP1_BASE_ADDR+DATA_GROUP_SIZE)
#define DATA_GROUP3_BASE_ADDR  (DATA_GROUP2_BASE_ADDR+DATA_GROUP_SIZE)
#define DATA_GROUP4_BASE_ADDR  (DATA_GROUP3_BASE_ADDR+DATA_GROUP_SIZE)

void SaveSetCfg(void);
uint8_t GetSetCfg(void); //�����Ƿ�ȡ����ȷ������

void SaveAdData(uint8_t* pdata);  //�Զ���ȫ�ֱ����ı仯�������ݣ������Ÿ���һ�±���������
void GetAdData(uint8_t groupIdx,uint8_t lineIdx,uint16_t* buf);
void MoveToNextGroup(void);

#endif //SPI_FLASH_H


