#ifndef __INPUT_OUTPUT_H
#define __INPUT_OUTPUT_H

//控制LED灯使用的是8位移位寄存器74LS595,(与
//Mcu相连的一端通过了一个双向总线收发器74ls245
#define LED_CTRL_PORT       GPIOC
#define LED_CTRL_RCK        GPIO_Pin_9
#define LED_CTRL_SCK        GPIO_Pin_8
#define LED_CTRL_SER        GPIO_Pin_7
#define LED_RUN_CTRL_PORT GPIOB
#define LED_RUN_CTRL_PIN      GPIO_Pin_14

#define LED_START                               0x80
#define LED_IMPEND                          0x40
#define LED_REVERSED                         0x20
#define LED_WORKING                         0x10
#define LED_NO_U_PAN                              0x08
#define LED_NOT_DISCHARGED               0x04
#define LED_AUTO_N_MANUAL              0x02
#define LED_END                                     0x01

#define LED_ON   0x01
#define LED_OFF  0x00

void LedCtrlPortInit(void);
void LedCtrlInit(void);
void LedCtrl(uint8_t data,uint8_t newState);
void ShowAndSetSysStatus(uint8_t data,uint8_t newState);



//蜂鸣器控制
#define BUZZER_CTRL_PORT       GPIOB
#define BUZZER_CTRL_PIN         GPIO_Pin_13
void BuzzerPortInit(void);
void BuzzerOn(void);
void BuzzerOff(void);
void BuzzerCtrl(uint8_t data);


//控制继电器的开合
#define RELAY_CTRL_PORT       GPIOD
#define RELAY_CTRL_STROBE        GPIO_Pin_11
#define RELAY_CTRL_DATA        GPIO_Pin_8
#define RELAY_CTRL_ENABLE        GPIO_Pin_9
#define RELAY_CTRL_CLK          GPIO_Pin_10

#define RELAY_ON  1
#define RELAY_OFF 0
void RelayCtrlPortInit(void);
void RelayCtrl(unsigned char num,unsigned char newStatus);
void RelayAllDown(void);



//按键值通过并转串的移位锁存器获得
#define KEY_INPUT_PORT1  GPIOC
#define KEY_165_SL  GPIO_Pin_12

#define KEY_INPUT_PORT2  GPIOA
#define KEY_165_CLK  GPIO_Pin_8

#define KEY_INPUT_PORT3  GPIOB
#define KEY_165_QH  GPIO_Pin_12

void KeyInputPortInit(void);
unsigned long input_165(void);


//AD通道选择控制，通过CD4067
#define AD_CH_SEL_L1_PORT  GPIOC
#define AD_CH_L1_A  GPIO_Pin_0
#define AD_CH_L1_B GPIO_Pin_1
#define AD_CH_L1_C  GPIO_Pin_2
#define AD_CH_L1_D     GPIO_Pin_3

#define AD_CH_SEL_L2_PORT  GPIOA
#define AD_CH_L2_A  GPIO_Pin_1
#define AD_CH_L2_B GPIO_Pin_2
#define AD_CH_L2_C  GPIO_Pin_3
#define AD_CH_L2_D     GPIO_Pin_4

#define AD_CH_SEL_L1_INH_PORT  GPIOE
#define AD_CH_L1_INH1  GPIO_Pin_0
#define AD_CH_L1_INH2  GPIO_Pin_1
#define AD_CH_L1_INH3  GPIO_Pin_2
#define AD_CH_L1_INH4  GPIO_Pin_3
#define AD_CH_L1_INH5  GPIO_Pin_4
#define AD_CH_L1_INH6  GPIO_Pin_5
#define AD_CH_L1_INH7  GPIO_Pin_6

#define AD_CH_SEL_L2_INH_PORT  GPIOA
#define AD_CH_L2_INH  GPIO_Pin_5

#define ROW_NUM                             7               //行
#define COL_NUM                            16              //列
#define CH_MAX_NUM          100

#define L1_OPEN_DELAY   4
#define L2_OPEN_DELAY  4
#define AD_CH_OPEN_DELAY  6

void AdChSelPortInit(void);
void signAdChSelect(uint32_t ch);
void autoAdChSelect(void);


#define SQ_CTRL_PORT  GPIOB
#define SQ_OUT GPIO_Pin_1
#define SQ_OUT_EXTI_LINE          EXTI_Line1
#define SQ_OUT_EXTI_PORT_SOURCE   GPIO_PortSourceGPIOB
#define SQ_OUT_EXTI_PIN_SOURCE    GPIO_PinSource1
#define SQ_OUT_EXTI_IRQn          EXTI1_IRQn 
void SqInit(void);
void SqExtiInit(void);
void SqCtrlPortInit(void);
void StartSqDetect(void); //启动上升沿检测
unsigned char  GetSqLevel(void); //取SQ电平值
void RelayStatusAllClr(void);
#endif
