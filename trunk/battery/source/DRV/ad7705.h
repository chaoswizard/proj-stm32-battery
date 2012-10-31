#ifndef __AD7705_H
#define __AD7705_H

#define SAD_SIMPLE_TEST  0

#define AD_PORT  GPIOE
#define SCLK_PIN  GPIO_Pin_9
#define DIN_PIN   GPIO_Pin_10
#define DOUT_PIN GPIO_Pin_11
#define DRDY_PIN GPIO_Pin_12

#define WRITE_BAND_H  1
#define WRITE_BAND_L WRITE_BAND_H

#define READ_BAND_H  1
#define READ_BAND_L READ_BAND_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Private define ------------------------------------------------------------*/

#define SCLK_H          AD_PORT->BSRR = SCLK_PIN 
#define SCLK_L          AD_PORT->BRR  = SCLK_PIN   
#define DIN_H           AD_PORT->BSRR = DIN_PIN  
#define DIN_L           AD_PORT->BRR  = DIN_PIN  
#define DOUT_read       (AD_PORT->IDR  & GPIO_Pin_11) 
#define DRDY_read       (AD_PORT->IDR  & GPIO_Pin_12)  


#define REV_DETECT_PORT  GPIOA
#define REV_DETECT_PIN  GPIO_Pin_6
#define REV_YES   0
#define REV_NO    1

/* Private function prototypes -----------------------------------------------*/
void AD7705PortInit(void);
void AD7705_reset(void);               //DINʱ
void AD7705_init (void);
uint16_t Read_ad7705_data ( void);
void AD7705_start(void);
void AD7705_stop(void);
void scanAndSetChStatus(void);



void ReverseDetectPortInit(void);
void ReverseDetectInit(void);
uint8_t getRevLevel(void);


#endif//__AD7705_H


