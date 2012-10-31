
#include "stm32f10x.h"

#include "stm32f10x_conf.h"

#include "AD7705.H"
#include "global.h"
#include "uimmi_ctrl.h"
#include "input_output.h"

#define WAIT_DRDY_TMR_DELAY  20
static SWTMR_NODE_HANDLE gAdSampTmr;
static volatile uint8_t gAdCurSampStatus = 0;

#define SAMP_END_LINE        100//s100       //采样结束行+1的位置
#define SAMP_START_LINE        0//采样开始行
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
		write_ad7705reg(0xff);		//DIN连续多于32个1，则器件复位
	}
} 

void AD7705_init(void)
{
        AD7705PortInit();
	AD7705_reset();
	
	// 1  向通信寄存器写数据，选择通道2
	//作为有效通道，将下一操作设为对时钟寄存器进行写操作
	SEND_CMD(0x21);
	// 2对时钟寄存器写操作，设置CLKDIV位，将外部时钟除二，
	//假定外部时钟频率 4.9512MHZ,更新率选为100hz
	write_ad7705reg(0x0c);
	
	// 3 向通信寄存器写数据，选择通道2作为有效通道。将下一
	//操作设为对设置寄存器的写操作。10b---- 100HZ; 11b---200Hz
	SEND_CMD(0x11);  //0x12 ---100hz,0x13---200hz,目前只有这个配置稳定性高
	/*ZERO (0) ZERO (0) ZERO (0) CLKDIS (0) CLKDIV (0) CLK (1) FS1 (0) FS0 (1)*/
	//SEND_CMD(0x1D);  //抽空测一下这个  ,60HZ,若改为此，则可以30ms完成一路
	//那么3s将可以完成一路的采样，否则，则需要将近3.5s才能完成采样
	//其它如spi flash,usb,bkp等的修改时间只剩下500ms了，这个可能并不足以存下
	//一轮的数据xxxx。
	//SEND_CMD(0x17);  //这个也需要4s左右稳定性好像比0x11要高
        //SEND_CMD(0x16);  //这个也需要4s左右,稳定性好像比0x11要高
        //SEND_CMD(0x15);  //这个也需要4s左右稳定性好像比0x11要高-->最高
        //SEND_CMD(0x14);  //这个也需要4s左右，稳定性好像比0x11要高
        //SEND_CMD(0x13);  //这个也需要4s左右，稳定性好像比0x11要高
        //SEND_CMD(0x19);  //这个也需要4s左右，xxxxx不行
                
	// 4向设置寄存器写数据，将增益设为1，设置为单极性、非缓冲模式
	// 清除滤波器同步，开始自校准
	write_ad7705reg(0x44);
	
	// 5,查询~DRDY输出
	//while ( DRDY_read );	
	// 6 从数据寄存器上读数据
	// 7跳回第5，第6步，直到从选定的通道中取出指定数量的采样
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

//完成一行数据采样后调用该函数，很多状态更新的工作在此进行
//该函数为完成一行数据采样后的数据处理过程，这里的内容稍多
void ad7705_dump(uint16_t grp)
{
    int i;
    //xprintf("\n\n=========================\n");
    //     while(g_curAdCh--)
    //    xprintf("--%d: %d-%d-\n",g_curAdCh ,batteryVolAdArray[g_curAdCh], grp);
    if(IS_SYS_NO_U_PAN() )
    {
        //保存到spi flash 中
        SaveAdData((uint8_t*)batteryVolAdArray);

    }
    else
    {
        //保存数据到u pan.
        
        xprintf("save data to u-pan\n");
    }

     if(0)                //gAdSampConfig.mode_sel结束条件达到，置系统运行状态为"结束"，则不需要再启动ad采样   等待SQ_END.
     {
        SET_SYS_RUN_STATUS_END();
        
        //将u盘或其它外设的数据flush.关闭文件
        
        MoveToNextGroup();                          //切到下一组，开始.将它们同步的信息同步到备份数据区
     }
     else               //  开始下一行的采样
     {
      
        MoveToNextLine();
        AD7705_start();           //启动下一行采样
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
            AD7705_stop();                         //采满指定的一行，这里在100路为一行，
            Read_ad7705_end();                //发出完成一行的消息，等待处理，处理过后视条件重新打开
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
                if(delay_cnt >= AD_SAMP_STEP_SEL_DELAY_CNT)                              //该值连12都不稳定，只能用13
                {
                    delay_cnt = 0;
            	    gAdCurSampStatus = AD_SAMP_STEP_CMD;
            	 }
                break;
            case AD_SAMP_STEP_CMD:
            	//读数据寄存器
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

                        //若之前是正在放电中，则这里设置为放电结束,并断开继电器
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

    //加上cpu延时
    delay_ms(10);

    //读数据寄存器
    SEND_CMD (0x39);   

    while ( DRDY_read );           //拿示波器测一下，这个值的周期20ms

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
	//读数据寄存器
	SEND_CMD (0x39);   
                       
	while ( DRDY_read );           //拿示波器测一下，这个值的周期20ms
#endif 	
	for(i=0;i<16;i++)
	{
		SCLK_L;//=0;   //准备通过DOUT输出数据
		sad_delay(READ_BAND_L);  //此期间将数据输出并维持
		
		SCLK_H;//=1;  // 转变为高电平，保证这时线上的数据不会改变

		if(0 == (DOUT_read)) //将输出并维持的数据保存
		{
			read_data=read_data<<1;
		}
		else
		{
			read_data = read_data<<1;
			read_data = read_data+0x01;		
		}
		
		sad_delay(READ_BAND_H);//经过这么长时间的延时后，再放开线上的维持状态
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
                	//读数据寄存器
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
void scanAndSetChStatus(void)   //只在收到SQ_START中调用一次
{
    uint8_t i;
    uint16_t tmp = 0;

 for(i= SAMP_START_LINE+1;i<=SAMP_END_LINE;i++)
 {
        if(IS_SYS_WORKING())  //还原状态，而不需要重新扫描
        {
            if(TST_CH_WORK_STATE_TST(i-1)&&(TST_AD_SAMP_SEL_TST(i-1)))
            {
                RelayCtrl(i, RELAY_ON);  //根据状态，操作继电器，进行放电
                xprintf("RL_ON:%d\n",i);
                delay_ms(20);
            }
        }
        else        //新一组数据的采集前扫描状态
        {
            tmp = PollReadAdByCh(i) ;
            //对于在用的通路，才需要判断其状态,因为停用的路，都设置为放电结束且高亮
            if((tmp > gAdSampConfig.mode_vol)&&(TST_AD_SAMP_SEL_TST(i-1)))// 1---100路
            {
                TST_CH_WORK_STATE_SET(i-1);//设置放电中                
                RelayCtrl(i, RELAY_ON);//合上继电器，进行放电
                xprintf("RL_ON:%d\n",i);
                delay_ms(20);                       //继电器打开速度不宜过快
            }
            else if((tmp < 2) && (REV_YES ==  getRevLevel()))
            {
                TST_CH_WARN_STATE_SET(i-1);                                         //设置为反向
                ShowAndSetSysStatus(LED_REVERSED, LED_ON);                //一般不会有。
                xprintf("had reversed channel!!\n");
            }
            else if((tmp < 2) && (REV_NO ==  getRevLevel()))
            {
                TST_CH_WARN_STATE_CLR(i-1);                                         //设置为悬空
                ShowAndSetSysStatus(LED_IMPEND, LED_ON);                        //一般不会有。
                xprintf("had impend channel!!\n");
            }
            else
            {
                TST_CH_WORK_STATE_CLR(i-1);//设置放电结束                
                RelayCtrl(i, RELAY_OFF);                    //断开继电器
            }
        }
    }
}

        

