#ifndef __AT45DB041_H
#define __AT45DB041_H


#define SPI_FLASH_SIZE                 512*1024
#define SPI_FLASH_SYS_CFG_SIZE      2*1024

#define ONE_BLOCK_INDEX_SIZE 2*100 //编号区大小
#define ONE_BLOCK_DATA_SIZE        2*100    //一次有效采样需保存的数据块大小
#define ONE_BLOCK_SIZE (ONE_BLOCK_INDEX_SIZE + ONE_BLOCK_DATA_SIZE)
#define SPI_FLASH_MAX_SAVE  ((unsigned char)((SPI_FLASH_SIZE-SPI_FLASH_SYS_CFG_SIZE)/ONE_BLOCK_SIZE))
//spi flash配置区结构
#define CHANNEL_USING                   1
#define CHANNEL_UN_USED             0

#if 0
EXT_GLOBAL  struct {
        u_int8 channel1:1;
        u_int8 channel2:1;
        u_int8 channel3:1;
        u_int8 channel4:1;
        u_int8 channel5:1;
        u_int8 channel6:1;
        u_int8 channel7:1;
        u_int8 channel8:1;
        
        u_int8 channel9:1;
        u_int8 channel10:1;
        u_int8 channel11:1;
        u_int8 channel12:1;
        u_int8 channel13:1;
        u_int8 channel14:1;
        u_int8 channel15:1;
        u_int8 channel16:1;
        
        u_int8 channel17:1;
        u_int8 channel18:1;
        u_int8 channel19:1;
        u_int8 channel20:1;
        u_int8 channel21:1;
        u_int8 channel22:1;
        u_int8 channel23:1;
        u_int8 channel24:1;
        
        u_int8 channel25:1;
        u_int8 channel26:1;
        u_int8 channel27:1;
        u_int8 channel281:1;
        u_int8 channel29:1;
        u_int8 channel30:1;
        u_int8 channel31:1;
        u_int8 channel32:1;
        
        u_int8 channel33:1;
        u_int8 channel34:1;
        u_int8 channel35:1;
        u_int8 channel36:1;
        u_int8 channel37:1;
        u_int8 channel38:1;
        u_int8 channel39:1;
        u_int8 channel40:1;
        
        u_int8 channel41:1;
        u_int8 channel42:1;
        u_int8 channel43:1;
        u_int8 channel44:1;
        u_int8 channel45:1;
        u_int8 channel46:1;
        u_int8 channel47:1;
        u_int8 channel48:1;
        
        u_int8 channel49:1;
        u_int8 channel50:1;
        u_int8 channel51:1;
        u_int8 channel52:1;
        u_int8 channel52:1;
        u_int8 channel54:1;
        u_int8 channel55:1;
        u_int8 channel56:1;
        
        u_int8 channel57:1;
        u_int8 channel58:1;
        u_int8 channel59:1;
        u_int8 channel60:1;
        u_int8 channel61:1;
        u_int8 channel62:1;
        u_int8 channel63:1;
        u_int8 channel64:1;
        
        u_int8 channel65:1;
        u_int8 channel66:1;
        u_int8 channel67:1;
        u_int8 channel68:1;
        u_int8 channel69:1;
        u_int8 channel70:1;
        u_int8 channel71:1;
        u_int8 channel72:1;
        
        u_int8 channel73:1;
        u_int8 channel74:1;
        u_int8 channel75:1;
        u_int8 channel76:1;
        u_int8 channel77:1;
        u_int8 channel78:1;
        u_int8 channel79:1;
        u_int8 channel80:1;
               
        u_int8 channel81:1;
        u_int8 channel82:1;
        u_int8 channel83:1;
        u_int8 channel84:1;
        u_int8 channel85:1;
        u_int8 channel86:1;
        u_int8 channel87:1;
        u_int8 channel88:1;
        
        u_int8 channel89:1;
        u_int8 channel90:1;
        u_int8 channel91:1;
        u_int8 channel92:1;
        u_int8 channel93:1;
        u_int8 channel94:1;                
        u_int8 channel95:1;
        u_int8 channel96:1;
        
        u_int8 channel97:1;
        u_int8 channel98:1;
        u_int8 channel99:1;
        u_int8 channel100:1;
        u_int8 channel101:1;
        u_int8 channel102:1;  
        u_int8 channel103:1;
        u_int8 channel104:1;
};
#endif



#define SPI_FLASH_PORT    GPIOD
#define SPI_041_SI   GPIO_Pin_15
#define SPI_041_SO GPIO_Pin_14
#define SPI_041_SCK GPIO_Pin_13
#define SPI_041_CS  GPIO_Pin_12

#define SPI_FLASH_RST_PORT  GPIOB
#define SPI_041_RST  GPIO_Pin_0

void SpiFlashPortInit(void);
void spiFlashInit(void);
void RST_DB041(void);
unsigned char writeByteSPI(unsigned char tt);
void ReadMemory(unsigned char *pInData,unsigned int mm, unsigned int startPage, unsigned int startByte);
unsigned char ReadStatusFromDataFlash(void);
void write_Page(unsigned char *wbuf,unsigned int tlen,unsigned int tpage,unsigned int tstart);
void write_flash(unsigned char *wbuf,unsigned int tlen,unsigned long taddr);
void read_flash(unsigned char *rbuf,unsigned int I_length,unsigned long L_addr);
#endif


