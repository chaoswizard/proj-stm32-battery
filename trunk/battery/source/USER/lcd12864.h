#ifndef __LCD12864_H
#define __LCD12864_H
void WR_XLCD (unsigned char dat_comm,unsigned char content);
unsigned char XLCD_RECV_DATA(void);
void XLCD_SEND_DATA(unsigned char data);
void XLCD_SEND_CMD(unsigned char cmd);
void XLCD_MOV_POS(unsigned char x,unsigned char y);
void INIT_XLCD(void);
void XFILLRAM(unsigned char dat);
void printstr(unsigned char x,unsigned char y);
void displaycursor(unsigned char x,unsigned char y,unsigned char w,unsigned char h);
void putpixel(unsigned char x,unsigned char y,unsigned char pixel_mode);
void line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char pixel_mode);
void printSmall(unsigned char x,unsigned char y);
void delay_us(unsigned int us);

#endif

