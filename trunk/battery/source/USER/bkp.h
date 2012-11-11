#ifndef __BKP_H
#define __BKP_H
#include "stm32f10x.h"
#include "global.h"


void  bkp_write(struct tag_BAK_DATA *data,uint16_t table[]);
void  bkp_read(struct tag_BAK_DATA *data,uint16_t table[]);
void bkpSysDefBakInit(void);

#endif//__BKP_H



