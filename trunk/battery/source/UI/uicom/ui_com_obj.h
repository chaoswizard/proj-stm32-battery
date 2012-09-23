/***********************************************************************
 *
 * MODULE NAME:  
 * DESCRIPTION:  ui com基本控制
 * AUTHOR:     wangxi (中山医院岗顶)
 * DATE: 2012/09/11
 *
 ***********************************************************************/
#ifndef __UI_COM_OBJ_H__
#define __UI_COM_OBJ_H__
#include "bits_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u_int8 T_UICOM_OBJ_COUNT;  
typedef u_int16 T_CHAR_CODE;


enum  UI_COM_PAINT_ORDER {
    UI_PAINT_ORDER_NULL = 0,
    UI_PAINT_ORDER_LEFT_RIGHT = (1 << 1),
    UI_PAINT_ORDER_TOP_BOTTOM = (1 << 2),
};

enum  UICOM_DATA_TYPE {
    UICOM_DATA_TYPE_TEXT = 1,
    UICOM_DATA_TYPE_PICTURE,
};

enum PIXEL_COLOR {
    PIXEL_MODE_CLEAR    = 0,
    PIXEL_MODE_SET      = 1,
    PIXEL_MODE_TURN     = 2,
    PIXEL_MODE_CURSOR   = 3,
};

enum {
    FONT_SIZE_SMALL    = 0,
    FONT_SIZE_NORMAL   = 1,
};


#define TEXT_ATTR(size, bgColor, fgColor)   (((size)&0x3) | (((bgColor)&0x7)<<2) | (((fgColor)&0x7)<<5))
#define TEXT_ATTR_SIZE(attr)                ((attr)& 0x3)
#define TEXT_ATTR_COLOR_BG(attr)            (((attr)>>2)&0x7)
#define TEXT_ATTR_COLOR_FG(attr)            (((attr)>>5)&0x7)


#define TEXT_SMALL_BLACK              TEXT_ATTR(FONT_SIZE_SMALL, PIXEL_MODE_CLEAR, PIXEL_MODE_SET)
#define TEXT_BIG_BLACK                TEXT_ATTR(FONT_SIZE_NORMAL, PIXEL_MODE_CLEAR, PIXEL_MODE_SET)

#define UICOM_DATA_BUF(c)   ((struct UICOM_DATA *)(c))->data

#define UICOM_DATA_FILL(c, strm)     {\
    ((struct UICOM_DATA *)(c))->data  = strm;\
}

#define UICOM_DATA_TEXT_ATTR(c, attrib)     {\
    ((struct UICOM_DATA *)(c))->type  = UICOM_DATA_TYPE_TEXT;\
    ((struct UICOM_DATA *)(c))->attr  = attrib;\
}


#define UICOM_DATA_PIC_ATTR(c, attrib)     {\
    ((struct UICOM_DATA *)(c))->type  = UICOM_DATA_TYPE_PICTURE;\
    ((struct UICOM_DATA *)(c))->attr  = attrib;\
}

struct UICOM_DATA {
    enum UICOM_DATA_TYPE    type;
    u_int8                  attr;
    void *data;
};

typedef struct UICOM_DATA *PUICOM_DATA;


struct UICOM_1PP_BMP_INFO {
    u_int8 *data;
    u_int8 size;
    u_int8 width;
    u_int8 height;
};

#define UICOM_TEXT_STR_REF(text)  ((u_int8 *)(guicom_str##text))
#define UICOM_TEXT_DECLAER(text)  extern const u_int8 *guicom_str##text
#define DEF_UICOM_CONTENT_TEXT(text, str)   \
const u_int8 *guicom_str##text = str

#define UICOM_PIC_DATA_REF(bmp)   ((u_int8 *)(guicom_bmp##bmp))
#define UICOM_PIC_DECLAER(bmp)    extern const u_int8 *guicom_bmp##bmp
#define DEF_UICOM_CONTENT_PIC(bmp, data)   \
const u_int8 * guicom_bmp##bmp = data


// 1D array, list / vector
void uicom_obj_list(T_UICOM_OBJ_COUNT len, void *param, 
                          void (*paint)(T_UICOM_OBJ_COUNT pos, void *param), 
                          u_int8 paintOrder);
// 2D array, table /picture
void uicom_obj_tab(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, void *param, 
                          void (*paint)(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, void *param), 
                          u_int8 paintOrder);

u_int8 uicom_font_getdata(u_int8 *ch, struct UICOM_1PP_BMP_INFO *info, u_int8 fontsize);

#ifdef __cplusplus
}
#endif
#endif


