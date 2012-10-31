
#include "common.h"

//when PCLK = 72M
//ÑÓÊ± n(us)
void delay_us(unsigned long n)
{
	unsigned long j;
	while(n--)
	{ 
	    j=108;               // 8
	    while(j--);
	}
}


//ÑÓÊ±n ms
void delay_ms(unsigned long n)
{
	while(n--)
	{
	   delay_us(1000);
	}
}

unsigned short calc_checkcode(unsigned char *buf,  int nsize)
{
    unsigned short checkcode = 0;
    unsigned short *pData = (unsigned short *)buf;

    nsize >>= 1;
    while(nsize)
    {
        nsize--;
        checkcode ^= buf[nsize];
    }
    
    return checkcode;
}


