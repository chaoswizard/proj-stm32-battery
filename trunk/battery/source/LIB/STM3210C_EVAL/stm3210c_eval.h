/**
  ******************************************************************************
  * @file    stm3210c_eval.h
  * @author  MCD Application Team
  * @version V5.0.1
  * @date    05-March-2012
  * @brief   This file contains definitions for STM3210C_EVAL's Leds, push-buttons
  *          COM ports, SD Card on SPI and sEE on I2C hardware resources.
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM3210C_EVAL_H
#define __STM3210C_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */
    
/** @addtogroup STM3210C_EVAL
  * @{
  */ 

/** @addtogroup STM3210C_EVAL_LOW_LEVEL
  * @{
  */ 
  
/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Types
  * @{
  */
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;

typedef enum 
{  
  BUTTON_WAKEUP = 0,
  BUTTON_TAMPER = 1,
  BUTTON_KEY = 2,
  BUTTON_RIGHT = 3,
  BUTTON_LEFT = 4,
  BUTTON_UP = 5,
  BUTTON_DOWN = 6,
  BUTTON_SEL = 7
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;


/**
  * @}
  */ 

/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** 
  * @brief  Define for STM3210C_EVAL board  
  */ 
#if !defined (USE_STM3210C_EVAL)
 #define USE_STM3210C_EVAL
#endif

/** @addtogroup STM322xG_EVAL_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_Pin_2
#define LED1_GPIO_PORT                   GPIOD
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOD  
  
#define LED2_PIN                         GPIO_Pin_3
#define LED2_GPIO_PORT                   GPIOD
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOD  

#define LED3_PIN                         GPIO_Pin_4  
#define LED3_GPIO_PORT                   GPIOD
#define LED3_GPIO_CLK                    RCC_APB2Periph_GPIOD  

#define LED4_PIN                         GPIO_Pin_7  
#define LED4_GPIO_PORT                   GPIOD
#define LED4_GPIO_CLK                    RCC_APB2Periph_GPIOD  


/**
  * @}
  */ 
  
/** @addtogroup STM3210C_EVAL_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          3 /*!< Joystick pins are connected to 
                                                an IO Expander (accessible through 
                                                I2C1 interface) */

/**
 * @brief Wakeup push-button
 */
#define WAKEUP_BUTTON_PIN                GPIO_Pin_0
#define WAKEUP_BUTTON_GPIO_PORT          GPIOA
#define WAKEUP_BUTTON_GPIO_CLK           RCC_APB2Periph_GPIOA
#define WAKEUP_BUTTON_EXTI_LINE          EXTI_Line0
#define WAKEUP_BUTTON_EXTI_PORT_SOURCE   GPIO_PortSourceGPIOA
#define WAKEUP_BUTTON_EXTI_PIN_SOURCE    GPIO_PinSource0
#define WAKEUP_BUTTON_EXTI_IRQn          EXTI0_IRQn 

/**
 * @brief Tamper push-button
 */
#define TAMPER_BUTTON_PIN                GPIO_Pin_13
#define TAMPER_BUTTON_GPIO_PORT          GPIOC
#define TAMPER_BUTTON_GPIO_CLK           RCC_APB2Periph_GPIOC
#define TAMPER_BUTTON_EXTI_LINE          EXTI_Line13
#define TAMPER_BUTTON_EXTI_PORT_SOURCE   GPIO_PortSourceGPIOC
#define TAMPER_BUTTON_EXTI_PIN_SOURCE    GPIO_PinSource13
#define TAMPER_BUTTON_EXTI_IRQn          EXTI15_10_IRQn 

/**
 * @brief Key push-button
 */
#define KEY_BUTTON_PIN                   GPIO_Pin_10
#define KEY_BUTTON_GPIO_PORT             GPIOB
#define KEY_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOB
#define KEY_BUTTON_EXTI_LINE             EXTI_Line10
#define KEY_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOB
#define KEY_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource10
#define KEY_BUTTON_EXTI_IRQn             EXTI15_10_IRQn  // notice lilhao
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */ 
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM3210C_EVAL_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
