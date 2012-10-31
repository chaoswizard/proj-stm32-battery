
#ifndef __COMMON_H__
#define __COMMON_H__
extern void delay_us(unsigned long n);
extern void delay_ms(unsigned long n);

unsigned short calc_checkcode(unsigned char *buf,  int nsize);

#endif //__COMMON_H__


