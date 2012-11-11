#include "stm32f10x.h"
#include "at45db041.h"
#include "common.h"

#define PAGESIZE	264			//528

#define SPI_041_SI_H   SPI_FLASH_PORT->BSRR |= SPI_041_SI
#define SPI_041_SO_H    SPI_FLASH_PORT->BSRR |= SPI_041_SO
#define SPI_041_SCK_H   SPI_FLASH_PORT->BSRR |= SPI_041_SCK
#define SPI_041_CS_H    SPI_FLASH_PORT->BSRR |= SPI_041_CS
#define SPI_041_RST_H SPI_FLASH_RST_PORT->BSRR |= SPI_041_RST


#define SPI_041_SI_L   SPI_FLASH_PORT->BRR |= SPI_041_SI
#define SPI_041_SO_L    SPI_FLASH_PORT->BRR |= SPI_041_SO
#define SPI_041_SCK_L  SPI_FLASH_PORT->BRR |= SPI_041_SCK
#define SPI_041_CS_L    SPI_FLASH_PORT->BRR |= SPI_041_CS
#define SPI_041_RST_L  SPI_FLASH_RST_PORT->BRR |= SPI_041_RST

void spi_delay(uint16_t cnt)
{
    while(cnt--)
    {
        delay_us(20);
    }
}

#define  SPI_DELAY(x)     spi_delay(x)
void SpiFlashPortInit(void)
{
    	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  SPI_041_SI|SPI_041_SCK|SPI_041_CS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(SPI_FLASH_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  SPI_041_SO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(SPI_FLASH_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  SPI_041_RST;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(SPI_FLASH_RST_PORT, &GPIO_InitStructure);
	
}

void spiFlashInit(void)
{
    SpiFlashPortInit();
    RST_DB041();
}

void RST_DB041(void)
{
	int j;
	SPI_041_RST_L;  //GPIOB->BRR |= 0x00000001;		        //PB0,	RST=0;	
	SPI_041_SCK_H;  //GPIOD->BSRR|= 0x00002000;		//PD13,	SCK=1;
	SPI_041_CS_H;   //GPIOD->BSRR|= 0x00001000;		//PD12,	CS=1;
	SPI_041_SI_H;   //GPIOD->BSRR|= 0x00008000;		        //PD15,	SI=1
	delay_us(1);
	SPI_041_RST_H;//GPIOB->BSRR|= 0x00000001;		//PB0,	RST=0;	
}

unsigned char writeByteSPI(unsigned char tt)
{
    unsigned char i,tmp;
    for(i=8;i!=0;i--)
    {
        GPIOD->BRR |= 0x00002000;							//PD13,SCK2=0;
        if(tt&0x80)
        {
            GPIOD->BSRR |= 0x00008000;	                            //PD15,SI=1;
        }
        else 
        {
            GPIOD->BRR |= 0x00008000;				//PD15,SI=0;
         }
        GPIOD->BSRR|= 0x00002000;							//PD13,SCK2=1;
        tmp=(tmp<<1);
        if(GPIOD->IDR & 0x4000)	
        {
            tmp++;				//PD14,SO
         }
        tt=tt<<1;
    }
    return(tmp);
}
void ReadMemory(unsigned char *pInData,unsigned int mm, unsigned int startPage, unsigned int startByte) 
{
	unsigned char i;
	GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0xE8);	                        //主存页读(D2,52),连续读(68,E8)
	writeByteSPI((startPage&0x7FF)>>7);
	writeByteSPI(((startPage&0x7F)<<1)+((startByte&0x1FF)>>8));
	writeByteSPI(startByte&0xFF);
	for(i=0;i<4;i++)
	{
	    writeByteSPI(0xFF);
	}
	for(;mm>0;mm--)	
	{
	    *(pInData++)=writeByteSPI(0xFF);
	}
	GPIOD->BSRR |= 0x00001000;	            //PD12,CS=1
}

unsigned char ReadStatusFromDataFlash(void)
{
	unsigned char i;
	GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0xD7);
	i = writeByteSPI(0xFF);
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

	SPI_DELAY(1);
        GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0x84);
	writeByteSPI(0x00);
	writeByteSPI(tstart>>8);
	writeByteSPI(tstart&0xFF);
	for(;tlen;tlen--)	
	{
	    writeByteSPI(*wbuf++);
	}
	GPIOD->BSRR |= 0x00001000;	//PD12,CS=1

	SPI_DELAY(1);
	GPIOD->BRR |= 0x00001000;	//PD12,CS=0
	writeByteSPI(0x83);
	writeByteSPI(tpage>>7);
	writeByteSPI(tpage<<1);
	writeByteSPI(0x00);
	GPIOD->BSRR |= 0x00001000;	//PD12,CS=1

	while(1)
	{
		SPI_DELAY(1);	
		tmp=ReadStatusFromDataFlash();
		if(tmp&0x80) break;
	}
}

void write_flash(unsigned char *wbuf,unsigned int tlen,unsigned long taddr)
{
	unsigned int k,tpage,tbyte,tmp;
	tpage=taddr/PAGESIZE;	
	tbyte=taddr%PAGESIZE;
	if((tlen+tbyte)<PAGESIZE) 
	{	
        	write_Page(wbuf,tlen,tpage,tbyte);
        	return;
	}
	k=PAGESIZE-tbyte; 
	write_Page(wbuf,k,tpage,tbyte);
	tlen-=k;	
	tpage++;	
	wbuf+=k;	
	tbyte=0;
	while(tlen>0)
	{
		if((tlen+tbyte)<PAGESIZE) 
		{	
        		write_Page(wbuf,tlen,tpage,0);
        		return;
		}
		k=PAGESIZE; 
		write_Page(wbuf,k,tpage,tbyte);
		tlen-=k;	
		tpage++;	
		tbyte=0;	
		wbuf+=k;		
	}
	/*
        while(1)   //等待flash空闲
	{
		SPI_DELAY(1);	
		tmp=ReadStatusFromDataFlash();  //似乎这个不起作用。那如果马上要读，就在写后加延时
		if(tmp&0x80) break;
	}
	*/
}

void read_flash(unsigned char *rbuf,unsigned int I_length,unsigned long L_addr)
{
	unsigned int I_start_page,I_start_byte;
	I_start_page=L_addr/PAGESIZE;
	I_start_byte=L_addr%PAGESIZE;
	ReadMemory(rbuf,I_length,I_start_page,I_start_byte); 
}


void spi_RW_test(void)
{
    unsigned char *tstr = "huangxiao";
  
    unsigned char strArray[20]; 
    write_flash(tstr,15,0);
    xprintf("writed\n");
    read_flash(strArray,15,0);
    xprintf("%s\n",strArray);

    write_flash(tstr,15,55);
    xprintf("writexxxxd\n");
    read_flash(strArray,15,55);
    xprintf("%s\n",strArray);
}




