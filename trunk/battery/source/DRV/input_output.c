#include "stm32f10x.h"
#include "input_output.h"
#include "global.h"

//LED 控制
#define LED_CTRL_RCK_H      LED_CTRL_PORT->BSRR |= LED_CTRL_RCK
#define LED_CTRL_SCK_H      LED_CTRL_PORT->BSRR |= LED_CTRL_SCK
#define LED_CTRL_SER_H      LED_CTRL_PORT->BSRR |= LED_CTRL_SER

#define LED_CTRL_RCK_L     LED_CTRL_PORT->BRR |= LED_CTRL_RCK
#define LED_CTRL_SCK_L      LED_CTRL_PORT->BRR |= LED_CTRL_SCK
#define LED_CTRL_SER_L      LED_CTRL_PORT->BRR |= LED_CTRL_SER

//LED显示,这是其中由595控制的部分，其只作为模块内函数
static void Write595_LED(uint8_t data)
{
	uint8_t tmp,i;
	tmp = data;

	LED_CTRL_RCK_L;
	for (i=0;i<8;i++)
	{        
		LED_CTRL_SCK_L;
		if(tmp&0x01)
		{
		    LED_CTRL_SER_L;  //led灯驱动反向了
		}
		else 
		{
		    LED_CTRL_SER_H;
		}
		delay_us(1);
		tmp=tmp>>1;
		LED_CTRL_SCK_H;//GPIOC->BSRR |= 0x00000100;	//PC8,CLK_595 = 1;
		delay_us(1);
	}
	LED_CTRL_RCK_H;//GPIOC->BSRR |= 0x00000200;	//PC9,LAT_595 = 1;

}
void LedCtrlPortInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  LED_CTRL_RCK |LED_CTRL_SCK|LED_CTRL_SER;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(LED_CTRL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  LED_RUN_CTRL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(LED_RUN_CTRL_PORT, &GPIO_InitStructure);

	//打开RUN_LED
	LED_RUN_CTRL_PORT->BSRR |= LED_RUN_CTRL_PIN;
}

void LedCtrlInit(void)
{
    int i;
    LedCtrlPortInit();
    g_bkpData.sysResAndStatus.led_buf = 0;   

}


/*******************
*data:LED_START/LED_IMPEND/LED_REVERSED/LED_WORKING/LED_NO_U_PAN/LED_WRITE_PROTECTED...
*newState: LED_ON/LED_OFF
*******************/
void LedCtrl(uint8_t data,uint8_t newState)
{
    if(newState)
    {
        g_bkpData.sysResAndStatus.led_buf |= data;
    }
    else
    {
        g_bkpData.sysResAndStatus.led_buf &= ~data;
    }
    Write595_LED(g_bkpData.sysResAndStatus.led_buf);
}

void ShowAndSetSysStatus(uint8_t data,uint8_t newState)
{
    LedCtrl(data,newState);
}
 void syncResAndStatusShow(void)
 {
    Write595_LED(g_bkpData.sysResAndStatus.led_buf);
 }


 
//RELAY控制
#define RELAY_CTRL_STROBE_H        RELAY_CTRL_PORT->BSRR |= RELAY_CTRL_STROBE
#define RELAY_CTRL_DATA_H        RELAY_CTRL_PORT->BSRR |= RELAY_CTRL_DATA
#define RELAY_CTRL_ENABLE_H        RELAY_CTRL_PORT->BSRR |= RELAY_CTRL_ENABLE
#define RELAY_CTRL_CLK_H          RELAY_CTRL_PORT->BSRR |= RELAY_CTRL_CLK

#define RELAY_CTRL_STROBE_L        RELAY_CTRL_PORT->BRR |= RELAY_CTRL_STROBE
#define RELAY_CTRL_DATA_L        RELAY_CTRL_PORT->BRR |= RELAY_CTRL_DATA
#define RELAY_CTRL_ENABLE_L        RELAY_CTRL_PORT->BRR |= RELAY_CTRL_ENABLE
#define RELAY_CTRL_CLK_L          RELAY_CTRL_PORT->BRR |= RELAY_CTRL_CLK
static void Write4094(unsigned char data);
static void flafshIO(void);
void RelayCtrlPortInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  RELAY_CTRL_STROBE |RELAY_CTRL_DATA|RELAY_CTRL_ENABLE|RELAY_CTRL_CLK;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(RELAY_CTRL_PORT, &GPIO_InitStructure);
	RelayAllDown();

}
//通过移位锁存三态输出器CD4094，来控制继电器的开合
static void Write4094(unsigned char data)
{
	uint8_t tmp,i;
	tmp=data;	
	for (i=0;i<8;i++)
	{	
		RELAY_CTRL_CLK_H;
		if(tmp&0x80) 	
		{
            	    RELAY_CTRL_DATA_L;
		}
		else 	
		{
		    RELAY_CTRL_DATA_H;
		}
		tmp=tmp<<1;
                delay_us(1);
		RELAY_CTRL_CLK_L;
                delay_us(1);
	}
}
static void flashIO(void)
{
	signed char i;
	RELAY_CTRL_STROBE_L;//GPIOD->BRR |= 0x0800;				//PD11=锁存,1=输入,0=保持
	RELAY_CTRL_ENABLE_H;//GPIOD->BSRR|= 0x0200;				//PD9=Enable Output,0=高阻
	for(i=12 ;i >= 0;i--)	
	{
	    Write4094(RelayStatus[i]);	        //清继电器输出	
	}
	RELAY_CTRL_STROBE_H;//GPIOD->BSRR  |= 0x0800;			//PD11=锁存,,1=输入,0=保持	
	RELAY_CTRL_ENABLE_L;//GPIOD->BRR|= 0x0200;				//PD9=Enable Output,0=高阻
}
void RelayStatusAllClr(void)
{
        signed char i;
        for(i=12 ;i >= 0;i--)	
	{
	    RelayStatus[i] = 0;	       
	}
}
void RelayCtrl(unsigned char num,unsigned char newStatus)
{
        if(newStatus)
        {
            TST_RELAY_SEL_SET(num-1);
            
            flashIO();
        }
        else
        {
            TST_RELAY_SEL_CLR(num-1);
            flashIO();
        }
}

void RelayAllDown(void)
{
       #if 0 
         int i; 
        for(i=1;i<=100;i++)
        {
        RelayCtrl(i, RELAY_OFF);
        }   
        #endif
        RelayStatusAllClr();
        flashIO();

    
}

void RelayTest(void)
{
    unsigned char  tst = 100;
   // while(1)
    {
        RelayCtrl(tst, RELAY_ON);
         delay_ms(100);
    };
    while(1);
}

//按键值通过并转串的移位锁存器获得
#define KEY_165_SL_H        KEY_INPUT_PORT1->BSRR |= KEY_165_SL
#define KEY_165_CLK_H        KEY_INPUT_PORT2->BSRR |= KEY_165_CLK

#define KEY_165_SL_L        KEY_INPUT_PORT1->BRR |= KEY_165_SL
#define KEY_165_CLK_L        KEY_INPUT_PORT2->BRR |= KEY_165_CLK

void KeyInputPortInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  KEY_165_SL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(KEY_INPUT_PORT1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  KEY_165_CLK;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(KEY_INPUT_PORT2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  KEY_165_QH;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(KEY_INPUT_PORT3, &GPIO_InitStructure);
}
unsigned long input_165(void) 
{	
    uint8_t i;
    uint32_t tmp=0;
    KEY_165_CLK_L;       //GPIOA->BRR  |= 0x00000100;					//A8,P_CP=0;
    KEY_165_SL_L;           //GPIOC->BRR  |= 0x00001000;					//PC12,P_SL=0;
    __NOP();
   KEY_165_SL_H;            // GPIOC->BSRR  |= 0x00001000;					//PC12,P_SL=1;
    for(i=0;i<24;i++)
    {
        if(KEY_INPUT_PORT3->IDR & KEY_165_QH)	
        {
            tmp = (tmp<<1);
        }
        else 
        {
            tmp = (tmp<<1)+1; 
        }

        KEY_165_CLK_L;//GPIOA->BRR  |= 0x00000100;				//A8,P_CP=0;
        __NOP();	
        __NOP();
        KEY_165_CLK_H;//GPIOA->BSRR |= 0x00000100;				//A8,P_CP=1;
    }
    return(tmp);
}


//AD channel select
#define AD_CH_SEL_INH_PORT_MASK   0xF0
#define AD_CH_SEL_L1_PORT_MASK   0x0F
#define AD_CH_SEL_L2_PORT_MASK 0xF0

#define AD_CH_HIGH  0xF0
#define AD_CH_LOW   0x0F

#define AD_CH_SEL_INVALID  0x80


void AdChSelPortInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  AD_CH_L1_A|AD_CH_L1_B|AD_CH_L1_C|AD_CH_L1_D;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(AD_CH_SEL_L1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  AD_CH_L2_A|AD_CH_L2_B|AD_CH_L2_C|AD_CH_L2_D;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(AD_CH_SEL_L2_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  AD_CH_L1_INH1|AD_CH_L1_INH2|AD_CH_L1_INH3|AD_CH_L1_INH4
	                        |AD_CH_L1_INH5|AD_CH_L1_INH6|AD_CH_L1_INH7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(AD_CH_SEL_L1_INH_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  AD_CH_L2_INH;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(AD_CH_SEL_L2_INH_PORT, &GPIO_InitStructure);

        //断开L2
        AD_CH_SEL_L2_INH_PORT->BSRR |=  AD_CH_L2_INH;   //断开L2
        //断开L1
         AD_CH_SEL_L1_INH_PORT->BSRR |= (AD_CH_L1_INH1|AD_CH_L1_INH2|AD_CH_L1_INH3
                                                            |AD_CH_L1_INH4|AD_CH_L1_INH5|AD_CH_L1_INH6|AD_CH_L1_INH7);

}


static void AdChSelectL1(void)
{        
        //选择L1通路，由低4位决定,对应数值
        AD_CH_SEL_L1_PORT->BRR |= AD_CH_SEL_L1_PORT_MASK;
        AD_CH_SEL_L1_PORT->BSRR |= (g_curAdCh-1)&AD_CH_SEL_L1_PORT_MASK;

        //打开L1选中通路的使能，由高4位决定,但对选通信号而言是低选通
        AD_CH_SEL_L1_INH_PORT->BSRR |= AD_CH_L1_INH1|AD_CH_L1_INH2|AD_CH_L1_INH3|AD_CH_L1_INH4
            |AD_CH_L1_INH5|AD_CH_L1_INH6|AD_CH_L1_INH7;       
        AD_CH_SEL_L1_INH_PORT->BRR |=  1<< (unsigned char)((g_curAdCh-1)>>4);

}

static void AdChSelectL2(void)
{
        //选择L2通路,这也是由高4位决定
         AD_CH_SEL_L2_PORT->BRR |= AD_CH_L2_A|AD_CH_L2_B|AD_CH_L2_C|AD_CH_L2_D;
         AD_CH_SEL_L2_PORT->BSRR |= ((g_curAdCh-1)>>4)<<1;
         
        //打开L2选中通路的使能，这里只有一个选择
        AD_CH_SEL_L2_INH_PORT->BRR |=  AD_CH_L2_INH;   //打开L2
}

void singleAdChSelect(uint8_t ch)
{      
 
         if(ch <1 ||ch>100)
         {
            return;
         }

         //全部断开L2
         AD_CH_SEL_L2_INH_PORT->BSRR |=  AD_CH_L2_INH;   //断开L2  

         //全部断开L1
         AD_CH_SEL_L1_INH_PORT->BSRR |= (AD_CH_L1_INH1|AD_CH_L1_INH2|AD_CH_L1_INH3
                                                            |AD_CH_L1_INH4|AD_CH_L1_INH5|AD_CH_L1_INH6|AD_CH_L1_INH7);
         
        //AdChSelectL1();
        //选择L1通路，由低4位决定,对应数值
        AD_CH_SEL_L1_PORT->BRR |= AD_CH_SEL_L1_PORT_MASK;
        AD_CH_SEL_L1_PORT->BSRR |= (ch-1)&AD_CH_SEL_L1_PORT_MASK;

        //打开L1选中通路的使能，由高4位决定,但对选通信号而言是低选通
        AD_CH_SEL_L1_INH_PORT->BSRR |= AD_CH_L1_INH1|AD_CH_L1_INH2|AD_CH_L1_INH3|AD_CH_L1_INH4
            |AD_CH_L1_INH5|AD_CH_L1_INH6|AD_CH_L1_INH7;       
        AD_CH_SEL_L1_INH_PORT->BRR |=  1<< (unsigned char)((ch-1)>>4);
        //delay_ms(L1_OPEN_DELAY);
        
        //AdChSelectL2();
        //选择L2通路,这也是由高4位决定
         AD_CH_SEL_L2_PORT->BRR |= AD_CH_L2_A|AD_CH_L2_B|AD_CH_L2_C|AD_CH_L2_D;
         AD_CH_SEL_L2_PORT->BSRR |= ((ch-1)>>4)<<1;
         
        //打开L2选中通路的使能，这里只有一个选择
        AD_CH_SEL_L2_INH_PORT->BRR |=  AD_CH_L2_INH;   //打开L2
        //delay_ms(L2_OPEN_DELAY);
        //delay_ms(AD_CH_OPEN_DELAY);  //可以在选择L1,L2后，一起进行延时，可缩短时间
}

void autoAdChSelect(void)
{
  

}

/////////////SQ信号
void SqCtrlPortInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin =  SQ_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  
    GPIO_Init(SQ_CTRL_PORT, &GPIO_InitStructure);
}

void SqExtiInit(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    
    EXTI_ClearITPendingBit(SQ_OUT_EXTI_LINE);

    /* Connect SQ EXTI Line to SQ GPIO Pin */
    GPIO_EXTILineConfig(SQ_OUT_EXTI_PORT_SOURCE, SQ_OUT_EXTI_PIN_SOURCE); 

        /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = SQ_OUT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}



void StartSqDetect(void) //启动下降沿检测
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* Enable the EXTI Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = SQ_OUT_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //或需仔细考究
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
}
unsigned char GetSqLevel(void) //取SQ电平值
{
          if (SQ_CTRL_PORT->IDR & SQ_OUT)
          {
                return 1;
          }
          else
          {
                return 0;
          }
}

void SqInit(void)
{
    SqCtrlPortInit();
    SqExtiInit();
}
void SqTest(void)
{
    SqInit();
    xprintf("bit = %d\n",GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1));
    while(1);

}


