
#include "stm32f10x.h"

#include "stm32f10x_conf.h"

#include "AD7705.H"

static void write_ad7705reg(uint8_t  byteword);
static uint8_t read_ad7705reg ( void);

#define SEND_CMD(v)  write_ad7705reg(v)	   

static void delay_us(uint32_t us)
{
    uint32_t i;
    for(i=0;i<us;i++)
	{
	  __NOP();
	}
}

void AD7705_configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/*PB2->SCLK */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	/* Configure I2 C2 pins: PE8->DIN and */  
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Configure AD7705 pins: PE10->DOUT*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING ;  
	GPIO_Init(GPIOE, &GPIO_InitStructure);   

	/* Configure AD7705 pins: PE12->DRDY*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;    
	GPIO_Init(GPIOE, &GPIO_InitStructure);   

	SCLK_H;
	DIN_H;
}

static uint8_t read_ad7705reg ( void)
{
	uint32_t i;
	uint8_t read_data;
	//CS=0;
	read_data=0x00;

	while ( DRDY_read );			//�ȴ�һ����Ч��ת�����
	
	for(i=0;i<8;i++)
	{
		SCLK_L;//=0;   			//׼��ͨ��DOUT�������
		delay_us(READ_BAND_L);  //���ڼ佫���������ά��
		
		SCLK_H;//=1;  			// ת��Ϊ�ߵ�ƽ����֤��ʱ���ϵ����ݲ���ı�

		if(0 == (DOUT_read)) 		//�������ά�ֵ����ݱ���
		{
			read_data = read_data<<1;
		}
		else
		{
			read_data = read_data<<1;
			read_data = read_data+0x01;		
		}
		
		delay_us(READ_BAND_H);//������ô��ʱ�����ʱ���ٷſ����ϵ�ά��״̬
	}
	//CS=1;	
	return ( read_data ); 
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
		
		delay_us(WRITE_BAND_L);
		SCLK_H;
		delay_us(WRITE_BAND_H);
		temp=temp>>1;
	}
	//CS=1;
}

void AD7705_inti (void)
{

	// 1  ��ͨ�żĴ���д���ݣ�ѡ��ͨ��2
	//��Ϊ��Чͨ��������һ������Ϊ��ʱ�ӼĴ�������д����
	SEND_CMD(0x21);
	// 2��ʱ�ӼĴ���д����������CLKDIVλ�����ⲿʱ�ӳ�����
	//�ٶ��ⲿʱ��Ƶ�� 4.9512MHZ,������ѡΪ50HZ
	write_ad7705reg(0x0c);

	// 3 ��ͨ�żĴ���д���ݣ�ѡ��ͨ��2��Ϊ��Чͨ��������һ
	//������Ϊ�����üĴ�����д������
	SEND_CMD(0x11);

	// 4�����üĴ���д���ݣ���������Ϊ1������Ϊ�����ԡ��ǻ���ģʽ
	// ����˲���ͬ������ʼ��У׼
	write_ad7705reg(0x44);
	
	// 5,��ѯ~DRDY���
	//while ( DRDY_read );	
	// 6 �����ݼĴ����϶�����
	// 7���ص�5����6����ֱ����ѡ����ͨ����ȡ��ָ�������Ĳ���
	SCLK_H;
	DIN_H;
}


uint16_t Read_ad7705_data ( void)
{
	uint16_t i;
	uint16_t read_data;
	//CS=0;
	read_data=0x00;

	//�����ݼĴ���
	SEND_CMD (0x39);   
	while ( DRDY_read );
	
	for(i=0;i<16;i++)
	{
		SCLK_L;//=0;   //׼��ͨ��DOUT�������
		delay_us(READ_BAND_L);  //���ڼ佫���������ά��
		
		SCLK_H;//=1;  // ת��Ϊ�ߵ�ƽ����֤��ʱ���ϵ����ݲ���ı�

		if(0 == (DOUT_read)) //�������ά�ֵ����ݱ���
		{
			read_data=read_data<<1;
		}
		else
		{
			read_data = read_data<<1;
			read_data = read_data+0x01;		
		}
		
		delay_us(READ_BAND_H);//������ô��ʱ�����ʱ���ٷſ����ϵ�ά��״̬
	}
	//CS=1;	
	return ( read_data ); 
}


