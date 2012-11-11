/******************************************************************************
  * CPU:STM32F107	V	C	T	7	
  * V=100Pins(C=48,R=64),	C=256K(8=64),T=LQFP,6=[-40-85](8=-40-125)
*******************************************************************************/ 

#include "stm32f10x.h"
//#include <stdio.h>
//#include <string.h>
//#include "lcd12864.h"	        	//lcd����
#include "input_output.h"		    //�������룬led�ƿ�������� �̵����������
//#include "at45db041.h"			//spi flash����
#include "ad7705.h"			//����ad����
#include "bsp.h"				//�弶����
#include "xprintf.h"

#define GLOBAL                  //����ȫ�ֱ�����δ�д˺궨��ĵط�����Ϊ����
#include "global.h"

//#include "bits_types.h"		    //λ���Ͷ���
//#include "common_define.h"  	//ͨ�ö���
//#include "brush_print.h"		    //����ͼ�λ��ƣ���ˢ����
#include "uimmi_ctrl.h"		    //�û������������
//#include "uikeypad_ctrl.h"		//������Ϣӳ��
//#include "uimmi_content.h"	    //�û�������ʾ����
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


extern void sad_test(void);// ��SAD���Թ���
extern void spi_RW_test(void);
extern void RelayTest(void);
extern void SqTest(void);
extern void bkp_test(void);
int main(void)
{ 	
	int i;		
        BSP_Init();                                             //��ʼ���󲿷ֵ�Ӳ������������usb �����������ʼ��
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
        if(!GetSetCfg())                 //��ȡ���ã���������Ч����ʹ����Щ���ó�ʼ��ȫ�ֱ���    
        {
                xprintf("\nloading default cfg !!\n");
                SpiSysDefCfgInit();                 //��ʼ������spi���ȫ�ֱ���������Ĭ�����ã���д��spi flash�С�
        }
        else                                                    //���Ѿ���Ч����ͬ��bkp�еĲ������ݵ�ad������
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
            if(TABLE_BAK_BKP ==  checkBkp())//�жϲ�ѡ����ǰ��bkp idx.ֻҪ���ù���������Ч
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
        SET_SYS_NO_U_PAN();  //���ϵ�ʱ����δ����ö�٣�ͳͳ��Ϊ��û��u��
        syncResAndStatusShow();
        
        USBH_Init(&USB_OTG_Core,                //usbHost init.�ṩu�̶�д����
#ifdef USE_USB_OTG_FS  
            USB_OTG_FS_CORE_ID,
#else 
            USB_OTG_HS_CORE_ID,
#endif 
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);

        //����u pan.����ʹ��ʱ�������Ƚ�u pan����ϵͳ��
        for(i = 0;i<40000;i++)    
        {
          USBH_Process(&USB_OTG_Core, &USB_Host);                       /* Host Task handler */
          if(!IS_SYS_NO_U_PAN())
          {
            xprintf("have a u pan \n");                             //����u pan�ɹ�
            break;
           }
        }
        ////////////////MMI///////////////////////////
	ui_mmi_open();
	ui_mmi_start();

        //ֻ���ڹ���ʱ������Ҫ��ԭ������
        if(IS_SYS_WORKING() && GetSqLevel())        
        {	
             struct EVENT_NODE_ITEM e;
             xprintf("\nrecover: sys_working && sq_level_h\n" );
             e.sig = EVENT_SYS_HW_SQ_RECOVER;               //����SQ_RECOVER��Ϣ
             ui_mmi_send_msg(&e);
        }
        else if(IS_SYS_END() && GetSqLevel())               //��ʱ��״̬��ϵͳ�Ѿ����������Բ�������ϣ���δȡ�����ʱ����
        {
            xprintf("\n sys_end && sq_level_h,wait sq_start\n");
            ;//��ʱ�����������ȴ������ȡ������ȴ������յ�SQ_END���ֵĹ���.
        }
        else if(IS_SYS_START()&&GetSqLevel())//��ʱ��״̬��ϵͳ����ȡ�����ʱ���磬��ʱ��ƽ��Ϊ�ͣ������ڼ��Ϊ��˵����ͣ����Ѿ��������µ��
        {
             struct EVENT_NODE_ITEM e;
             xprintf("\n sys_start && sq_level_h,send sq_start\n");
             e.sig = EVENT_SYS_HW_SQ_START;                     //����SQ_START//�л������飬�Զ���ʼ
             ui_mmi_send_msg(&e);
        }
        else                                                                            //�͵�ƽ����Ȼ��ʼ����
        {
             xprintf("\n sq_level_l,do nothing\n");
            ;//��ʱ�����������ȴ������װ�룬Ȼ��ʼ
        }
        
       syncResAndStatusShow();
       
       StartSqDetect();            //ʹ��SQ�ź��ⲿ�жϣ����ڸö���������mmi����Ϣ��������ui_mmi_open����ܵ���
	while (1)
	{
		ui_mmi_proc();
                USBH_Process(&USB_OTG_Core, &USB_Host);   /* Host Task handler */
		IWDG_ReloadCounter();  	                                    // Reload IWDG counter
	}
	ui_mmi_close();
}




