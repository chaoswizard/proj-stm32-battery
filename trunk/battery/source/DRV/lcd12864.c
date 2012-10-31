#include "stm32f10x.h"
#include <string.h>
#include "lcd12864.h"
#include "global.h"
#include "gui_menu_item.h"
#include "common.h"

//LCD_D/I           <-------->PB5
//LCD_R/W       <-------->PB6
//LCD_E             <--------->PB7
//LCD_CS1       <------->PB8
//LCD_CS2       <------->PB9
//LCD_RST       <------->PC11
//LCD_DB(0~7) <--------->PD(0~7)

#define LCD_PAGE_NUM                                           (8)
#define LCD_PAGE_HEIGHT_PIXEL_NUM            (8)
#define LCD_WIDTH_PIXEL_NUM                          (128)
#define LCD_HEIGHT_PIXEL_NUM                        (64)

#define Y_PIXLE_TO_ROW(pos_y)       ((pos_y) >> 3) 
#define X_PIXLE_TO_COL(pos_x)       ((pos_x) & 0x3F) 

#define LCD_POS_IS_INVALID(x, y) (((x) > (LCD_WIDTH_PIXEL_NUM - 1)) \
            || ((y) > (LCD_HEIGHT_PIXEL_NUM - 1)))

#define  XLCD_MOV_POS(x, y )  {\
    lcd12864_cmd(CMD_LCD_DISP_PAGE | y);\
    lcd12864_cmd( CMD_LCD_DISP_Y | x);\
}
#define DEV_LCD_SET_DATA(pos_x, pos_y, data)  {\
    XLCD_MOV_POS(pos_x, pos_y); \
    lcd12864_write_data(data);\
}

#define DEV_LCD_GET_DATA(pos_x, pos_y, data)  {\
    XLCD_MOV_POS(pos_x, pos_y);\
    lcd12864_read_data();\
    (data) = lcd12864_read_data();\
}

#define LCD_CS(pos_x) {\
    if ((pos_x)<(LCD_WIDTH_PIXEL_NUM>>1))\
    {\
             LCD_CS10;\
             LCD_CS21;\
    } else { \
            LCD_CS11;\
            LCD_CS20;\
    }\
}


#define LCD_E1       GPIOB->BSRR = GPIO_Pin_7 //PB7 
#define LCD_E0       GPIOB->BRR  = GPIO_Pin_7  

#define LCD_DI1      GPIOB->BSRR = GPIO_Pin_5 //PB5
#define LCD_DI0      GPIOB->BRR  = GPIO_Pin_5 
  
#define LCD_RW1      GPIOB->BSRR = GPIO_Pin_6 //PB6
#define LCD_RW0      GPIOB->BRR  = GPIO_Pin_6 

#define LCD_RST1     GPIOC->BSRR = GPIO_Pin_11 //PC11
#define LCD_RST0     GPIOC->BRR  = GPIO_Pin_11 

#define LCD_CS11     GPIOB->BSRR = GPIO_Pin_8 //PB8
#define LCD_CS10     GPIOB->BRR  = GPIO_Pin_8

#define LCD_CS21     GPIOB->BSRR = GPIO_Pin_9//PB9
#define LCD_CS20     GPIOB->BRR  = GPIO_Pin_9 

#define LCD_PORT             GPIOD
#define CMD_LCD_DISP_ON       0x3F 
#define CMD_LCD_DISP_OFF      0x3E 
#define CMD_LCD_DISP_ROW    0xC0 
#define CMD_LCD_DISP_PAGE   0xB8 
#define CMD_LCD_DISP_Y           0x40 


#define LCD_PORT_SPEED    GPIO_Speed_50MHz
#define LCD_PB_GROUP   (GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8 |GPIO_Pin_9)
#define LCD_PD_GROUP   (GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3 |GPIO_Pin_4|GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7)
#define LCD_PC_GROUP   (GPIO_Pin_11)

#define LCD_DATA_PORT_OUT_OD_50M        0x77777777
#define LCD_DATA_PORT_OUT_PP_50M        0x33333333
#define LCD_DATA_PORT_IN_FLOATING       0x44444444
#define LCD_DATA_PORT_IN_UD                     0xbbbbbbbb

#define LCD_DATA_PORT_OUT   LCD_DATA_PORT_OUT_PP_50M
#define LCD_DATA_PORT_IN    LCD_DATA_PORT_IN_FLOATING
#if 0
static void lcd_delay(unsigned long j)
{
	while(j--)
	{ 
            __NOP();
	}
}
#else
#define lcd_delay(x)
#endif

void  lcd12864_Configuration(void) 
{ 
        GPIO_InitTypeDef  GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = LCD_PD_GROUP;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        GPIO_InitStructure.GPIO_Speed = LCD_PORT_SPEED; 
        GPIO_Init(LCD_PORT, &GPIO_InitStructure);      // PORTD 为开漏输出 

        GPIO_InitStructure.GPIO_Pin = LCD_PB_GROUP; 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        GPIO_InitStructure.GPIO_Speed = LCD_PORT_SPEED; 
        GPIO_Init(GPIOB, &GPIO_InitStructure);  
        GPIO_InitStructure.GPIO_Pin = LCD_PC_GROUP; 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        GPIO_InitStructure.GPIO_Speed = LCD_PORT_SPEED; 
        GPIO_Init(GPIOC, &GPIO_InitStructure);  
        delay_ms(1);
} 

void  lcd12864_reset(void)
{
        LCD_E0;
        lcd_delay(10);
        LCD_RST0;
        lcd_delay(2*100);
        LCD_RST1;
        lcd_delay(2*100);
}

void  lcd12864_CS(u8 index) //0--select none 1--left 2--right 3--all
{
        switch(index)
        {
        case 0:
            LCD_CS11;
            LCD_CS21;
            break;
        case 1:
            LCD_CS10;
            LCD_CS21;
            break;
        case 2:
            LCD_CS11;
            LCD_CS20;
            break;
        case 3:
            LCD_CS10;
            LCD_CS20;
            break;
        }        
        lcd_delay(10);
}


void  lcd12864_Check_Busy(void) 
{  
        __IO unsigned char temp =0;
        
        //D/I="L",表示DB7~DB0为显示指令数据
        LCD_DI0;        // DI =0 指令 
        LCD_RW1;        // RW =1  读模式 
        LCD_PORT->CRL  = LCD_DATA_PORT_IN; //设为默认的浮动输入模式
        lcd_delay(1); 
        
        //当R/W=1,D/I=0;在E信号为"H"的作用下，状态分别输出到数据总线DB7~DB0的相应位
        do{ 
                LCD_E1;
                lcd_delay(1); 
                temp = (LCD_PORT->IDR & GPIO_Pin_7);
        }while(temp);//等待不忙 
        
        LCD_E0;   
        LCD_PORT->CRL  = LCD_DATA_PORT_OUT;  //把端口设置为默认输出
        lcd_delay(1);

} 



void  lcd12864_cmd(vu8 portValue) 
{  
        LCD_DI0;        // DI =0 指令 
        LCD_RW1;      // RW =1  读模式 
        LCD_PORT->CRL  = LCD_DATA_PORT_IN; //设为输入模式
        LCD_E1;
        while(LCD_PORT->IDR & GPIO_Pin_7);
        LCD_PORT->CRL  = LCD_DATA_PORT_OUT; 
        lcd_delay(1);
        
        ////////////////////
        LCD_RW0;        //WRITE
        LCD_DI0;          //ICODE  
        LCD_PORT->BRR |= 0x000000ff;
        LCD_PORT->BSRR |= portValue;
        lcd_delay(1);
        
        LCD_E1;
        lcd_delay(1); //RW = 0,DI = 0;E=H-->L,数据被写到IR或DR
        LCD_E0; 
        lcd_delay(1); 
}
void  lcd12864_write_data(vu8 portValue) 
{ 
        LCD_DI0;    // DI =0 指令 
        LCD_RW1;    // RW =1  读模式 
        LCD_PORT->CRL  = LCD_DATA_PORT_IN; 
        LCD_E1;
        while(LCD_PORT->IDR & GPIO_Pin_7);
        LCD_E0;   
        LCD_PORT->CRL  = LCD_DATA_PORT_OUT; 
        lcd_delay(1); 
        
        ///////////////////
        LCD_RW0;        //WRITE
        LCD_DI1;            //DATA

        LCD_PORT->BRR |= 0x000000ff;
        LCD_PORT->BSRR |= portValue;
        lcd_delay(1); 
        
        LCD_E1;         //R/W = "L", E信号下降沿锁存 DB7~DB0
        lcd_delay(1);  
        LCD_E0;             
        lcd_delay(1); 
}

vu8 lcd12864_read_data(void)//在写地址后,要进行一次空读,以后每读一次地址自动加1
{
    __IO unsigned char temp =0;
    LCD_DI0;       // DI =0 指令 
    LCD_RW1;    // RW =1  读模式 
    LCD_PORT->CRL  = LCD_DATA_PORT_IN; 
    LCD_E1;
    while(LCD_PORT->IDR & GPIO_Pin_7);
    
    /////////////////       
    LCD_RW1; //READ
    LCD_DI1;    //DATA
    lcd_delay(1); 
    LCD_E1;
    lcd_delay(1);
    temp= (unsigned char)LCD_PORT->IDR;
    LCD_E0; 
    
    return temp;
}

void  lcd12864_init() 
{   
        lcd12864_Configuration();
        lcd12864_reset();

        lcd12864_CS(3);//选左屏 右屏 
        lcd12864_cmd(CMD_LCD_DISP_PAGE);//0xb8
        lcd_delay(50); 
        lcd12864_cmd(CMD_LCD_DISP_Y);  //0x40
        lcd_delay(50);  
        lcd12864_cmd(CMD_LCD_DISP_ROW);//START LINE 0xc0
        lcd_delay(50); 
        lcd12864_cmd(CMD_LCD_DISP_ON); //0x3f
        lcd_delay(50);
} 

void  lcd12864_fill(u8  dat)
{
    unsigned char i,j;
    
    lcd12864_CS(3);
    for(j=0;j<LCD_PAGE_NUM;j++)
    {
        XLCD_MOV_POS(0, j);
        for(i=0;i<(LCD_WIDTH_PIXEL_NUM/2);i++)
        {
            lcd12864_write_data(dat);
        }
    }
    lcd12864_CS(0);
}

void  lcd12864_Clear(void)
{
    lcd12864_fill(0x00);
}

//画点程序
void lcd12864_putpixel(unsigned char x, unsigned char y, unsigned char pixel_mode)
{
        T_SCREEN_PIXEL tmp;

        if (pixel_mode == PIXEL_MODE_TRANS)
        {
            return;
        }

        if (LCD_POS_IS_INVALID(x, y))
        {
            return;
        }


        LCD_CS(x);
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


