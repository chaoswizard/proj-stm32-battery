#include "stm32f10x.h"	
#include "bsp.h"
#include "Global.h"
#include "ad7705.h"
#include "input_output.h"
#include "xprintf.h"



//����ϵͳʱ��,ʹ�ܸ�����ʱ��
void RCC_Configuration(void)
{
	//��ʼ��ϵͳʱ��,�ú����ڽ���__main֮ǰ�㱻���á���startup_stm32f10x_xxx.s��
	//SystemInit();						

	//ʹ������ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB 
	| RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);   

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	// Enable PWR and BKP clocks  
	PWR_BackupAccessCmd(ENABLE);	// Allow access to BKP Domain  

}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;	//����ṹ�����

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�������ȷּ���

	#if 0
	// Enable the RTC Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif

}

void GPIO_Configuration(void)
{
        #if 1    //���º������������ŵ����ԣ����ﶼ���Բ�����
        //�ڸ�ģ��ĳ�ʼ���У����ٴε��ã��Ա�֤ģ��������
	KeyInputPortInit();
	AdChSelPortInit();
	AD7705PortInit();
	RelayCtrlPortInit();
	LedCtrlPortInit();
	SpiFlashPortInit();
	SqCtrlPortInit();
	ReverseDetectPortInit();
	#endif
}

/*******************************************************************************
* Function Name  : SysTick_SetReload
* Description    : Sets SysTick Reload value.
* Input          : - Reload: SysTick Reload new value.
*                    This parameter must be a number between 1 and 0xFFFFFF.
* Output         : None
* Return         : None
*******************************************************************************/
static void SysTick_SetReload(u32 Reload)
{
	/* Check the parameters */
	assert_param(IS_SYSTICK_RELOAD(Reload));

	SysTick->LOAD = Reload;
}

/*******************************************************************************
* Function Name  : SysTick_CounterCmd
* Description    : Enables or disables the SysTick counter.
* Input          : - SysTick_Counter: new state of the SysTick counter.
*                    This parameter can be one of the following values:
*                       - SysTick_Counter_Disable: Disable counter
*                       - SysTick_Counter_Enable: Enable counter
*                       - SysTick_Counter_Clear: Clear counter value to 0
* Output         : None
* Return         : None
*******************************************************************************/
/* SysTick counter state */
#define SysTick_Counter_Disable        ((u32)0xFFFFFFFE)
#define SysTick_Counter_Enable         ((u32)0x00000001)
#define SysTick_Counter_Clear          ((u32)0x00000000)
static void SysTick_CounterCmd(u32 SysTick_Counter)
{
	/* Check the parameters */
	assert_param(IS_SYSTICK_COUNTER(SysTick_Counter));

	if (SysTick_Counter == SysTick_Counter_Enable)
	{
		SysTick->CTRL |= SysTick_Counter_Enable;
	}
	else if (SysTick_Counter == SysTick_Counter_Disable) 
	{
		SysTick->CTRL &= SysTick_Counter_Disable;
	}
	else /* SysTick_Counter == SysTick_Counter_Clear */
	{
		SysTick->VAL = SysTick_Counter_Clear;
	}    
}

/*******************************************************************************
* Function Name  : SysTick_ITConfig
* Description    : Enables or disables the SysTick Interrupt.
* Input          : - NewState: new state of the SysTick Interrupt.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
/* ---------------------- SysTick registers bit mask -------------------- */
/* CTRL TICKINT Mask */
#define CTRL_TICKINT_Set      ((u32)0x00000002)
#define CTRL_TICKINT_Reset    ((u32)0xFFFFFFFD)
static void SysTick_ITConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    SysTick->CTRL |= CTRL_TICKINT_Set;
  }
  else
  {
    SysTick->CTRL &= CTRL_TICKINT_Reset;
  }
}
static void  OS_SysTick_Config(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    uint32_t         cnts;

    RCC_GetClocksFreq(&rcc_clocks);
    xprintf("SYSCLK_Frequency = %d\nHCLK_Frequency=%d\n",\
        rcc_clocks.SYSCLK_Frequency,rcc_clocks.HCLK_Frequency);
    cnts = (uint32_t)rcc_clocks.HCLK_Frequency/2/OS_TICKS_PER_SEC;
    SysTick_SetReload(cnts);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_CounterCmd(SysTick_Counter_Enable);
    SysTick_ITConfig(ENABLE);
}

void RTC_Configuration(void)
{  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	// Enable PWR and BKP clocks  
	PWR_BackupAccessCmd(ENABLE);	// Allow access to BKP Domain  
	BKP_DeInit();	// Reset Backup Domain   
	RCC_LSEConfig(RCC_LSE_ON);	//Enable LSE  
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)  
	{}	// Wait till LSE is ready  
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


//��ɻ���Ӳ������������
void BSP_Init(void)
{
//	uint32_t i;
	//�����豸����
	// 1 ʱ�����ȶ����еĻ���
	RCC_Configuration();		//��λ��ʱ�ӿ���(RCC). 

	// 2�����ж��쳣������ϵͳ����δ֪�쳣״̬
	NVIC_Configuration();		//�ж����á�

	// 3GPIO�ŵ����ã���������Χ������Ӳ���ӿڣ�Ӧʹ֮����һ��ȷ���ȶ���״̬��
	GPIO_Configuration();	//GPIO�����á�

	// 4����ģ������.������Χ�豸������ƽ̨����ǰ��ʼ����ɣ�Ҳ�ڴ˴����á�
	xPrintf_Init();

	//5 ...�����豸��ʼ����ÿ���豸�ĳ�ʼ������ǰ��������й�ϵ����Ϊÿһ
	//���豸������Ҫ(1)������ĳһʱ����;(2)�����е��ж����ü���Ӧ;(3)����Χ
	//������GPIO�ܽ�(��ʱ����ӳ�䣬ʵ���ϣ����ǽ��ܽŴ�Ĭ�����Ӹĵ����ʵ�
	//�Ŀ���������)
	AD7705_stop(); 
	OS_SysTick_Config();   // ϵͳ��׼ʱ��10 ms 

	//RTC_Configuration();
        //////////////////////ģ���ʼ��
	Rtc_time_init();        //Rtc time init. using one register to decide if there needed to re-Adjust.and first time convert rtc time to 'systmtime��
	LedCtrlInit();          //led���Ƴ�ʼ����Ĭ��Ϊ����
	SqInit();                   //SQ exti init,but not open yet.
	AD7705_init();	//SAD��λ����ʼ��
	spiFlashInit();         //�洢�豸1
	
}

void   lock_all_int(void)
{
        //NVIC_DisableIRQ(SysTick_IRQn);
}

void   unlock_all_int(void)
{
        //NVIC_EnableIRQ(SysTick_IRQn);
}



