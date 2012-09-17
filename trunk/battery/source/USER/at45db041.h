#ifndef __AT45DB041_H
#define __AT45DB041_H
void RST_DB041(void);
unsigned char writeByteSPI(unsigned char tt);
void ReadMemory(unsigned char *pInData,unsigned int mm, unsigned int startPage, unsigned int startByte);
unsigned char ReadStatusFromDataFlash(void);
void write_Page(unsigned char *wbuf,unsigned int tlen,unsigned int tpage,unsigned int tstart);
void write_flash(unsigned char *wbuf,unsigned int tlen,unsigned long taddr);
void read_flash(unsigned char *rbuf,unsigned int I_length,unsigned long L_addr);
#endif


