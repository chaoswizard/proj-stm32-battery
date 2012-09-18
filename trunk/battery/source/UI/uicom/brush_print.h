/***********************************************************************
 *
 * MODULE NAME:  
 * DESCRIPTION:   基本图形绘制
 * AUTHOR:     wangxi (中山医院岗顶)
 * DATE: 2012/09/11
 *
 ***********************************************************************/
#ifndef __BRUSH_PRINT_H__
#define __BRUSH_PRINT_H__
#include "bits_types.h"
#include "ui_com_obj.h"

enum SCREEN_PIXEL_MODE {
    SCREEN_PIXEL_MODE_NULL,
};

enum SCREEN_LINE_MODE {
    SCREEN_LINE_MODE_NULL,
};

enum SCREEN_CURSOR_MODE {
    SCREEN_CURSOR_RECT,
    SCREEN_CURSOR_ELLIPSE,
};

enum SCREEN_FONT_MODE {
    SCREEN_FONT_NULL,
};


enum SCREEN_RECT_MODE {
    SCREEN_RECT_MODE_NULL,
};


//color | width | pixelMode
typedef u_int8  T_SCREEN_PIXEL_ATTR;
#define PIXEL_ATTR_GET_WIDTH(attr)  (((attr)) & (0xf))
#define PIXEL_ATTR_SET_WIDTH(attr, width)  {((attr) &= (~0xf));((attr) |= ((width) & (0xf)))}

#define PIXEL_ATTR_GET_COLOR(attr)  (((attr)>>4) & (0xf))
#define PIXEL_ATTR_SET_COLOR(attr, color)  {((attr) &= (~(0xf<<4)));((attr) |= (((color) & (0xf))<<4))}



typedef u_int8  T_SCREEN_PIXEL;

enum PIXEL_MODE {
    PIXEL_MODE_CLEAR    = 0,
    PIXEL_MODE_SET      = 1,
    PIXEL_MODE_TURN     = 2,
    PIXEL_MODE_CURSOR   = 3,
};


struct SCREEN_ZONE {
/*
* (x,y)......................
*  |............W...........|
*  |..........................Y
*  |..........................|
*/
    T_SCREEN_PIXEL x;
    T_SCREEN_PIXEL y;
    T_SCREEN_PIXEL w; // r a
    T_SCREEN_PIXEL h; //r  b 
};

#define SCREEN_ZONE_INIT(zone, xx, yy, ww, hh) {\
    (zone)->x = xx;(zone)->y = yy;(zone)->w = ww;(zone)->h = hh;\
}

struct RECT_BORDER {
    T_SCREEN_PIXEL_ATTR l; // left
    T_SCREEN_PIXEL_ATTR t; // top
    T_SCREEN_PIXEL_ATTR r; // right
    T_SCREEN_PIXEL_ATTR b; //bottom
};

struct OSD_ZONE {
    struct SCREEN_ZONE zone;
    struct RECT_BORDER border;
};

#ifdef __cplusplus
extern "C" {
#endif
void Screen_PrintInit(void);
void Screen_PrintPixel(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, enum PIXEL_MODE pixel_mode);
void Screen_PrintLine(T_SCREEN_PIXEL x1, T_SCREEN_PIXEL y1, T_SCREEN_PIXEL x2, T_SCREEN_PIXEL y2, enum PIXEL_MODE pixel_mode);
void Screen_PrintRect(struct OSD_ZONE *zone, enum PIXEL_MODE pixel_mode);
void Screen_PrintFillRect(struct SCREEN_ZONE *rect,  enum PIXEL_MODE pixel_mode);
void Screen_PrintEllipse(struct SCREEN_ZONE *rect, T_SCREEN_PIXEL_ATTR attr);
void Screen_PrintCursor(struct OSD_ZONE *zone, T_SCREEN_PIXEL_ATTR attr);
void Screen_PrintString(struct SCREEN_ZONE *rect, u_int8 *str, T_SCREEN_PIXEL_ATTR attr);
void Screen_PrintBmp(struct SCREEN_ZONE *rect, u_int8 *data, T_SCREEN_PIXEL_ATTR attr);
void Screen_PrintClear(struct SCREEN_ZONE *rect);
T_SCREEN_PIXEL Screen_PrintFont(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info);
T_SCREEN_PIXEL Screen_PrintFont_By_Bit(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info);
T_SCREEN_PIXEL Screen_PrintFont_By_Byte(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info);

#ifdef __cplusplus
}
#endif
#endif



