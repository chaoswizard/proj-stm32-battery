
#include "stm32f10x.h"

#include "stm32f10x_conf.h"

#include "AD7705.H"
#include "global.h"
#include "uimmi_ctrl.h"
#include "input_output.h"

#define WAIT_DRDY_TMR_DELAY  20
static SWTMR_NODE_HANDLE gAdSampTmr;
static volatile uint8_t gAdCurSampStatus = 0;

#define SAMP_END_LINE        100//s100       //����������+1��λ��
#define SAMP_START_LINE        0//������ʼ��
#define SET_VOL   36700             // 2.8v

enum {
       AD_SAMP_STEP_STOP,
       AD_SAMP_STEP_CMD,
       AD_SAMP_STEP_SEL_DELAY,
       AD_SAMP_STEP_DRDY_DELAY,
       AD_SAMP_STEP_START,
       AD_SAMP_STEP_DRDY,
};
void sad_delay(unsigned long n)
{
	unsigned long j;
	while(n--)
	{ 
	    j=108;               // 8
	    while(j--);
	}
}

static void write_ad7705reg(uint8_t  byteword);

#define SEND_CMD(v)  write_ad7705reg(v)	   


void AD7705PortInit (void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  SCLK_PIN |DIN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(AD_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  DOUT_PIN |DRDY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(AD_PORT, &GPIO_InitStructure);   

	SCLK_H;
	DIN_H;
}

static void write_ad7705reg(uint8_t  byteword)  
{
        uint8_t temp;
	uint32_t i;
	//CS=0;
	temp=0x80;
	for(i=0;i<8;i++)
	{
		SCLK_L;
		if((temp&byteword)==0)
		{
			DIN_L;
		}
		else 
		{
			DIN_H;
		}

		sad_delay(WRITE_BAND_L);
		SCLK_H;
		sad_delay(WRITE_BAND_H);
		temp=temp>>1;
	}
	//CS=1;
}

void AD7705_reset(void)  
{ 
	int i = 0;
	for(i = 0;i<5;i++)
	{
		write_ad7705reg(0xff);		//DIN��������32��1����������λ
	}
} 

void AD7705_init(void)
{
        AD7705PortInit();
	AD7705_reset();
	
	// 1  ��ͨ�żĴ���д���ݣ�ѡ��ͨ��2
	//��Ϊ��Чͨ��������һ������Ϊ��ʱ�ӼĴ�������д����
	SEND_CMD(0x21);
	// 2��ʱ�ӼĴ���д����������CLKDIVλ�����ⲿʱ�ӳ�����
	//�ٶ��ⲿʱ��Ƶ�� 4.9512MHZ,������ѡΪ100hz
	write_ad7705reg(0x0c);
	
	// 3 ��ͨ�żĴ���д���ݣ�ѡ��ͨ��2��Ϊ��Чͨ��������һ
	//������Ϊ�����üĴ�����д������10b---- 100HZ; 11b---200Hz
	SEND_CMD(0x11);  //0x12 ---100hz,0x13---200hz,Ŀǰֻ����������ȶ��Ը�
	/*ZERO (0) ZERO (0) ZERO (0) CLKDIS (0) CLKDIV (0) CLK (1) FS1 (0) FS0 (1)*/
	//SEND_CMD(0x1D);  //��ղ�һ�����  ,60HZ,����Ϊ�ˣ������30ms���һ·
	//��ô3s���������һ·�Ĳ�������������Ҫ����3.5s������ɲ���
	//������spi flash,usb,bkp�ȵ��޸�ʱ��ֻʣ��500ms�ˣ�������ܲ������Դ���
	//һ�ֵ�����xxxx��
	//SEND_CMD(0x17);  //���Ҳ��Ҫ4s�����ȶ��Ժ����0x11Ҫ��
        //SEND_CMD(0x16);  //���Ҳ��Ҫ4s����,�ȶ��Ժ����0x11Ҫ��
        //SEND_CMD(0x15);  //���Ҳ��Ҫ4s�����ȶ��Ժ����0x11Ҫ��-->���
        //SEND_CMD(0x14);  //���Ҳ��Ҫ4s���ң��ȶ��Ժ����0x11Ҫ��
        //SEND_CMD(0x13);  //���Ҳ��Ҫ4s���ң��ȶ��Ժ����0x11Ҫ��
        //SEND_CMD(0x19);  //���Ҳ��Ҫ4s���ң�xxxxx����
                
	// 4�����üĴ���д���ݣ���������Ϊ1������Ϊ�����ԡ��ǻ���ģʽ
	// ����˲���ͬ������ʼ��У׼
	write_ad7705reg(0x44);
	
	// 5,��ѯ~DRDY���
	//while ( DRDY_read );	
	// 6 �����ݼĴ����϶�����
	// 7���ص�5����6����ֱ����ѡ����ͨ����ȡ��ָ�������Ĳ���
	SCLK_H;
	DIN_H;
}

static void  Read_ad7705_end(void)
{
        struct EVENT_NODE_ITEM e;

        g_LastAdCh++;
        e.sig = EVENT_SYS_HW_AD;
        SYS_EVENT_AD_INIT(e.param,  g_LastAdCh);
        ui_mmi_send_msg(&e);
}

//���һ�����ݲ�������øú������ܶ�״̬���µĹ����ڴ˽���
//�ú���Ϊ���һ�����ݲ���������ݴ�����̣�����������Զ�
void ad7705_dump(uint16_t grp)
{
    int i;
    //xprintf("\n\n=========================\n");
    //     while(g_curAdCh--)
    //    xprintf("--%d: %d-%d-\n",g_curAdCh ,batteryVolAdArray[g_curAdCh], grp);
    if(IS_SYS_NO_U_PAN() )
    {
        //���浽spi flash ��
        SaveAdData((uint8_t*)batteryVolAdArray);

    }
    else
    {
        //�������ݵ�u pan.
        
        xprintf("save data to u-pan\n");
    }

     if(0)                //gAdSampConfig.mode_sel���������ﵽ����ϵͳ����״̬Ϊ"����"������Ҫ������ad����   �ȴ�SQ_END.
     {
        SET_SYS_RUN_STATUS_END();
        
        //��u�̻��������������flush.�ر��ļ�
        
        MoveToNextGroup();                          //�е���һ�飬��ʼ.������ͬ������Ϣͬ��������������
     }
     else               //  ��ʼ��һ�еĲ���
     {
      
        MoveToNextLine();
        AD7705_start();           //������һ�в���
     }

     SaveBkpData(TABLE_BKP);
     SaveBkpData(TABLE_BAK_BKP);

}


#define AD_SAMP_STEP_SEL_DELAY_CNT 14
void Read_ad7705(void *p)
{
    static uint8_t delay_cnt;
      if (AD_SAMP_STEP_STOP == gAdCurSampStatus )
      {
            return;
      }
      
      if(SAMP_END_LINE == g_curAdCh) //SYS_AD_CH_MAX  //for test
      {
            AD7705_stop();                         //����ָ����һ�У�������100·Ϊһ�У�
            Read_ad7705_end();                //�������һ�е���Ϣ���ȴ���������������������´�
            return;
      }
          
        switch (gAdCurSampStatus)
        {
            case AD_SAMP_STEP_START:
                singleAdChSelect(g_curAdCh+1);               
            	gAdCurSampStatus = AD_SAMP_STEP_SEL_DELAY;
                break;
            case AD_SAMP_STEP_SEL_DELAY:
                delay_cnt++;
                if(delay_cnt >= AD_SAMP_STEP_SEL_DELAY_CNT)                              //��ֵ��12�����ȶ���ֻ����13
                {
                    delay_cnt = 0;
            	    gAdCurSampStatus = AD_SAMP_STEP_CMD;
            	 }
                break;
            case AD_SAMP_STEP_CMD:
            	//�����ݼĴ���
            	SEND_CMD (0x39);   
            	gAdCurSampStatus = AD_SAMP_STEP_DRDY_DELAY;
                break;
            case AD_SAMP_STEP_DRDY_DELAY:
                gAdCurSampStatus = AD_SAMP_STEP_DRDY;
                break;
            case AD_SAMP_STEP_DRDY:
            	if (DRDY_read)
            	{
            	        xputc('*');
            	        break;
            	}
            	else
            	{
                        batteryVolAdArray[g_curAdCh] = Read_ad7705_data();

                        //��֮ǰ�����ڷŵ��У�����������Ϊ�ŵ����,���Ͽ��̵���
                          if((batteryVolAdArray[g_curAdCh] < gAdSampConfig.mode_vol)&&TST_CH_WORK_STATE_TST(g_curAdCh))
                        {
                            TST_CH_WORK_STATE_CLR(g_curAdCh);
                            RelayCtrl(g_curAdCh+1, RELAY_OFF);
                        }
                	gAdCurSampStatus = AD_SAMP_STEP_START;
                  	g_curAdCh++;
            	}
            break;
            default:
            break;
        }
}
uint16_t PollReadAdByCh(uint8_t ch)
{
    uint16_t val = 0;
    singleAdChSelect(ch);

    //����cpu��ʱ
    delay_ms(10);

    //�����ݼĴ���
    SEND_CMD (0x39);   

    while ( DRDY_read );           //��ʾ������һ�£����ֵ������20ms

    val = Read_ad7705_data();
    return val;

}

uint16_t Read_ad7705_data ( void)
{
	uint16_t i;
	uint16_t read_data;
	//CS=0;
	read_data=0x00;
#if SAD_SIMPLE_TEST
	//�����ݼĴ���
	SEND_CMD (0x39);   
                       
	while ( DRDY_read );           //��ʾ������һ�£����ֵ������20ms
#endif 	
	for(i=0;i<16;i++)
	{
		SCLK_L;//=0;   //׼��ͨ��DOUT�������
		sad_delay(READ_BAND_L);  //���ڼ佫���������ά��
		
		SCLK_H;//=1;  // ת��Ϊ�ߵ�ƽ����֤��ʱ���ϵ����ݲ���ı�

		if(0 == (DOUT_read)) //�������ά�ֵ����ݱ���
		{
			read_data=read_data<<1;
		}
		else
		{
			read_data = read_data<<1;
			read_data = read_data+0x01;		
		}
		
		sad_delay(READ_BAND_H);//������ô��ʱ�����ʱ���ٷſ����ϵ�ά��״̬
	}
	//CS=1;	
	return ( read_data ); 
}
#if SAD_SIMPLE_TEST
void sad_test(void)
{
    static int flag,tmp,c;
    xprintf("sad_test\n");
    while(1)
    {
        for(flag = 0;flag<50;flag++)
        {
            singleAdChSelect(3);
            delay_ms(8);
            Read_ad7705_data();
             singleAdChSelect(27);
             delay_ms(8);
             tmp = Read_ad7705_data();         
        } 
        xprintf("27: %d\n", tmp*5000/65535);
        xprintf("c = %d\n",c++);
        
     }
}
#else
#if 0
void sad_proc(void)
{
    xprintf("sad_proc\n");
    AD7705_start();
    while(1)
    {

          if(SYS_AD_CH_MAX == g_curAdCh)
          {
                AD7705_stop();
                Read_ad7705_end();
                AD7705_start();
          }
            switch (gAdCurSampStatus)
            {
                case AD_SAMP_STEP_START:
                    singleAdChSelect(g_curAdCh+1);
                    gAdCurSampStatus = AD_SAMP_STEP_SEL_DELAY;//AD_SAMP_STEP_CMD;
                    break;
                case AD_SAMP_STEP_SEL_DELAY:
                        delay_ms(8);
                	gAdCurSampStatus = AD_SAMP_STEP_CMD;
                break;
                case AD_SAMP_STEP_CMD:
                	//�����ݼĴ���
                	SEND_CMD (0x39);   
                	gAdCurSampStatus = AD_SAMP_STEP_DRDY;
                    break;
                case AD_SAMP_STEP_DRDY:
                	if (DRDY_read)
                	{
                	       delay_ms(1);
                	        break;
                	}
                	else
                	{
                            batteryVolAdArray[g_curAdCh] = Read_ad7705_data();
                            if(TST_CH_WORK_STATE_TST(g_curAdCh)&&batteryVolAdArray[g_curAdCh] < SET_VOL)
                            {
                                TST_CH_WORK_STATE_CLR(g_cruAdCh);
                            }
                          	g_curAdCh++;
                          	
                          	gAdCurSampStatus = AD_SAMP_STEP_START;
                	}
                    break;
                default:
                    break;
        }
    }
}
#endif
#endif
void AD7705_start(void)
{   
    //gAdSampTmr = ui_mmi_start_timer(WAIT_DRDY_TMR_DELAY,   \
     //              Read_ad7705,  0, 1);
    g_curAdCh = SAMP_START_LINE; //for test
    gAdCurSampStatus = AD_SAMP_STEP_START;
}

void AD7705_stop(void)
{   
    //ui_mmi_stop_timer(gAdSampTmr);
    gAdCurSampStatus = AD_SAMP_STEP_STOP;
    gAdSampTmr = 0;
}

void ReverseDetectPortInit(void)
{
        GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  REV_DETECT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(REV_DETECT_PORT, &GPIO_InitStructure);

}

void ReverseDetectInit(void)
{
    ReverseDetectPortInit();
}
uint8_t getRevLevel(void)
{
    return GPIO_ReadInputDataBit(REV_DETECT_PORT,REV_DETECT_PIN);
}
void scanAndSetChStatus(void)   //ֻ���յ�SQ_START�е���һ��
{
    uint8_t i;
    uint16_t tmp = 0;

 for(i= SAMP_START_LINE+1;i<=SAMP_END_LINE;i++)
 {
        if(IS_SYS_WORKING())  //��ԭ״̬��������Ҫ����ɨ��
        {
            if(TST_CH_WORK_STATE_TST(i-1)&&(TST_AD_SAMP_SEL_TST(i-1)))
            {
                RelayCtrl(i, RELAY_ON);  //����״̬�������̵��������зŵ�
                xprintf("RL_ON:%d\n",i);
                delay_ms(20);
            }
        }
        else        //��һ�����ݵĲɼ�ǰɨ��״̬
        {
            tmp = PollReadAdByCh(i) ;
            //�������õ�ͨ·������Ҫ�ж���״̬,��Ϊͣ�õ�·��������Ϊ�ŵ�����Ҹ���
            if((tmp > gAdSampConfig.mode_vol)&&(TST_AD_SAMP_SEL_TST(i-1)))// 1---100·
            {
                TST_CH_WORK_STATE_SET(i-1);//���÷ŵ���                
                RelayCtrl(i, RELAY_ON);//���ϼ̵��������зŵ�
                xprintf("RL_ON:%d\n",i);
                delay_ms(20);                       //�̵������ٶȲ��˹���
            }
            else if((tmp < 2) && (REV_YES ==  getRevLevel()))
            {
                TST_CH_WARN_STATE_SET(i-1);                                         //����Ϊ����
                ShowAndSetSysStatus(LED_REVERSED, LED_ON);                //һ�㲻���С�
                xprintf("had reversed channel!!\n");
            }
            else if((tmp < 2) && (REV_NO ==  getRevLevel()))
            {
                TST_CH_WARN_STATE_CLR(i-1);                                         //����Ϊ����
                ShowAndSetSysStatus(LED_IMPEND, LED_ON);                        //һ�㲻���С�
                xprintf("had impend channel!!\n");
            }
            else
            {
                TST_CH_WORK_STATE_CLR(i-1);//���÷ŵ����                
                RelayCtrl(i, RELAY_OFF);                    //�Ͽ��̵���
            }
        }
    }
}

        

