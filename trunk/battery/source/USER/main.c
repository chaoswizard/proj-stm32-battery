/******************************************************************************
  * CPU:STM32F103	R	B	T	6	
  * R=64Pins(C=48,V=100),	B=128K(8=64),T=LQFP,6=[-40-85](8=-40-125)
*******************************************************************************/ 

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "lcd12864.h"
#include "input_output.h"
#include "at45db041.h"
#include "keyboard.h"
#include "ad677.h"

#define GLOBAL
#include "global.h"

#include "bits_types.h"
#include "common_define.h"
#include "brush_print.h"
#include "uimmi_ctrl.h"
#include "uikeypad_ctrl.h"
#include "uimmi_content.h"
#include "uimenu_ctrl.h"
#include "gui_menu_item.h"
#include "gui_osd_obj.h"
#include "ui_com_obj.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCConvertedValue[8];


/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void Timer_Config(void);
void DMA1_ADC1_Configuration(void);
void RTC_Configuration(void);


void TIM2_IRQHandler(void)
{
	uint32_t tmp;
	//static uint8_t jj;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{	//10mS
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);	//清中断
		Flag_10ms_second=1;
		//-------------------Check Key Press-----------------
		if(keyDelay==0)
		{
			tmp=input_165();
			if(tmp!=0){
				if(preKey==tmp)	{
					preKey=0;	curKey=tmp;	Flag_keyPressed=1;
					keyDelay=20;	GPIOA->BSRR |= 0x0400;	/*PA10=SPK*/							
				}else preKey=tmp;
			}
		}else 
		{
			if(keyDelay--==14)	GPIOA->BRR |= 0x0400;	/*PA10=SPK*/
		}	
		//---------------Check Input------------------------
		if((GPIOB->IDR & 0x0002)!=0)		//PB1
		{
				if(!Flag_exInput)	
				{
					if(inputDelay++>10) 
						Flag_exInput=1;
				}	
		}
		else {	inputDelay=0; Flag_exInput=0;	}
		//---------------Start ADC1-------------------------
		//if(++jj>10)
		{
//				jj=0;
		}
	}
}

void TIM1_UP_TIM16_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_FLAG_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);	//清中断
//		t1count++;
	}	
}

//void TIM3_IRQHandler(void)
//{	//0.5us,AD677.CLK
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
//	{
//		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);	//清中断
//		if((GPIOE->ODR)&1) GPIOE->BRR|=0x00000001;		//PE0=CLK,=0
//		else GPIOE->BSRR|=0x00000001;		//PE0=CLK,=1
//	}
//}
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM4,TIM_FLAG_Update);	//清中断
//		t4count++;
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//启动ADC1软转换		
	}
}

void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    // Toggle LED1
    //STM_EVAL_LEDToggle(LED1);
		Flag_1_second	=1;    
//		rtcount++;
    RTC_ClearITPendingBit(RTC_FLAG_SEC);		// Clear Interrupt pending bit 
  }
}

void DMA1_Channel1_IRQHandler(void)
{
  if (DMA1->ISR & (1 << 1)) {            // TCIF interrupt?
    Flag_AD_done = 1;
    DMA1->IFCR  = (1 << 1);              // clear TCIF interrupt              
  }
}

int main(void)
{
	int i,j,k;  	
	long tadd;
  RCC_Configuration();				// System clocks configuration
  GPIO_Configuration();				// GPIO configuration 		
	for(i=0;i<13;i++) OutputBuf[i]=0;
	flashIO();
	
  NVIC_Configuration();				// NVIC configuration 
	Timer_Config();							// TIMER configuration 
	RTC_Configuration();
	DMA1_ADC1_Configuration();	
	while(1)
	{
		RST_DB041();
		i=ReadStatusFromDataFlash();	//[7]RDY/BUSY,[6]COMP,[5,4,3,2]=0111,[1]PROTECT,[0]=PAGESIZE
    if((i&0x3C)==0x1C) break;			//正常0x9C
		for(j=0;j<200;j++);
  } 	
#if 1
    ui_mmi_open();
    sprintf(mystr,"ABC%d",123);  printSmall(1,10);while(1);
    ui_mmi_start();
    while (1)
    {
        ui_mmi_proc();
    }
    ui_mmi_close();
#endif


    
	curIndex=0;	drawMainFace(1);
	led_Buf=0x08;	Flag_LEDChange=1;
	ADBufIndex=0;	scanCh=0;	changeChannel();	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)	//1..42
  {    
    RTC_Configuration();	// RTC Configuration
    /* Adjust time by values entered by the user on the hyperterminal */
    //Time_Adjust();
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
//      printf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
//      printf("\r\n\n External Reset occurred....");
    }
//    printf("\r\n No need to configure RTC....");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    // Enable the RTC Second
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    // Wait until last write operation on RTC registers has finished		
    RTC_WaitForLastTask();
  }
  RCC_ClearFlag();		// Clear reset flags
	ReadMemory(EnableCh,100,2047,0);	
	for(k=0;k<100;k++)
	{	
		if(EnableCh[k]!=1) EnableCh[k]=0;
		StatusCh[k]=0;
	}
	ReadMemory(tmpSet,5,2047,100);
	workMode=tmpSet[0];	if(workMode>2) workMode=0;
	setingTime=tmpSet[1]*256+tmpSet[2];
	setingVal=tmpSet[3]*256+tmpSet[4];
	
	//Calibration();	//开机前校正AD677
	
	Flag_CheckDealy=0;
//	Voltage[1]=0xFFF;
//	Voltage[2]=0;
  while (1)
  {
		if(Flag_exInput)
		{
			if(!Flag_IsRunning)
			{
				Flag_IsRunning=1;
				for(k=0;k<100;k++)
				{	
					if(EnableCh[k]!=1) {	StatusCh[k]=0; tmpSet[k]=0;	}
					else	{ StatusCh[k]=1;	tmpSet[k]=1;	}
					//if(Voltage[k]<30) led_Buf|=0x40;	//电压过低报警
				}				
				testIOTran(); Flag_OutputChange=1;		//继电器吸合
				led_Buf|=0x80;	led_Buf&=0xF7;	Flag_LEDChange=1;
				runingTime=setingTime;	runingVal=setingVal;
				Flag_CheckDealy=1;	Sec3Count=0; saveCount=0;
				
				ReadMemory(tmpSet,1,2047,105);
				saveCurveIndex=tmpSet[0];
				if(++saveCurveIndex>2) saveCurveIndex=0;
				tmpSet[0]=saveCurveIndex;
				write_Page(tmpSet,1,2047,105);
				tmpSet[0]=saveCount%256;	//曲线长度清0
				tmpSet[1]=saveCount/256;	//曲线长度清0
				write_Page(tmpSet,2,2047,106+saveCurveIndex*2);
			}
		}
		if(Flag_1_second)
		{
			Flag_1_second	=0;
			if(Flag_IsRunning)
			{
				if(++Sec3Count>2)
				{
					Sec3Count=0;
					if(Flag_CheckDealy)
					{
						Flag_CheckDealy=0;
						for(k=0;k<100;k++)
						{	
							if(Voltage[k]<30) led_Buf|=0x40;	//电压过低报警
						}
					}						
					if(saveCount<600) 
					{
						tadd=saveCurveIndex*120000L+200*saveCount;
						write_flash((unsigned char*)(&Voltage[0]),200,tadd);	//保存电压
						++saveCount;							//曲线更新长度
						tmpSet[0]=saveCount%256;	
						tmpSet[1]=saveCount/256;	
						write_Page(tmpSet,2,2047,106+saveCurveIndex*2);
					}
				}	//3S
				if(workMode==0)
				{	//时间模式
					if(runingTime--==0)
					{
						for(k=0;k<100;k++) {	StatusCh[k]=0;	tmpSet[k]=0;	}
						testIOTran(); Flag_OutputChange=1;		//继电器断开						
						Flag_IsRunning=0; led_Buf&=0x7F;	led_Buf|=0x08;	Flag_LEDChange=1;
					}
				}else if(workMode==2)
				{
				}
				
			}			
			
			if(curFace==MAIN_FACE)
			{
				refreshMainFace();
			}
		}		

		if(Flag_AD_done) 
		{
			Flag_AD_done = 0;
//			Voltage[0]=(unsigned int)(2475.0*ADCConvertedValue[0]/4095.0);//(unsigned int)(5000UL*ADCConvertedValue[0]/4095);
//			Voltage[0]=(unsigned int)((25.0+(1430.0-2480.0*ADCConvertedValue[0]/4095.0)/4.3));	
//			Voltage[0]=ADCConvertedValue[0];
//			if(Voltage[0]<Voltage[1]) Voltage[1]=Voltage[0];
//			if(Voltage[0]>Voltage[2]) Voltage[2]=Voltage[0];
/*			
			for(i=0;i<7;i++)
			{
					j=i*16+scanCh;						
					if(j<100) ADBuf[j][ADBufIndex]=ADCConvertedValue[i];
					else break;
			}
			if(++scanCh>15)
			{
				scanCh=0;		
				if(++ADBufIndex>6)
				{
					for(k=0;k<100;k++)
					{	
						if(EnableCh[k]!=1) continue;
						Bublesort(ADBuf[k],7);
						j=(ADBuf[k][2]+ADBuf[k][3]+ADBuf[k][4])/3;
						Voltage[k]=(unsigned int)(4960UL*j/4095);
						//if(Voltage[k]<30) Voltage[k]=0;
					}
					ADBufIndex=0; 
				}
			}
			changeChannel();
	*/
			//TIM_SetCounter(TIM4,0);		TIM_ClearFlag(TIM4,TIM_FLAG_Update);			//清除标志
		}
		if(Flag_10ms_second)
		{	//10ms
			Flag_10ms_second=0;
			if(Flag_OutputChange)		{		Flag_OutputChange=0; flashIO();						}
			if(Flag_LEDChange)			{		Flag_LEDChange=0;		Write595(led_Buf);		}
			Voltage[0]=Sample();
		}		
		if(Flag_keyPressed)
		{
			if(curKey!=0) KeyServer(curKey);
			Flag_keyPressed=0;	curKey=0;	preKey=0;
		}    
    IWDG_ReloadCounter();  	// Reload IWDG counter
  }
}

void RCC_Configuration(void)
{
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);

  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div2); 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
		RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);    
}

void GPIO_Configuration(void)
{
	GPIOA->CRL =0x00000000;		//模拟输入
	GPIOA->CRH =0x00001181;		//PA8=LS165_CK,PA10=SPK,PA11=LCD_CS1, PA11=LCD_CS1(RUN_LED)	10MHz推挽输出
	GPIOA->ODR|=0x00000200; 	//PA9=165_DAT上拉输入(默认下拉)
	
	GPIOB->CRL =0x11100081;		//PB0=DB_RST,PB5=LCD_DI,PB6=LCD_RW,PB7=LCD_E
	GPIOB->CRH =0x00000011;		//P8=LCD_CS2,PB9=LC9_RST 	10MHz推挽输出
	GPIOB->ODR|=0x00000002; 	//PB1=Ex_Input上拉输入(默认下拉)

	GPIOC->CRL =0x10001111;		//CD4067[A,B,C,D],	PC7=595_DAT
	GPIOC->CRH =0x00010011;		//PC8=595_SCK,PC9=595_LAT, PC12=165.PL	10MHz推挽输出
	
	GPIOD->CRL =0x11111111;		//PD8=4094_CLK,PD9=4094_ENABLE,PD10=4094_DATA,PD11=4094_STROBE
	GPIOD->CRH =0x18115555;		//PD12=041_CS, PD13=041_SCK, PD15=041_SI	10MHz推挽输出
	GPIOD->ODR|=0x00004000; 	//PD14=041_SO上拉输入(默认下拉)

	GPIOE->CRL =0x01414411;		//PE0=CLK,PE1=Sample,PE2=SDATA,PE3=SCLK,PE4=CAL,PE5=BUSY
	GPIOE->CRH =0x00000000;		//PE6=
	GPIOE->BRR|=0x0000006C;
}

void Timer_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		//定义TIM结构体变量
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//使能TIM1外设
	TIM_DeInit(TIM1);	//复位时钟TIM1，恢复到初始状态
	TIM_TimeBaseStructure.TIM_Period = 39999;
	TIM_TimeBaseStructure.TIM_Prescaler = 599;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//TIM1时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数方式
	//定时时间计算公式:	
	//	T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK
	//	 =(39999+1)*(5+1)/24MHz=10mS
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);		//初始化
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);			//清除标志
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);	//使能中断源
	TIM_Cmd(TIM1, ENABLE);					//使能TIM1
//--------------------------------------------------------------
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2外设
	TIM_DeInit(TIM2);	//复位时钟TIM2，恢复到初始状态
	TIM_TimeBaseStructure.TIM_Period = 39999;
	TIM_TimeBaseStructure.TIM_Prescaler = 5;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//TIM2时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数方式
	//定时时间计算公式:	
	//	T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK
	//	 =(39999+1)*(5+1)/24MHz=10mS
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);		//初始化
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);			//清除标志
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	//使能中断源
	TIM_Cmd(TIM2, ENABLE);					//使能TIM2
//--------------------------------------------------------------

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3外设
//	TIM_DeInit(TIM3);	//复位时钟TIM3，恢复到初始状态
//	TIM_TimeBaseStructure.TIM_Period = 11;
//	TIM_TimeBaseStructure.TIM_Prescaler = 0;
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//TIM3时钟分频
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数方式
	//定时时间计算公式:	
	//	T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK
	//	 =(11+1)*(1)/24MHz=0.5uS
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		//初始化
//	TIM_ClearFlag(TIM3,TIM_FLAG_Update);			//清除标志
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);	//使能中断源
//	TIM_Cmd(TIM3, ENABLE);					//使能TIM3
//----------------------------------------------------------------

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM4外设
	TIM_DeInit(TIM4);	//复位时钟TIM4，恢复到初始状态
	TIM_TimeBaseStructure.TIM_Period = 23999;
	TIM_TimeBaseStructure.TIM_Prescaler = 3;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//TIM4时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数方式
	//定时时间计算公式:	
	//	T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK
	//	 =(23999+1)*(1)/24MHz=1 mS
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		//初始化
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);			//清除标志
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);	//使能中断源
	TIM_Cmd(TIM4, ENABLE);					//使能TIM4	
//-----------------------------------------------------------------------
//  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//  IWDG_SetPrescaler(IWDG_Prescaler_256);
//  IWDG_SetReload(0xFFF);	//40K,最大256  
//  IWDG_ReloadCounter();		// Reload IWDG counter 
//  IWDG_Enable();
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;	//定义结构体变量

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	//设置优先分级组
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;	//选择中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断向量配置	
  NVIC_Init(&NVIC_InitStructure);	//按以上设置初始化中断向量

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	//选择中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断向量配置	
  NVIC_Init(&NVIC_InitStructure);	//按以上设置初始化中断向量

//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	//选择中断通道
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断向量配置	
//  NVIC_Init(&NVIC_InitStructure);	//按以上设置初始化中断向量

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	//选择中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断向量配置	
  NVIC_Init(&NVIC_InitStructure);	//按以上设置初始化中断向量

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;	//选择中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断向量配置	
  NVIC_Init(&NVIC_InitStructure);	//按以上设置初始化中断向量
	
  // Enable the RTC Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}
void DMA1_ADC1_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

  // DMA1通道1配置----------------------------------------------
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;						//用于定义DMA外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;	//用于定义DMA内存基地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;										//定义外设是目的还是源
  DMA_InitStructure.DMA_BufferSize = 7;																	//指定DMA缓存的大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址是否递增
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;								//内存地址是否递增
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//定义外设数据长度
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;		//HalfWord=16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;												//工作在循环缓存模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;										//高优先级
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;													//禁止内存到内存传输
  DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);     // enable DMA1 Channel1 Interrupt 
  DMA_Cmd(DMA1_Channel1, ENABLE);		//使能DMA1通道1 
  
  //ADC1配置------------------------------------------------------
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//独立模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;					//扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 7;								//采样通道数
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5); //239
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_41Cycles5); //71
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_41Cycles5); //55
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_41Cycles5); //41
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_41Cycles5); //28
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_41Cycles5); //13
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_41Cycles5); //7

	
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_41Cycles5);	//内部参考电压1.2V[1.16,1.24]
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);		//Temperature=25+(1430-3300*temp/4095)/4.3;
	ADC_TempSensorVrefintCmd(ENABLE);	//使能温度传感器通道
	

  ADC_DMACmd(ADC1, ENABLE);  												//使能ADC1的DMA  
  ADC_Cmd(ADC1, ENABLE);  													//使能ADC1
  ADC_ResetCalibration(ADC1);												//重置ADC1的校准寄存器 
  while(ADC_GetResetCalibrationStatus(ADC1));				//等待ADC1复位校准寄存器结束
  ADC_StartCalibration(ADC1);  											//启动ADC1校准
  while(ADC_GetCalibrationStatus(ADC1));  					//等待ADC1校准完成
	
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);						//启动ADC1软转换		
}

void RTC_Configuration(void)
{  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	// Enable PWR and BKP clocks  
  PWR_BackupAccessCmd(ENABLE);	// Allow access to BKP Domain  
  BKP_DeInit();	// Reset Backup Domain   
  RCC_LSEConfig(RCC_LSE_ON);	//Enable LSE  
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)  {}	// Wait till LSE is ready  
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	// Select LSE as RTC Clock Source  
  RCC_RTCCLKCmd(ENABLE);	// Enable RTC Clock  
  RTC_WaitForSynchro();		// Wait for RTC registers synchronization   
  RTC_WaitForLastTask();	// Wait until last write operation on RTC registers has finished  
  RTC_ITConfig(RTC_IT_SEC, ENABLE);	// Enable the RTC Second  
  RTC_WaitForLastTask();	// Wait until last write operation on RTC registers has finished
  // Set RTC prescaler: set RTC period to 1sec 
  RTC_SetPrescaler(32767); 	// RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)   
  RTC_WaitForLastTask();		// Wait until last write operation on RTC registers has finished
}
#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif
