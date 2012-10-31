/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

#include "global.h"
#include "input_output.h"


/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[25]  = {0};    //文件名字符串

FATFS fatfs;
FIL file;
uint8_t line_idx = 0;   

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level);
static void     Toggle_Leds(void);
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 

/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
  static uint8_t startup = 0;  
  
  if(startup == 0 )
  {
    startup = 1;
    xprintf("\n ==> USB HOST test<==\n");  
#ifdef USE_USB_OTG_HS 
    xprintf(" USB OTG HS MSC Host\n");
#else
    xprintf(" USB OTG FS MSC Host\n");
#endif

    xprintf("> USB Host library  v2.1.0 started.\n"); 
  }
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
  xprintf("%s\n",MSG_DEV_ATTACHED);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 
  xprintf("%s\n",MSG_UNREC_ERROR); 
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
  /* Set default screen color*/
  xprintf("%s\n",MSG_DEV_DISCONNECTED);
  ShowAndSetSysStatus(LED_NO_U_PAN,LED_ON);
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
   static unsigned char rst_cnt = 0;
  /* callback for USB-Reset */
  xprintf("Reset usb host %d time\n",rst_cnt++);
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
   xprintf("%s\n",MSG_DEV_HIGHSPEED);
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    xprintf("%s\n",MSG_DEV_FULLSPEED);
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
    xprintf("%s\n",MSG_DEV_LOWSPEED);
  }
  else
  {
    xprintf("%s\n",MSG_DEV_ERROR);
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  
  xprintf("VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
  xprintf("PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
    xprintf("%s\n",MSG_MSC_CLASS);
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
    xprintf("%s\n",MSG_HID_CLASS);
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  xprintf("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
  xprintf("Product : %s\n", (char *)ProductString);  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  xprintf( "Serial Number : %s\n", (char *)SerialNumString);    
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
  
  /* Enumeration complete */
  xprintf("%s\n",MSG_DEV_ENUMERATED);
  
  //xprintf( "To see the root content of the disk : \n" );

} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
  xprintf("> Device not supported.\n"); 
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
  usbh_usr_status = USBH_USR_NO_RESP;  

 #if 0
  xprintf("please press any key to send test data\n");

  if(xUSART1_getchar())  //输入任意串口数据?? 
  {
    
    usbh_usr_status = USBH_USR_RESP_OK;  //用户应答OK.
  } 
  #else
  usbh_usr_status = USBH_USR_RESP_OK;  //用户应答OK.
  #endif
  
  return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
  xprintf("Overcurrent detected.\n");
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void) //做成消息响应，响应100路采样完的消息，将其保存
{
  FRESULT res;
  //uint8_t writeTextBuff[] = "  hello i am here...  ";
  uint16_t bytesWritten, bytesToWrite;
  
  switch(USBH_USR_ApplicationState)
  {
      case USH_USR_FS_INIT:   /* Initialises the File System*/
        if ( f_mount( 0, &fatfs ) != FR_OK ) 
        {
              /* efs initialisation fails*/
              xprintf("> Cannot initialize File System.\n");
              return(-1);
        }
        xprintf("> Disk capacity : %d M Bytes\n", USBH_MSC_Param.MSCapacity * \
        USBH_MSC_Param.MSPageLength/1024/1024); 

        if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
        {
            xprintf("%s\n",MSG_WR_PROTECT);
        }
        ShowAndSetSysStatus(LED_NO_U_PAN,LED_OFF);//这时，u pan挂载成功
     
        USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;//USH_USR_FS_READLIST;//USH_USR_FS_WRITEFILE;
        break;
         
      case USH_USR_FS_READLIST:        //列出文件列表
        Explore_Disk("0:/", 1);                     
        line_idx = 0;   
        USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;    
        break;

      case USH_USR_FS_WRITEFILE:        ////////////////写文件,即文件保存
        //USB_OTG_BSP_mDelay(100);   //100 ms延时，由于我们一次保存间隔较长，这里可不用延时
#if 0
        if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
        {
              xprintf( "> write protected \n");                   ////////加上一个判断，显示在Led上。
              ShowAndSetSysStatus(LED_WRITE_PROTECTED,LED_ON);
              break;
        }   
#endif //不判断写保护

 #if 1       
        //创建及打开文件
        
#else
        if(f_open(&file, "0:2012-11.txt",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)	   //0:STM32.TXT
        { 
          /* Write buffer to file */
          xprintf("write content to file\n");
          bytesToWrite = sizeof(writeTextBuff); 
          
          res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);   
          
          if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
          {
            xprintf("> file CANNOT be writen.\n");
          }
          else
          {
            xprintf("> file created\n");   //文件已经存在
          }
          
          /*close file and filesystem*/
          f_close(&file);
          
          f_mount(0, NULL);           //使0 lun 指向空，防止误操作。
        }
        
        else
        {
          xprintf ("> file Had created in the disk\n");
        }
    	
        
#endif
        USBH_USR_ApplicationState = 5;//USH_USR_FS_WRITEFILE;//只写文件一件事
        break;
        
      default: 
      	break;
  }
  return(0);
}

/**
* @brief  Explore_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level)
{

  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;
  char tmp[14];
  
  res = f_opendir(&dir, path);
  if (res == FR_OK) {
    while(HCD_IsDeviceConnected(&USB_OTG_Core)) 
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) 
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }

      fn = fno.fname;
      strcpy(tmp, fn); 

      line_idx++;
      if(line_idx > 9)
      {
        line_idx = 0;

        xprintf("%s\n","Press any key to read the root path...");
        xUSART1_getchar();
        /*
        while((HCD_IsDeviceConnected(&USB_OTG_Core))&& \
          (xUSART1_getchar())) 
        {
          xprintf("##");
          Toggle_Leds();          
        }
        */
      } 
      
      if(recu_level == 1)
      {
        xprintf("   |__\n");
      }
      else if(recu_level == 2)
      {
        xprintf("   |   |__\n");
      }
      if((fno.fattrib & AM_MASK) == AM_DIR)
      {
        strcat(tmp, "\n"); 
        xprintf("%s\n",(char *)tmp);
      }
      else
      {
        strcat(tmp, "\n"); 
        xprintf("%s\n",(char *)tmp);
      }

      if(((fno.fattrib & AM_MASK) == AM_DIR)&&(recu_level == 1))
      {
        Explore_Disk(fn, 2);
      }
    }
  }
  return res;
}


/**
* @brief  Toggle_Leds
*         Toggle leds to shows user input state
* @param  None
* @retval None
*/
static void Toggle_Leds(void)
{
  static uint32_t i;
  if (i++ == 0x10000)
  {
    i = 0;
  }  
}
/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

