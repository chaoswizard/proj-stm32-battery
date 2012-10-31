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


#if SAD_SIMPLE_TEST
extern void sad_test(void);// 简单SAD测试过程
#else
extern void sad_proc(void);//仿真主流程的一段测试代码。
#endif
extern void spi_RW_test(void);
extern void RelayTest(void);
extern void SqTest(void);
extern void bkp_test(void);
int main(void)
{ 	
	int i;		
	static int ch = 0;
        BSP_Init();                                             //初始化大部分的硬件环境，除了usb 在主过程里初始化
       	xprintf("start system\n");
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
#define BKP_PROTECT_WORD  0xAABB
         if (BKP_ReadBackupRegister(BKP_DR22) != BKP_PROTECT_WORD)
         {
            xprintf("loading default bkp !!\n");
            bkpSysDefBakInit();
            BKP_WriteBackupRegister(BKP_DR22, 0xAAAA);
         }
         else
        {
            if(TABLE_BAK_BKP ==  checkBkp())//判断并选定当前的bkp idx.只要配置过，不会无效
            {
                SaveBkpData(TABLE_BKP);
            }
            else
            {
                SaveBkpData(TABLE_BAK_BKP);    
            }
        }
        
        if(!GetSetCfg())                 //读取配置，若配置有效，则使用这些配置初始化全局变量    
        {
                xprintf("loading default cfg !!\n");
                SpiSysDefCfgInit();                 //初始化放在spi里的全局变量，设置默认配置，并写回spi flash中。
        }
        else                                                    //若已经有效，则同步bkp中的部分内容到ad配置中
        {
            gAdSampConfig.data_group_valid_line_idx[g_bkpData.group_idx] = g_bkpData.last_line_in_group; //其实一般，左边不用到
        }

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

////////////////MMI///////////////////////////
	ui_mmi_open();
	ui_mmi_start();
        StartSqDetect();            //使能SQ信号外部中断，由于该动作将调用mmi的消息处理，故在ui_mmi_open后才能调用
        
        if(IS_SYS_WORKING() && GetSqLevel())//已经连接上，为高电平，且状态为工作中，则主动发送SQ开始信号
        {
            struct EVENT_NODE_ITEM e;
             e.sig = EVENT_SYS_HW_SQ_START;  //自动还原运行
             ui_mmi_send_msg(&e);

             //若显示掉电前有u盘，则尝试挂载之
             if(!IS_SYS_NO_U_PAN())
             {
                SET_SYS_NO_U_PAN();
                for(i = 0;i<30000;i++)    //看下能否挂载成功,若这个循环下来，还未能挂载成功，则说明u盘不存在
                {
                  USBH_Process(&USB_OTG_Core, &USB_Host);/* Host Task handler *///进行u pan挂载
                  if(!IS_SYS_NO_U_PAN())
                  {
                    xprintf("have u pan\n");
                    break;
                   }
                }
            }
        }
	while (1)
	{
		ui_mmi_proc();
                USBH_Process(&USB_OTG_Core, &USB_Host);/* Host Task handler */
		IWDG_ReloadCounter();  	// Reload IWDG counter
	}
	ui_mmi_close();
}




