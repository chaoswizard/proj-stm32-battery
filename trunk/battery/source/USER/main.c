/******************************************************************************
  * CPU:STM32F107	V	C	T	7	
  * V=100Pins(C=48,R=64),	C=256K(8=64),T=LQFP,6=[-40-85](8=-40-125)
*******************************************************************************/ 

#include "stm32f10x.h"
//#include <stdio.h>
//#include <string.h>
//#include "lcd12864.h"	        	//lcd驱动
#include "input_output.h"		    //按键读入，led灯控制输出， 继电器控制输出
//#include "at45db041.h"			//spi flash驱动
#include "ad7705.h"			//串行ad驱动
#include "bsp.h"				//板级启动
#include "xprintf.h"

#define GLOBAL                  //定义全局变量，未有此宏定义的地方，则为声明
#include "global.h"

//#include "bits_types.h"		    //位类型定义
//#include "common_define.h"  	//通用定义
//#include "brush_print.h"		    //基本图形绘制，画刷函数
#include "uimmi_ctrl.h"		    //用户交互界面控制
//#include "uikeypad_ctrl.h"		//按键消息映射
//#include "uimmi_content.h"	    //用户界面显示内容
//#include "uimenu_ctrl.h"
//#include "gui_menu_item.h"
//#include "gui_osd_obj.h"
//#include "ui_com_obj.h"

//RTC
#include "rtc_time.h"

/*USB Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;
/**
* @}
*/ 


extern void sad_test(void);// 简单SAD测试过程
extern void spi_RW_test(void);
extern void RelayTest(void);
extern void SqTest(void);
extern void bkp_test(void);
int main(void)
{ 	
	int i;		
        BSP_Init();                                             //初始化大部分的硬件环境，除了usb 在主过程里初始化
       	xprintf("\n\n^v^start system^v^\n\n");
	CHECK_VER_RIGHT(); 
 #if 0  
         bkp_test();
	//SqTest();
	#if SAD_SIMPLE_TEST
        sad_test();
        #else
        //sad_proc();
        #endif
        //spi_RW_test();
	//RelayTest();	
	
#if 0   //usb   test      
	  USBH_Init(&USB_OTG_Core, 
#ifdef USE_USB_OTG_FS  
            USB_OTG_FS_CORE_ID,
#else 
            USB_OTG_HS_CORE_ID,
#endif 
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);
  
  while (1)
  {
    /* Host Task handler */
    USBH_Process(&USB_OTG_Core, &USB_Host);
    if (i++ == 0x10000)
    {
      i = 0;
    }      
  }
#endif
#endif        
        if(IS_SYS_MANUAL())
        {
            SET_SYS_AUTO();
        }
        if(!GetSetCfg())                 //读取配置，若配置有效，则使用这些配置初始化全局变量    
        {
                xprintf("\nloading default cfg !!\n");
                SpiSysDefCfgInit();                 //初始化放在spi里的全局变量，设置默认配置，并写回spi flash中。
        }
        else                                                    //若已经有效，则同步bkp中的部分内容到ad配置中
        {
            xprintf("\nget cfg from spi_flash!!\n");
        }

 #define BKP_PROTECT_WORD  0xAABB
 
         if (BKP_ReadBackupRegister(BKP_DR22) != BKP_PROTECT_WORD)
         {
            xprintf("loading default bkp !!\n");
            bkpSysDefBakInit();
            BKP_WriteBackupRegister(BKP_DR22, BKP_PROTECT_WORD);
         }
         else
        {
            if(TABLE_BAK_BKP ==  checkBkp())//判断并选定当前的bkp idx.只要配置过，不会无效
            {
                xprintf("\nnow table bak bkp valid !!\n");
                SaveBkpData(TABLE_BKP);
            }
            else
            {
                xprintf("\nnow table bkp valid !!\n");
                SaveBkpData(TABLE_BAK_BKP);    
            }
        }
        SET_SYS_NO_U_PAN();  //刚上电时，尚未进行枚举，统统认为是没有u盘
        syncResAndStatusShow();
        
        USBH_Init(&USB_OTG_Core,                //usbHost init.提供u盘读写服务
#ifdef USE_USB_OTG_FS  
            USB_OTG_FS_CORE_ID,
#else 
            USB_OTG_HS_CORE_ID,
#endif 
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);

        //挂载u pan.建议使用时，我们先将u pan接入系统中
        for(i = 0;i<40000;i++)    
        {
          USBH_Process(&USB_OTG_Core, &USB_Host);                       /* Host Task handler */
          if(!IS_SYS_NO_U_PAN())
          {
            xprintf("have a u pan \n");                             //挂载u pan成功
            break;
           }
        }
        ////////////////MMI///////////////////////////
	ui_mmi_open();
	ui_mmi_start();

        //只有在工作时，才需要还原继续。
        if(IS_SYS_WORKING() && GetSqLevel())        
        {	
             struct EVENT_NODE_ITEM e;
             xprintf("\nrecover: sys_working && sq_level_h\n" );
             e.sig = EVENT_SYS_HW_SQ_RECOVER;               //发出SQ_RECOVER消息
             ui_mmi_send_msg(&e);
        }
        else if(IS_SYS_END() && GetSqLevel())               //这时的状态是系统已经按条件测试并保存完毕，尚未取出电池时掉电
        {
            xprintf("\n sys_end && sq_level_h,wait sq_start\n");
            ;//这时，不动作，等待将电池取出，或等待运行收到SQ_END部分的过程.
        }
        else if(IS_SYS_START()&&GetSqLevel())//这时的状态是系统正在取出电池时掉电，这时电平本为低，但现在检测为高说明在停电后，已经换好了新电池
        {
             struct EVENT_NODE_ITEM e;
             xprintf("\n sys_start && sq_level_h,send sq_start\n");
             e.sig = EVENT_SYS_HW_SQ_START;                     //发出SQ_START//切换到新组，自动开始
             ui_mmi_send_msg(&e);
        }
        else                                                                            //低电平，自然开始即可
        {
             xprintf("\n sq_level_l,do nothing\n");
            ;//这时，不动作，等待将电池装入，然后开始
        }
        
       syncResAndStatusShow();
       
       StartSqDetect();            //使能SQ信号外部中断，由于该动作将调用mmi的消息处理，故在ui_mmi_open后才能调用
	while (1)
	{
		ui_mmi_proc();
                USBH_Process(&USB_OTG_Core, &USB_Host);   /* Host Task handler */
		IWDG_ReloadCounter();  	                                    // Reload IWDG counter
	}
	ui_mmi_close();
}




