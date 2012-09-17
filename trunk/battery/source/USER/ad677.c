#include "stm32f10x.h"
#include <string.h>
#include "ad677.h"
#include "global.h"

void Calibration(void)
{
//	GPIOE->CRL =0x01414411;		//PE0=CLK,PE1=Sample,PE2=SDATA,PE3=SCLK,PE4=CAL,PE5=BUSY
//	GPIOE->CRH =0x00000000;		//PE6=
	uint32_t i,j;
	while(1)
	{
		GPIOE->BRR|=0x00000001;			//PE0=CLK,=0
		GPIOE->BRR|=0x00000002;			//PE1=SAM,=0	
		GPIOE->BSRR|=0x00000010;		//PE4=CAL,=1
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcalh>0.05,取0.25
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcalh>0.05,取0.25
		GPIOE->BRR|=0x00000010;		//PE4=CAL,=0	
//		while((GPIOE->IDR & 0x0020)==0);	//等待BUSY=1
		cal32=0;
		while(1)//for(i=0;i<100000;i++)		//85532
		{
			cal32++;
			GPIOE->BSRR|=0x00000001;		//PE0=CLK,=1		Tc=0.5(2MHz)
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP(); 	//Tch>0.05,=0.25
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP(); 	//Tch>0.05,=0.25
			GPIOE->BRR|=0x00000001;		//PE0=CLK,=0
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcl>0.05,=0.25
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcl>0.05,=0.25
			if((GPIOE->IDR & 0x0020)==0)	//等待BUSY=0
				break;		//PE5=BUSY
		}
		for(j=0;j<1000;j++)
		{
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcalh>0.05,取0.25
		}
	}
}

uint16_t Sample(void)
{	//最高为100KHz,
	uint32_t i,j;
	uint16_t addd;
	while(1)
	{
		GPIOE->BRR|=0x00000001;			//PE0=CLK,=0
		GPIOE->BRR|=0x00000010;			//PE4=CAL,=0	
		GPIOE->BSRR|=0x00000002;		//PE1=SAM,=1
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcalh>0.05,取0.25
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcalh>0.05,取0.25
		GPIOE->BRR|=0x00000002;		//PE1=CAL,=0	
		while((GPIOE->IDR & 0x0020)==0);	//等待BUSY=1
		cal32=0;	addd=0;
		while(1)//for(i=0;i<100000;i++)		//85532
		{
			cal32++;	
			GPIOE->BSRR|=0x00000001;		//PE0=CLK,=1		Tc=0.5(2MHz)
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP(); 	//Tch>0.05,=0.25
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP(); 	//Tch>0.05,=0.25
			GPIOE->BRR|=0x00000001;		//PE0=CLK,=0
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcl>0.05,=0.25
			__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcl>0.05,=0.25

			if((GPIOE->IDR & 0x0008)==0)	//PE3=SCLK
			{
				addd>>=1;
				if((GPIOE->IDR & 0x0004)==4)	//PE2=SDAT
					addd|=0x8000;
			}
			if((GPIOE->IDR & 0x0020)==0)	//等待BUSY=0
				break;		//PE5=BUSY
		}
		for(j=0;j<10;j++)
		{
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	//Tcalh>0.05,取0.25
		}
	}
	return(addd);
}
