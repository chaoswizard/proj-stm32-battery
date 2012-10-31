#ifndef __LCD12864_H
#define __LCD12864_H

void  lcd12864_Clear(void);
void  lcd12864_init() ;
void lcd12864_putpixel(unsigned char x, unsigned char y, unsigned char pixel_mode);

#endif

