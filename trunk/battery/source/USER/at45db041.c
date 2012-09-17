#include "stm32f10x.h"
#include "at45db041.h"

#define PAGESIZE	264			//528

void RST_DB041(void)
{
	int j;
	GPIOB->BRR |= 0x00000001;		//PB0,	RST=0;	
	GPIOD->BSRR|= 0x00002000;		//PD13,	SCK=1;
	GPIOD->BSRR|= 0x00001000;		//PD12,	CS=1;
	GPIOD->BSRR|= 0x00008000;		//PD15,	SI=1
	for(j=0;j<200;j++);
	GPIOB->BSRR|= 0x00000001;		//PB0,	RST=0;	
}

unsigned char writeByteSPI(unsigned char tt)
{
	unsigned char i,tmp;
  for(i=8;i!=0;i--)
	{
		GPIOD->BRR |= 0x00002000;							//PD13,SCK2=0;
    if(tt&0x80)GPIOD->BSRR |= 0x00008000;	//PD15,SI=1;
    else GPIOD->BRR |= 0x00008000;				//PD15,SI=0;
    GPIOD->BSRR|= 0x00002000;							//PD13,SCK2=1;
		tmp=(tmp<<1);
		if(GPIOD->IDR & 0x4000)	tmp++;				//PD14,SO
    tt=tt<<1;
  }
	return(tmp);
}
void ReadMemory(unsigned char *pInData,unsigned int mm, unsigned int startPage, unsigned int startByte) 
{
	unsigned char i;
	GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0xE8);	//Ö÷´æÒ³¶Á(D2,52),Á¬Ðø¶Á(68,E8)
	writeByteSPI((startPage&0x7FF)>>7);
	writeByteSPI(((startPage&0x7F)<<1)+((startByte&0x1FF)>>8));
	writeByteSPI(startByte&0xFF);
	for(i=0;i<4;i++)writeByteSPI(0xFF);
	for(;mm>0;mm--)	*(pInData++)=writeByteSPI(0xFF);
	GPIOD->BSRR |= 0x00001000;	//PD12,CS=1
}

unsigned char ReadStatusFromDataFlash(void)
{
	unsigned char i;
	GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0xD7);
	i=writeByteSPI(0xFF);
	GPIOD->BSRR |= 0x00001000;	//PD12,CS=1
	return(i);
}

void write_Page(unsigned char *wbuf,unsigned int tlen,unsigned int tpage,unsigned int tstart)
{
	unsigned char tmp;
	unsigned int i;
  GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0x53);
	writeByteSPI(tpage>>7);
	writeByteSPI(tpage<<1);
	writeByteSPI(0x00);
  GPIOD->BSRR |= 0x00001000;	//PD12,CS=1

	for(i=0;i<20000;i++);	//delay_ms(1);
  GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0x84);
	writeByteSPI(0x00);
	writeByteSPI(tstart>>8);
	writeByteSPI(tstart&0xFF);
	for(;tlen;tlen--)	writeByteSPI(*wbuf++);
	GPIOD->BSRR |= 0x00001000;	//PD12,CS=1

	for(i=0;i<20000;i++);	//delay_ms(1);
	GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0x83);
	writeByteSPI(tpage>>7);
	writeByteSPI(tpage<<1);
	writeByteSPI(0x00);
	GPIOD->BSRR |= 0x00001000;	//PD12,CS=1

	while(1)
	{
		for(i=0;i<20000;i++);	//delay_ms(1);
		tmp=ReadStatusFromDataFlash();
		if(tmp&0x80) break;
	}
}

void write_flash(unsigned char *wbuf,unsigned int tlen,unsigned long taddr)
{
	unsigned int k,tpage,tbyte;
	tpage=taddr/PAGESIZE;	tbyte=taddr%PAGESIZE;
	if((tlen+tbyte)<PAGESIZE) {	write_Page(wbuf,tlen,tpage,tbyte);return;}
	k=PAGESIZE-tbyte; write_Page(wbuf,k,tpage,tbyte);
	tlen-=k;	tpage++;	wbuf+=k;	tbyte=0;
	while(tlen>0)
	{
		if((tlen+tbyte)<PAGESIZE) {	write_Page(wbuf,tlen,tpage,0);return;}
		k=PAGESIZE; write_Page(wbuf,k,tpage,tbyte);
		tlen-=k;	tpage++;	tbyte=0;	wbuf+=k;		
	}
}

void read_flash(unsigned char *rbuf,unsigned int I_length,unsigned long L_addr)
{
	unsigned int I_start_page,I_start_byte;
	I_start_page=L_addr/PAGESIZE;
	I_start_byte=L_addr%PAGESIZE;
	ReadMemory(rbuf,I_length,I_start_page,I_start_byte); 
}
