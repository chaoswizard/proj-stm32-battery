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

#ifdef __cplusplus
extern "C" {
#endif
void Screen_PrintInit(void);
void Screen_PrintPixel(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, enum PIXEL_COLOR pixel_mode);
void Screen_PrintLine(T_SCREEN_PIXEL x1, T_SCREEN_PIXEL y1, T_SCREEN_PIXEL x2, T_SCREEN_PIXEL y2, enum PIXEL_COLOR pixel_mode);
void Screen_PrintRect(struct OSD_ZONE *zone, enum PIXEL_COLOR pixel_mode);
void Screen_PrintFillRect(struct SCREEN_ZONE *rect,  enum PIXEL_COLOR pixel_mode);
void Screen_PrintEllipse(struct SCREEN_ZONE *rect, T_UICOM_PIXEL_ATTR attr);
void Screen_PrintCursor(struct OSD_ZONE *zone, T_UICOM_PIXEL_ATTR attr);
void Screen_PrintString(struct SCREEN_ZONE *rect, u_int8 *str, T_UICOM_DATA_ATTR attr);
void Screen_PrintBmp(struct SCREEN_ZONE *rect, u_int8 *data, T_UICOM_DATA_ATTR attr);
void Screen_PrintClear(struct SCREEN_ZONE *rect);
void Screen_Print_8Bit_V(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, u_int8 data, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor);
void Screen_PrintMapRule(struct OSD_ZONE *zone, enum PIXEL_COLOR pixel_mode);

T_SCREEN_PIXEL Screen_PrintFont(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor);
T_SCREEN_PIXEL Screen_PrintFont_By_Bit(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor);
T_SCREEN_PIXEL Screen_PrintFont_By_Byte(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor);

#ifdef __cplusplus
}
#endif
#endif



