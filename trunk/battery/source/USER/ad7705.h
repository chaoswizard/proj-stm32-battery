#ifndef __AD7705_H
#define __AD7705_H

#define WRITE_BAND_H  1000
#define WRITE_BAND_L WRITE_BAND_H

#define READ_BAND_H  1000
#define READ_BAND_L READ_BAND_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Private define ------------------------------------------------------------*/

#define SCLK_H          GPIOB->BSRR = GPIO_Pin_2  /* GPIO_SetBits(GPIOB , GPIO_Pin_6)   */
#define SCLK_L          GPIOB->BRR  = GPIO_Pin_2   /* GPIO_ResetBits(GPIOB , GPIO_Pin_6) */
   
#define DIN_H           GPIOE->BSRR = GPIO_Pin_8  /* GPIO_SetBits(GPIOB , GPIO_Pin_7)   */
#define DIN_L           GPIOE->BRR  = GPIO_Pin_8  /* GPIO_ResetBits(GPIOB , GPIO_Pin_7) */
#define DOUT_read       (GPIOE->IDR  & GPIO_Pin_10) /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) */
#define DRDY_read       (GPIOE->IDR  & GPIO_Pin_12)  /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) */

/* Private function prototypes -----------------------------------------------*/
void AD7705_configuration(void);
void AD7705_reset(void);               //DIN³¤Ê±
void AD7705_inti (void);


uint16_t Read_ad7705_data ( void);


#endif//__AD7705_H


