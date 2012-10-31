/**
  ******************************************************************************
  * @file    stm3210c_eval.c
  * @author  MCD Application Team
  * @version V5.0.1
  * @date    05-March-2012
  * @brief   This file provides
  *            - set of firmware functions to manage Leds, push-button and COM ports
  *            - low level initialization functions for SD card (on SPI) and I2C
  *              serial EEPROM (sEE)
  *          available on STM3210C-EVAL evaluation board from STMicroelectronics.   
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
#include "stm3210c_eval.h"
#include "stm32f10x_dma.h"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM3210C_EVAL
  * @{
  */ 
      
/** @defgroup STM3210C_EVAL_LOW_LEVEL 
  * @brief This file provides firmware functions to manage Leds, push-buttons, 
  *        COM ports, SD card on SPI and EEPROM (sEE) available on STM3210C-EVAL 
  *        evaluation board from STMicroelectronics.
  * @{
  */ 

/** @defgroup STM3210C_EVAL_LOW_LEVEL_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LOW_LEVEL_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LOW_LEVEL_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LOW_LEVEL_Private_Variables
  * @{
  */ 
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT,
                                 LED4_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN,
                                 LED4_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK,
                                 LED4_GPIO_CLK};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {WAKEUP_BUTTON_GPIO_PORT, TAMPER_BUTTON_GPIO_PORT,
                                      KEY_BUTTON_GPIO_PORT}; 

const uint16_t BUTTON_PIN[BUTTONn] = {WAKEUP_BUTTON_PIN, TAMPER_BUTTON_PIN,
                                      KEY_BUTTON_PIN}; 

const uint32_t BUTTON_CLK[BUTTONn] = {WAKEUP_BUTTON_GPIO_CLK, TAMPER_BUTTON_GPIO_CLK,
                                      KEY_BUTTON_GPIO_CLK};

const uint16_t BUTTON_EXTI_LINE[BUTTONn] = {WAKEUP_BUTTON_EXTI_LINE,
                                            TAMPER_BUTTON_EXTI_LINE, 
                                            KEY_BUTTON_EXTI_LINE};

const uint16_t BUTTON_PORT_SOURCE[BUTTONn] = {WAKEUP_BUTTON_EXTI_PORT_SOURCE,
                                              TAMPER_BUTTON_EXTI_PORT_SOURCE, 
                                              KEY_BUTTON_EXTI_PORT_SOURCE};
								 
const uint16_t BUTTON_PIN_SOURCE[BUTTONn] = {WAKEUP_BUTTON_EXTI_PIN_SOURCE,
                                             TAMPER_BUTTON_EXTI_PIN_SOURCE, 
                                             KEY_BUTTON_EXTI_PIN_SOURCE}; 

const uint16_t BUTTON_IRQn[BUTTONn] = {WAKEUP_BUTTON_EXTI_IRQn, TAMPER_BUTTON_EXTI_IRQn,
                                       KEY_BUTTON_EXTI_IRQn};

/**
  * @}
  */ 

    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
