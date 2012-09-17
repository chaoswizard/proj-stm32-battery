#include "stm32f10x.h"
#include "input_output.h"
#include "global.h"

void Write595(uint8_t qdata)
{
	uint8_t tmp,i;
	tmp=qdata;	GPIOC->BRR |= 0x00000200;			//PC9,LAT_595 = 0;
	for (i=0;i<8;i++)
	{
		GPIOC->BRR |= 0x00000100;		//PC8,CLK_595 = 0;
		__NOP();
		if(tmp&0x01) GPIOC->BRR |= 0x00000080;	//PC7,DAT_595=0;
		else GPIOC->BSRR |= 0x00000080;					//PC7,DAT_595=1;
		__NOP();	tmp=tmp>>1;
		GPIOC->BSRR |= 0x00000100;	//PC8,CLK_595 = 1;
		__NOP();
	}
	GPIOC->BSRR |= 0x00000200;	//PC9,LAT_595 = 1;
}
void flashIO(void)
{
	uint8_t i;
	GPIOD->BRR |= 0x0800;				//PD11=锁存,1=输入,0=保持
	GPIOD->BSRR|= 0x0200;				//PD9=Enable Output,0=高阻
	for(i=0;i<13;i++)	Write4094E(OutputBuf[12-i]);	//清继电器输出	
	GPIOD->BSRR  |= 0x0800;			//PD11=锁存,,1=输入,0=保持	
	GPIOD->BRR|= 0x0200;				//PD9=Enable Output,0=高阻
}
void Write4094E(unsigned char dat)
{
	uint8_t tmp,i;
	tmp=dat;	
	for (i=0;i<8;i++)
	{	//Tclk>0.8us, Pw>0.2us
		GPIOD->BSRR |= 0x0100;		//PD8=时钟,上升沿输入
		if(tmp&0x80) 	GPIOD->BRR |= 0x0400;	//PD10=数据
		else 	GPIOD->BSRR  |= 0x0400;				//PD10=数据
		__NOP();	__NOP();	__NOP();	__NOP();	tmp=tmp<<1;
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		GPIOD->BRR |= 0x0100;			//PD8=时钟,上升沿输入
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	}
}

unsigned long input_165(void) 
{	
	uint8_t i;
	uint32_t tmp=0;
	GPIOA->BRR  |= 0x00000100;					//A8,P_CP=0;
	GPIOC->BRR  |= 0x00001000;					//PC12,P_SL=0;
	__NOP();
	GPIOC->BSRR  |= 0x00001000;					//PC12,P_SL=1;
	for(i=0;i<24;i++)
	{
		if(GPIOA->IDR & 0x0200)	
		{
            tmp = (tmp<<1);
        }
		else 
		{
            tmp = (tmp<<1)+1; 
        }
        //A9,DAT
		GPIOA->BRR  |= 0x00000100;				//A8,P_CP=0;
		__NOP();	__NOP();
		GPIOA->BSRR |= 0x00000100;				//A8,P_CP=1;
	}
	return(tmp);
}
void changeChannel(void)
{
	GPIOC->BRR 	|= 0x0000000F;							//A,B,C,D=0 选中第一通道
	GPIOC->BSRR |= (uint32_t)(scanCh&0x0F);	//A,B,C,D=0 选中第一通道
}
/* 冒泡排序法 */
void Bublesort(uint16_t a[],int n)
{
     int i,j,k;
     for(j=0;j<n;j++)   /* 气泡法要排序n次*/
     {
          for(i=0;i<n-j;i++)  /* 值比较大的元素沉下去后，只把剩下的元素中的最大值再沉下去就可以啦 */
          {
               if(a[i]>a[i+1])  /* 把值比较大的元素沉到底 */
               {
                    k=a[i];
                    a[i]=a[i+1];
                    a[i+1]=k;
               }
          }
     }
}
