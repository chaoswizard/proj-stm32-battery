#include "stm32f10x.h"
#include <string.h>
#include "lcd12864.h"
#include "ziku.h"
#include "global.h"
#include "gui_menu_item.h"

#define XLCD_CMD	0
#define XLCD_DAT	1

void WR_XLCD (unsigned char dat_comm,unsigned char content)
{
  if(dat_comm) GPIOB->BSRR	|= 0x00000020;	
  else GPIOB->BRR	|= 0x00000020;
	GPIOD->CRL  = 0x11111111;
	GPIOB->BRR |= 0x00000080;
	GPIOB->BRR	|= 0x00000040;
	GPIOD->BRR	|= 0x000000FF;
	GPIOD->BSRR	|= (unsigned int)content;	
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIOB->BSRR	|= 0x00000080;			//PB7,XLCD_E=1;
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIOB->BRR	|= 0x00000080;			//PB7,XLCD_E=0;
}

unsigned char RD_XLCD(void)
{
	unsigned char temp;
  GPIOB->BSRR	|= 0x00000020;
  GPIOB->BSRR	|= 0x00000040;
	GPIOD->CRL   = 0x88888888;
	GPIOD->ODR	|= 0x000000FF;
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIOB->BSRR	|= 0x00000080;	//PB7,XLCD_E=1;
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	temp=(unsigned char)(GPIOD->IDR);			//temp=PORTB;
	GPIOB->BRR	|= 0x00000080;
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIOD->CRL  = 0x11111111;
	return(temp);
}

void INIT_XLCD(void)
{
	int i;
	GPIOB->BRR	|= 0x00000200;	
  for(i=0;i<1000;i++)	;
 	GPIOB->BSRR	|= 0x00000200;	
	GPIOA->BRR |= 0x00000800;;	
	GPIOB->BRR |= 0x00000100;				
  WR_XLCD(XLCD_CMD,0x3F);
  WR_XLCD(XLCD_CMD,0xC0);
	GPIOA->BSRR |= 0x00000800;;			
	GPIOB->BSRR	|= 0x00000100;			
}

void XFILLRAM(unsigned char dat)
{
	unsigned char i,j;
	GPIOA->BRR |= 0x00000800;				
	GPIOB->BRR |= 0x00000100;				
 	for(j=0;j<8;j++)
  {
		WR_XLCD(XLCD_CMD,0xB8+j);
		WR_XLCD(XLCD_CMD,0x40);
		for(i=0;i<64;i++)
		{
		  	WR_XLCD(XLCD_DAT,dat);
		}
	}
	GPIOA->BSRR |= 0x00000800;				
	GPIOB->BSRR	|= 0x00000100;				
}


void printf_debug(unsigned char *tips, int val,unsigned char *tips2)
{
    struct SCREEN_ZONE zone;
    unsigned char  str[32];
    zone.x = 1;
    zone.y = 20;

    Screen_PrintClear(NULL);
    sprintf(str, "%s:%X %s", tips, val, tips2);
    Screen_PrintString(&zone, str, FONT_SIZE_NORMAL);
}

void printstr(unsigned char x,unsigned char y)
{
    struct SCREEN_ZONE zone;

    zone.x = x;
    zone.y = y<<3;
    
    Screen_PrintString(&zone, mystr, FONT_SIZE_NORMAL);
}

void displaycursor(unsigned char x,unsigned char y,unsigned char w,unsigned char h)
{
	register unsigned char i,j,tmp;
	if((x>127)|(y>7)) return;
	for(j=0;j<h;j++)
	{
		if(j+y>7)break;
		for(i=0;i<w;i++)
		{
			if(i+x>127)break;
			if(i+x<64)
			{
				GPIOA->BRR |= 0x00000800;			//PA11,XLCD_CS1=LCD_SELECT;
				GPIOB->BSRR|= 0x00000100;			//PB8,XLCD_CS2=LCD_SELECT;
			}
			else 
			{
				GPIOA->BSRR|= 0x00000800;			//PA11,XLCD_CS1=LCD_SELECT;
				GPIOB->BRR |= 0x00000100;			//PB8,XLCD_CS2=LCD_SELECT;
			}
			WR_XLCD(XLCD_CMD,0xB8+j+y);
			WR_XLCD(XLCD_CMD,0x40+((i+x)&0x3F));
			tmp=RD_XLCD();
			tmp=RD_XLCD();
			WR_XLCD(XLCD_CMD,0xB8+j+y);
			WR_XLCD(XLCD_CMD,0x40+((i+x)&0x3F));
			WR_XLCD(XLCD_DAT,tmp^0xFF);
		}
	}
}

void putpixel(unsigned char x,unsigned char y,unsigned char pixel_mode)
#if 0
{
    Screen_PrintPixel(x, y, pixel_mode);
}
#else
{
	unsigned char tmp;
	if((x>127)||(y>63))return;
	if(x<64)
	{
		GPIOA->BRR |= 0x00000800;			//PA11,XLCD_CS1=LCD_SELECT;
		GPIOB->BSRR|= 0x00000100;			//PB8,XLCD_CS2=LCD_SELECT;
	}
	else 
	{
		GPIOA->BSRR|= 0x00000800;			//PA11,XLCD_CS1=LCD_SELECT;
		GPIOB->BRR |= 0x00000100;			//PB8,XLCD_CS2=LCD_SELECT;
	}
	WR_XLCD(XLCD_CMD,0xB8+(y>>3));
	WR_XLCD(XLCD_CMD,0x40+(x&0x3F));
	tmp=RD_XLCD();
	tmp=RD_XLCD();
	WR_XLCD(XLCD_CMD,0xB8+(y>>3));
	WR_XLCD(XLCD_CMD,0x40+(x&0x3F));
	if(pixel_mode==1)					WR_XLCD(XLCD_DAT,tmp|(1<<((y&0x07))));
	else if(pixel_mode==0)		WR_XLCD(XLCD_DAT,tmp&(~(1<<((y&0x7)))));
	else if(pixel_mode==2)		WR_XLCD(XLCD_DAT,tmp^(1<<((y&0x7))));
}

#endif
void line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char pixel_mode)
{
    Screen_PrintLine(x1, y1, x2, y2, pixel_mode);
}

void printSmall(unsigned char x,unsigned char y)
#if 0
{
    struct SCREEN_ZONE zone;

    zone.x = x;
    zone.y = y;
    
    Screen_PrintString(&zone, mystr, FONT_SIZE_NORMAL);
}
#else
{	//x=[0,15],y=[0,64]£¬Í¼ÐÎ·½Ê½
	unsigned char temp,i,j,k,qq,len;
    struct UICOM_1PP_BMP_INFO  fontInfo;

    
	if((x>122)||(y>57)) return;

	len=strlen(mystr);
 	for(j=0;j<len;j++)
	{	
        temp = uicom_font_getdata((u_int8 *)mystr + j, &fontInfo, FONT_SIZE_SMALL);
        if (0 == temp)
        {
            break;
        }
        
		for(i=0;i<6;i++)
		{			
			qq = fontInfo.data[i];
			for(k=0;k<8;k++)
			{
				if((qq>>k)&0x01) putpixel(j*6+x+i,y+k,1);
				else putpixel(j*6+x+i,y+k,0);
			}
		}
	}
}

#endif
