
#include "stm32f10x.h"

#include "stm32f10x_conf.h"

#include "AD7705.H"
#include "global.h"
#include "uimmi_ctrl.h"
#include "input_output.h"

#include "usbh_usr.h"
#include "ff.h"


#if 1
#define AD7705_DEBUG  xprintf
#else
#define AD7705_DEBUG
#endif

#define WAIT_DRDY_TMR_DELAY  20
static SWTMR_NODE_HANDLE gAdSampTmr;
static volatile uint8_t gAdCurSampStatus = 0;

#define SAMP_END_LINE        100//s100       //����������+1��λ��
#define SAMP_START_LINE        0//������ʼ��

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
	//�ٶ��ⲿʱ��Ƶ�� 4.9512MHZ,������ѡΪ500hz
	/*ZERO (0) ZERO (0) ZERO (0) CLKDIS (0) CLKDIV (0) CLK (1) FS1 (0) FS0 (1)*/
	write_ad7705reg(0xc);//(0xe); //0xf
	
	// 3 ��ͨ�żĴ���д���ݣ�ѡ��ͨ��2��Ϊ��Чͨ��������һ
	//������Ϊ�����üĴ�����д������10b---- 100HZ; 11b---200Hz
	SEND_CMD(0x11);  //0x12 ---100hz,0x13---200hz,Ŀǰֻ����������ȶ��Ը�
                
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
uint8_t isDetectEnd(void)
{
    static uint16_t tmCnt = 0;
    uint8_t ret = 1;
    uint8_t i = 0;
    //return 0;
    tmCnt++;
    g_bkpData.had_used_time = SYS_AD_PERIOD*tmCnt;
    
    if(0 == gAdSampConfig.mode_sel)                                     //��ʱ��
    {
           if(gAdSampConfig.mode_min*60 <= g_bkpData.had_used_time )
           {
                tmCnt = 0;
                g_bkpData.had_used_time = 0;

                ret = 1;
                AD7705_DEBUG("\n!!end by time!!\n");
           }
           else
           {
                ret = 0;
           }
    }
    else if(1 == gAdSampConfig.mode_sel) //����ѹ
    {
        //���õ�ͨ·�Ҳ��ǲ��ŵ��ͨ·����ѹ��������ֵ����
        for(i=0;i<(SYS_AD_CH_MAX+7)>>3;i++)//ȫ���ŵ����,��ʼʱ�����գ����򣬲��ŵ磬����Ϊ�ŵ����
        {
            if(g_bkpData.ch_work_status[i])   //ֻҪ��һ����Ϊ�㣬��δ����
            {
                ret = 0;
                break;
            }
        }
        if(ret)
        {
                AD7705_DEBUG("\n!!end by vol!!\n");
         }
    }
    else   //���ģʽ
    {
        for(i=0;i<(SYS_AD_CH_MAX+7)>>3;i++)//ȫ���ŵ����,��ʼʱ�����գ����򣬲��ŵ磬����Ϊ�ŵ����
        {
            if(g_bkpData.ch_work_status[i])
            {
                ret = 0;
                break;
            }
        }
        if(ret)
        {   
               AD7705_DEBUG("\n!!end by mis vol!!\n");
        }
       else   if(gAdSampConfig.mode_min*60 <= g_bkpData.had_used_time )
       {
            tmCnt = 0;
            g_bkpData.had_used_time = 0;

            ret = 1;
            AD7705_DEBUG("\n!!end by mis time!!\n");
       }
    }
    return ret;
}
//���һ�����ݲ�������øú������ܶ�״̬���µĹ����ڴ˽���
//�ú���Ϊ���һ�����ݲ���������ݴ�����̣�����������Զ�
static u_int16 batteryVolAdArray_src[SYS_AD_CH_MAX] ; 
#define CMP_SRC_LINE  1  //ȡ��һ������
#define CMP_DST_LINE  5  //�õ��������һ�����
void ad7705_dump(uint16_t grp)
{
    uint8_t i;
     if(isDetectEnd())               
     {
        RelayAllDown();   //�ر����м̵�����ʱ�䵽�Ľ�����δ�ػ�ر����еļ̵���
        //��u�̻��������������flush.�ر��ļ�
        if(!IS_SYS_NO_U_PAN()) 
        {
            AD7705_DEBUG("group end,using u pan,close file\n");
            //����һ��ָ��������沢�ر��ļ�,��ֱ�ӹرգ����ٱ���һ������
            f_close(&file);            //δ��ɼ��������ǿ�ƽ����˷ŵ����
        }              
        else
        {
            AD7705_DEBUG("save to spi flash\n");
            SaveAdData((uint8_t*)batteryVolAdArray);//���浽spi flash ��
        } 
        //MoveToNextGroup(); 
         SET_SYS_RUN_STATUS_END();   //��״̬Ϊ�����ɣ��ȴ������
     }
     
     else                                                           //  ��ʼ��һ�еĲ���
     {
        if(IS_SYS_WORKING())                  //��ԭ����ĿǰΪ�򵥴�����ԭʱ�½��ļ�
        {
             if(!IS_SYS_NO_U_PAN() ) 
            {
                AD7705_DEBUG("save data to u-pan\n");
                
                memcpy(writeTextBuff,(uint8_t*)batteryVolAdArray, SYS_AD_CH_MAX); //���Ƶ�usb usr buffer��
                
                Set_USBH_USR_MSC_App_Step(USH_USR_FS_WRITEFILE); //�������ݵ�u pan.
            }
            else
            {
                AD7705_DEBUG("save data to spi_flash\n");
                SaveAdData((uint8_t*)batteryVolAdArray);//���浽spi flash ��
            }
            
            if(CMP_SRC_LINE == g_bkpData.last_line_in_group)
            {
                memcpy((uint8_t*)batteryVolAdArray_src,(uint8_t*)batteryVolAdArray, SYS_AD_CH_MAX); //���Ƶ�usb usr buffer��
            }
            else if(CMP_DST_LINE == g_bkpData.last_line_in_group)
            {
                AD7705_DEBUG("discharge judge\n");
                for(i = 0;i< SYS_AD_CH_MAX;i++)
                {
                    if(CH_STATUS_NORMAL== ch_warn_status[i] && (batteryVolAdArray_src[i] <= batteryVolAdArray[i]))
                    {
                        ch_warn_status[i] = CH_STATUS_NO_DISCHARGED;
                        TST_CH_WORK_STATE_CLR(i);                         //���÷ŵ����    
                         ShowAndSetSysStatus(LED_NOT_DISCHARGED, LED_ON);               
                    }
                }
            }
            MoveToNextLine();
            SaveBkpData(TABLE_BKP);
            SaveBkpData(TABLE_BAK_BKP);        
            AD7705_start();                                     //������һ�в���
        }
        else
        {
            AD7705_DEBUG("not yet working\n");
        }
     }
}

#define AD_SAMP_STEP_SEL_DELAY_CNT 13
void Read_ad7705(void *p)
{
    static uint8_t delay_cnt;
      if (AD_SAMP_STEP_STOP == gAdCurSampStatus )
      {
            return;
      }
      
      if(SAMP_END_LINE == g_curAdCh) 
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
            	gAdCurSampStatus = AD_SAMP_STEP_DRDY;//AD_SAMP_STEP_DRDY_DELAY;
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
                          if((batteryVolAdArray[g_curAdCh] < g_mode_vol_ad_val)&&TST_CH_WORK_STATE_TST(g_curAdCh))
                        {
                            TST_CH_WORK_STATE_CLR(g_curAdCh);    //�����
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
    delay_ms(10);   //6

    //�����ݼĴ���
    SEND_CMD (0x39);   

    while ( DRDY_read );           //��ʾ������һ�£����ֵ������20ms

    val = Read_ad7705_data();
    return val;

}
uint16_t Read_ad7705_data_test ( uint8_t ch)
{
	uint16_t i;
	uint16_t read_data;
	//CS=0;
	read_data=0x00;

	singleAdChSelect(ch);
        //����cpu��ʱ
        delay_ms(10);    //6
        
	//�����ݼĴ���
	SEND_CMD (0x39);   
                       
	while ( DRDY_read );           //��ʾ������һ�£����ֵ������20ms

	for(i=0;i<16;i++)
	{
		SCLK_L;             //=0;   //׼��ͨ��DOUT�������
		sad_delay(READ_BAND_L);  //���ڼ佫���������ά��
		
		SCLK_H;         //=1;  // ת��Ϊ�ߵ�ƽ����֤��ʱ���ϵ����ݲ���ı�

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

uint16_t Read_ad7705_data ( void)
{
	uint16_t i;
	uint16_t read_data;
	//CS=0;
	read_data=0x00;
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

void sad_test(void)
{
    static int flag,tmp,c;
    xprintf("sad_test\n");
    while(1)
    {
        for(flag = 0;flag<50;flag++)
        {
              tmp = Read_ad7705_data_test(3);
              Read_ad7705_data_test(27);

        } 
        xprintf("3 : %d\n", tmp*5000/65535);
        xprintf("c = %d\n",c++);
        
     }
}
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

    if(IS_SYS_WORKING())                              //��ԭ״̬��������Ҫ����ɨ��,��ʱ�ľ�����Ϣ�����ܻ�ԭ
    {
         for(i= SAMP_START_LINE+1;i<=SAMP_END_LINE;i++)
        {
            if(TST_CH_WORK_STATE_TST(i-1)&&(TST_AD_SAMP_SEL_TST(i-1)))
            {
                RelayCtrl(i, RELAY_ON);                         //����״̬�������̵��������зŵ�
                AD7705_DEBUG("wch[%d]--RL_ON\n",i);
                delay_ms(10);
            }
        }
    }
    else                                                        //��һ�����ݵĲɼ�ǰɨ��״̬
    {
           //�ڷǻ�ԭ�����У��Ƚ�����������״̬��ʼ����Ȼ��ȴ��ж�
          g_bkpData.had_used_time = 0;                          //�ŵ��Ѿ���ʱΪ��                 
         TST_CH_WORK_STATE_ALL_CLR();       //�̵���״̬ȫ��Ϊ��
         TST_RELAY_SEL_ALL_CLR();
         CLR_SYS_WARN_STATUS();                         //�������
         
         for(i= SAMP_START_LINE+1;i<=SAMP_END_LINE;i++)
        {
                tmp = PollReadAdByCh(i) ;
                AD7705_DEBUG("ch[%d] = %d \n",i,tmp);
                
                //�������õ�ͨ·������Ҫ�ж���״̬,��Ϊͣ�õ�·��������Ϊ�ŵ�����Ҹ���
                if((tmp > g_mode_vol_ad_val)&&(TST_AD_SAMP_SEL_TST(i-1)))// 1---100·
                {               
                    ch_warn_status[i-1] = CH_STATUS_NORMAL;
                    TST_CH_WORK_STATE_SET(i-1);             //���÷ŵ���          
                    RelayCtrl(i, RELAY_ON);                                 //���ϼ̵��������зŵ�
                    AD7705_DEBUG("ch[%d]--RL_ON\n",i);
                    delay_ms(10);                                                   //�̵������ٶȲ��˹���
                }
                else if((tmp < g_param_rervse_ad_val) && (REV_YES ==  getRevLevel()))
                {
                    ch_warn_status[i-1] = CH_STATUS_REVERSED;                      //����Ϊ����
                    TST_CH_WORK_STATE_CLR(i-1);                                      //���÷ŵ����
                    ShowAndSetSysStatus(LED_REVERSED, LED_ON);                  //һ�㲻���С�
                    AD7705_DEBUG("ch[%d]--reversed!!\n",i);
                }
                else if((tmp < g_param_limit_ad_val) && (REV_NO ==  getRevLevel()))
                {
                    ch_warn_status[i-1] = CH_STATUS_IMPEND;                             //����Ϊ����
                    TST_CH_WORK_STATE_CLR(i-1);                                             //���÷ŵ����
                    ShowAndSetSysStatus(LED_IMPEND, LED_ON);                        //һ�㲻���С�
                    AD7705_DEBUG("ch[%d]--impend!!\n",i);
                }
                else
                {                
                    ch_warn_status[i-1] = CH_STATUS_NORMAL;
                    TST_CH_WORK_STATE_CLR(i-1);                         //���÷ŵ����    
                    AD7705_DEBUG("ch[%d]--RL_OFF!!\n",i);
                    //RelayCtrl(i, RELAY_OFF);                                            //�Ͽ��̵���
                }
        }
    }
}

        

