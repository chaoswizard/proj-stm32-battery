#include "stm32f10x.h"
#include <string.h>
#include "lcd12864.h"
#include "ziku.h"
#include "global.h"
#include "gui_menu_item.h"

#define LCD_ROW_NUM                         (8)
#define LCD_ROW_HEIGHT_PIXEL_NUM            (8)
#define LCD_WIDTH_PIXEL_NUM                 (128)
#define LCD_HEIGHT_PIXEL_NUM                (64)
    

#define Y_PIXLE_TO_ROW(pos_y)       ((pos_y) >> 3) 
#define X_PIXLE_TO_COL(pos_x)       ((pos_x) & 0x3F) 

#define LCD_POS_IS_INVALID(x, y) (((x) > (LCD_WIDTH_PIXEL_NUM - 1)) || ((y) > (LCD_HEIGHT_PIXEL_NUM - 1)))

#define XLCD_DAT	1

#define DEV_LCD_CS_A() {\
    GPIOA->BRR |= 0x00000800;\
    GPIOB->BSRR|= 0x00000100;\
}

#define DEV_LCD_CS_B() {\
    GPIOA->BSRR |= 0x00000800;\
    GPIOB->BRR |= 0x00000100;\
}

#define DEV_LCD_START_FILL() {\
    GPIOA->BRR |= 0x00000800; \
    GPIOB->BRR |= 0x00000100;\
}

#define DEV_LCD_END_FILL() {\
    GPIOA->BSRR |= 0x00000800; \
    GPIOB->BSRR |= 0x00000100;\
}

#define DEV_LCD_MOVE_POS(pos_x, pos_y)  XLCD_MOV_POS(pos_x, pos_y)
#define DEV_LCD_WRITE(data)  XLCD_SEND_DATA((data))
#define DEV_LCD_READ(data)   (data) = XLCD_RECV_DATA()


#define DEV_LCD_CS(pos_x) {\
    if((pos_x)<(LCD_WIDTH_PIXEL_NUM/2))\
    {\
        DEV_LCD_CS_A();\
    } else { \
       DEV_LCD_CS_B();\
    }\
}


#define DEV_LCD_SET_DATA(pos_x, pos_y, data)  {\
    DEV_LCD_MOVE_POS(pos_x, pos_y); \
    DEV_LCD_WRITE(data);\
}
#define DEV_LCD_GET_DATA(pos_x, pos_y, data)  {\
    DEV_LCD_MOVE_POS(pos_x, pos_y);\
    DEV_LCD_READ(data); \
}



void WR_XLCD (unsigned char dat_comm,unsigned char content)
{
    // W/R = 0.   
    
    if(dat_comm) //  D/I = 0
    {
        GPIOB->BSRR |= 0x00000020;//LCD_RST  ÖÃ1
    }
    else  // D/I = 1
    {
        GPIOB->BRR  |= 0x00000020;//
    }
    
	GPIOD->CRL   = 0x11111111;
	GPIOB->BRR  |= 0x00000080;
	GPIOB->BRR	|= 0x00000040;
	GPIOD->BRR	|= 0x000000FF;
	GPIOD->BSRR	|= (unsigned int)content;	
    delay_us(10);
	GPIOB->BSRR	|= 0x00000080;			//PB7,XLCD_E=1;
    delay_us(10);
	GPIOB->BRR	|= 0x00000080;			//PB7,XLCD_E=0;
}

void XLCD_WAIT(void)
{
    #if 0
    GPIOB->BSRR |= 0x00000020;
	GPIOD->CRL   = 0x11111111;
	GPIOB->BRR  |= 0x00000080;
	GPIOB->BRR	|= 0x00000040;
	GPIOD->BRR	|= 0x000000FF;
	GPIOB->BSRR	|= 0x00000080;			//PB7,XLCD_E=1;
	while(GPIOD->IDR & (0x80)){
        delay_us(5);
    }
	GPIOB->BRR	|= 0x00000080;	
    #endif//PB7,XLCD_E=0;
}


unsigned char XLCD_RECV_DATA(void)
{
	unsigned char temp, i;
    
    i = 2;
    while (i--)
    {
        GPIOB->BSRR |= 0x00000020;
        GPIOB->BSRR |= 0x00000040;
        GPIOD->CRL   = 0x88888888;
        GPIOD->ODR  |= 0x000000FF;
        GPIOB->BSRR |= 0x00000080;  //PB7,XLCD_E=1;
        delay_us(10);
        temp=(unsigned char)(GPIOD->IDR);           //temp=PORTB;
        GPIOB->BRR  |= 0x00000080;
        delay_us(6);
        GPIOD->CRL  = 0x11111111;
    } 
	return (temp);
}

void INIT_XLCD(void)
{
	GPIOB->BRR	|= 0x00000200;	
    
    delay_us(1000);

 	GPIOB->BSRR	|= 0x00000200;	
	GPIOA->BRR |= 0x00000800;;	
	GPIOB->BRR |= 0x00000100;				
    XLCD_SEND_CMD(0x3F);
    XLCD_SEND_CMD(0xC0);
	GPIOA->BSRR |= 0x00000800;;			
	GPIOB->BSRR	|= 0x00000100;			
}


void XLCD_SEND_CMD(unsigned char cmd)
{
    WR_XLCD(0, cmd);
}

void XLCD_SEND_DATA(unsigned char data)
{
    WR_XLCD(1, data);
}


void XLCD_MOV_POS(unsigned char x,unsigned char y)
{
    XLCD_SEND_CMD(0xB8 | y);
    XLCD_SEND_CMD(0x40 | x);
}

void XFILLRAM(unsigned char dat)
{
	unsigned char i,j;
#if 0
	GPIOA->BRR |= 0x00000800;				
	GPIOB->BRR |= 0x00000100;				
    for(j=0;j<8;j++)
    {
        XLCD_MOV_POS(0, j);
        for(i=0;i<64;i++)
        {
          	XLCD_SEND_DATA(dat);
        }
    }
	GPIOA->BSRR |= 0x00000800;				
	GPIOB->BSRR	|= 0x00000100;	
#else
    DEV_LCD_START_FILL(); 
    for(j=0;j<LCD_ROW_NUM;j++)
    {
        DEV_LCD_MOVE_POS(0, j);
        for(i=0;i<(LCD_WIDTH_PIXEL_NUM/2);i++)
        {
            DEV_LCD_WRITE(dat);
        }
    }
    DEV_LCD_END_FILL();
#endif
}


void delay_us(unsigned int us)
{
    u_int32 i;
    for (i=0; i<us; i++)
    {
        __NOP();
    }
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
			XLCD_MOV_POS((i+x)& 0x3F, j+y);
			tmp=XLCD_RECV_DATA();
			XLCD_MOV_POS(((i+x)&0x3F), j+y);
			XLCD_SEND_DATA(tmp^0xFF);
		}
	}
}

void putpixel(unsigned char x, unsigned char y, unsigned char pixel_mode)
{
	T_SCREEN_PIXEL tmp;

    if (LCD_POS_IS_INVALID(x, y))
    {
        return;
    }

    DEV_LCD_CS(x);
    DEV_LCD_GET_DATA(X_PIXLE_TO_COL(x), Y_PIXLE_TO_ROW(y), tmp);
    if (pixel_mode == PIXEL_MODE_SET)
    {
        tmp |= (1<<((y & 0x07)));
    }
    else if (pixel_mode == PIXEL_MODE_CLEAR)    
    {
        tmp &= (~(1<<((y & 0x7))));
    }
    else if (pixel_mode == PIXEL_MODE_TURN) 
    {
        tmp ^= (1<<((y & 0x7)));
    }
    else
    {
        return;
    }
    DEV_LCD_SET_DATA(X_PIXLE_TO_COL(x), Y_PIXLE_TO_ROW(y), tmp);
}


void XLCD_SET_BYTE(T_SCREEN_PIXEL col, T_SCREEN_PIXEL row, u_int8 data)
{
    DEV_LCD_CS(col);
    DEV_LCD_SET_DATA(X_PIXLE_TO_COL(col), Y_PIXLE_TO_ROW(row) , data);
}

void printSmall(unsigned char x,unsigned char y)
{
    struct SCREEN_ZONE zone;

    zone.x = x;
    zone.y = y;
    
    Screen_PrintString(&zone, mystr, FONT_SIZE_SMALL);
}
