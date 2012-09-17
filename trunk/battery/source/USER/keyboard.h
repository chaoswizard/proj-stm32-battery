#ifndef __KEYBOARD_H
#define __KEYBOARD_H

void KeyServer(uint32_t qkey);
void drawMainFace(unsigned char clearing);
void refreshMainFace(void);
void drawSetMenu(unsigned char clearing);
void drawLogo(void);
void saveSucceed(void);
void testIOTran(void);
void drawPicture(void);

#endif

#define KEY_INC		0x00010000
#define KEY_DEC		0x00000004
#define KEY_UP 		0x00020000
#define KEY_DOWN	0x00000080
#define KEY_LEFT	0x00000100
#define KEY_RIGHT	0x00000400
#define KEY_OK		0x00000002
#define KEY_ESC		0x00000001
#define KEY_0 		0x00040000
#define KEY_1 		0x00080000
#define KEY_2 		0x00100000
#define KEY_3			0x00200000
#define KEY_4			0x00000800
#define KEY_5 		0x00001000
#define KEY_6 		0x00002000
#define KEY_7 		0x00000008
#define KEY_8 		0x00000010
#define KEY_9			0x00000020
#define KEY_RUN		0x00400000
#define KEY_STOP	0x00004000

