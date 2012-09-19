
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

	while ( DRDY_read );			//等待一个有效字转换完成
	
	for(i=0;i<8;i++)
	{
		SCLK_L;//=0;   			//准备通过DOUT输出数据
		delay_us(READ_BAND_L);  //此期间将数据输出并维持
		
		SCLK_H;//=1;  			// 转变为高电平，保证这时线上的数据不会改变

		if(0 == (DOUT_read)) 		//将输出并维持的数据保存
		{
			read_data = read_data<<1;
		}
		else
		{
			read_data = read_data<<1;
			read_data = read_data+0x01;		
		}
		
		delay_us(READ_BAND_H);//经过这么长时间的延时后，再放开线上的维持状态
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

	// 1  向通信寄存器写数据，选择通道2
	//作为有效通道，将下一操作设为对时钟寄存器进行写操作
	SEND_CMD(0x21);
	// 2对时钟寄存器写操作，设置CLKDIV位，将外部时钟除二，
	//假定外部时钟频率 4.9512MHZ,更新率选为50HZ
	write_ad7705reg(0x0c);

	// 3 向通信寄存器写数据，选择通道2作为有效通道。将下一
	//操作设为对设置寄存器的写操作。
	SEND_CMD(0x11);

	// 4向设置寄存器写数据，将增益设为1，设置为单极性、非缓冲模式
	// 清除滤波器同步，开始自校准
	write_ad7705reg(0x44);
	
	// 5,查询~DRDY输出
	//while ( DRDY_read );	
	// 6 从数据寄存器上读数据
	// 7跳回第5，第6步，直到从选定的通道中取出指定数量的采样
	SCLK_H;
	DIN_H;
}


uint16_t Read_ad7705_data ( void)
{
	uint16_t i;
	uint16_t read_data;
	//CS=0;
	read_data=0x00;

	//读数据寄存器
	SEND_CMD (0x39);   
	while ( DRDY_read );
	
	for(i=0;i<16;i++)
	{
		SCLK_L;//=0;   //准备通过DOUT输出数据
		delay_us(READ_BAND_L);  //此期间将数据输出并维持
		
		SCLK_H;//=1;  // 转变为高电平，保证这时线上的数据不会改变

		if(0 == (DOUT_read)) //将输出并维持的数据保存
		{
			read_data=read_data<<1;
		}
		else
		{
			read_data = read_data<<1;
			read_data = read_data+0x01;		
		}
		
		delay_us(READ_BAND_H);//经过这么长时间的延时后，再放开线上的维持状态
	}
	//CS=1;	
	return ( read_data ); 
}


