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
#include "syscom_util.h"


#ifdef __cplusplus
extern "C" {
#endif
//==========================================================
typedef u_int8  T_UICOM_COUNT;  
typedef void *  T_UICOM_HANDL;  

//==========================================================

typedef u_int8  T_SCREEN_PIXEL;

enum PIXEL_COLOR {
    PIXEL_MODE_CLEAR    = 0,//白
    PIXEL_MODE_SET      = 1,//黑
    PIXEL_MODE_TURN     = 2,//翻转
    PIXEL_MODE_TRANS    = 3,//透明色
};

//color | width | pixelMode
typedef u_int8  T_UICOM_PIXEL_ATTR;
#define PIXEL_ATTR_GET_WIDTH(attr)  (((attr)) & (0xf))
#define PIXEL_ATTR_SET_WIDTH(attr, width)  {((attr) &= (~0xf));((attr) |= ((width) & (0xf)))}

#define PIXEL_ATTR_GET_COLOR(attr)  (((attr)>>4) & (0xf))
#define PIXEL_ATTR_SET_COLOR(attr, color)  {((attr) &= (~(0xf<<4)));((attr) |= (((color) & (0xf))<<4))}
//-----------------------------------------------------------------------
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

#define SCREEN_BORDER_INIT(border, ll, tt, rr, bb) {\
    (border)->l = ll;(border)->t = tt;(border)->r = rr;(border)->b = bb;\
}

struct RECT_BORDER {
    T_UICOM_PIXEL_ATTR l; // left
    T_UICOM_PIXEL_ATTR t; // top
    T_UICOM_PIXEL_ATTR r; // right
    T_UICOM_PIXEL_ATTR b; //bottom
};

struct OSD_ZONE {
    struct SCREEN_ZONE zone;
    struct RECT_BORDER border;
};

//==========================================================
typedef u_int8 T_UICOM_PAINT_FLAG;

#define PAINT_STATUS_NULL         0
#define PAINT_FLAG_BORDER         0x1
#define PAINT_FLAG_DATA           0x2
#define PAINT_FLAG_NORMAL         (PAINT_FLAG_DATA | PAINT_FLAG_BORDER)
//----------------------------------------------------------------------
#define  PAINT_MAP_NULL           0
#define  PAINT_MAP_PIXEL          0x1
#define  PAINT_MAP_LINE           0x2
#define  PAINT_MAP_BAR            0x4
#define  PAINT_MAP_PIE            0x8
#define  PAINT_MAP_STACKED_LINE   0x16
//==========================================================

enum T_UICOM_STATUS {
    UICOM_STATUS_NORMAL,
    UICOM_STATUS_SELECTED,
    UICOM_STATUS_DISABLE,
};

#define UICOM_IS_FOCUS(type)  (UICOM_STATUS_SELECTED == type)
//==========================================================
typedef u_int8 T_UICOM_ORDER_FLAG;

#define UICOM_ORDER_DEFAULT       0
#define UICOM_ORDER_LEFT_RIGHT    0x1
#define UICOM_ORDER_TOP_BOTTOM    0x2
//==========================================================
typedef enum  {
    FONT_SIZE_SMALL,
    FONT_SIZE_NORMAL,
} T_UICOM_FONT_SIZE;

enum UICOM_FOCUS_TYPE {
    UICOM_FOCUS_TYPE_NULL,
    UICOM_FOCUS_TYPE_RECT,
    UICOM_FOCUS_TYPE_VLINE,
    UICOM_FOCUS_TYPE_HLINE,
    UICOM_FOCUS_TYPE_BORDER,
};
//==========================================================

enum  UICOM_DATA_TYPE {
    UICOM_DATA_TYPE_TEXT = 1,
    UICOM_DATA_TYPE_PICTURE,
    UICOM_DATA_TYPE_VALUE,
};
//----------------------------------------------------------------------
typedef u_int32  T_UICOM_DATA_ATTR;
//startfocus[>0]|endfocus[>0]|color | width | pixelMode
#define TEXT_ATTR(size, bgColor, fgColor)     (((size)&0x3) | (((bgColor)&0x7)<<2) | (((fgColor)&0x7)<<5))
#define TEXT_ATTR_FOCUS_RANGE(startFocus, endFocus)  (util_bit_set_range(startFocus, endFocus) << 8)
#define TEXT_ATTR_FOCUS(focus)                       (1<<(focus+8))
#define TEXT_ATTR_GET_SIZE(attr)                ((attr)& 0x3)
#define TEXT_ATTR_GET_COLOR_BG(attr)            (((attr)>>2)&0x7)
#define TEXT_ATTR_GET_COLOR_FG(attr)            (((attr)>>5)&0x7)
#define TEXT_ATTR_GET_FOCUS(attr)               (((attr)>>8)&0xFFFFFF)
//----------------------------------------------------------------------
#define TEXT_SMALL_BLACK              TEXT_ATTR(FONT_SIZE_SMALL, PIXEL_MODE_TRANS,  PIXEL_MODE_TURN)
#define TEXT_BIG_BLACK                TEXT_ATTR(FONT_SIZE_NORMAL, PIXEL_MODE_TRANS, PIXEL_MODE_TURN)
//IXEL_MODE_CLEAR, PIXEL_MODE_SET
struct UICOM_DATA {
    enum UICOM_DATA_TYPE    type;
    T_UICOM_DATA_ATTR       attr;
    void                    *data;//string address OR picture address OR  u_32 value
};

typedef struct UICOM_DATA *PUICOM_DATA;
//----------------------------------------------------------------------
#define UICOM_DATA_BUF(c)   ((struct UICOM_DATA *)(c))->data

#define UICOM_DATA_FILL(c, strm)     {\
    ((struct UICOM_DATA *)(c))->data  = (void *)strm;\
}

#define UICOM_DATA_TEXT_ATTR_RST(c, attrib)     {\
    ((struct UICOM_DATA *)(c))->type  = UICOM_DATA_TYPE_TEXT;\
    ((struct UICOM_DATA *)(c))->attr  = attrib;\
}

#define UICOM_DATA_TEXT_ATTR_ADD(c, attrib)     {\
    ((struct UICOM_DATA *)(c))->type  = UICOM_DATA_TYPE_TEXT;\
    ((struct UICOM_DATA *)(c))->attr  |= attrib;\
}

#define UICOM_DATA_PIC_ATTR_RST(c, attrib)     {\
    ((struct UICOM_DATA *)(c))->type  = UICOM_DATA_TYPE_PICTURE;\
    ((struct UICOM_DATA *)(c))->attr  = attrib;\
}

#define UICOM_DATA_PIC_ATTR_ADD(c, attrib)     {\
    ((struct UICOM_DATA *)(c))->type  = UICOM_DATA_TYPE_PICTURE;\
    ((struct UICOM_DATA *)(c))->attr  |= attrib;\
}

#define UICOM_DATA_VALUE_ATTR_RST(c, attrib)     {\
    ((struct UICOM_DATA *)(c))->type  = UICOM_DATA_TYPE_VALUE;\
    ((struct UICOM_DATA *)(c))->attr  = attrib;\
}

//==========================================================
#define UICOM_TEXT_STR_REF(text)  ((u_int8 *)(guicom_str##text))
#define UICOM_TEXT_DECLAER(text)  extern const u_int8 *guicom_str##text
#define DEF_UICOM_CONTENT_TEXT(text, str)   \
const u_int8 *guicom_str##text = str

#define UICOM_PIC_DATA_REF(bmp)   ((u_int8 *)(guicom_bmp##bmp))
#define UICOM_PIC_DECLAER(bmp)    extern const u_int8 *guicom_bmp##bmp
#define DEF_UICOM_CONTENT_PIC(bmp, data)   \
const u_int8 * guicom_bmp##bmp = data
//==========================================================
struct UICOM_1PP_BMP_INFO {
    u_int8 *data;
    u_int8 size;
    u_int8 width;
    u_int8 height;
};

//==========================================================
enum INPUTBOX_PROC_TYPE{
    INPUTBOX_PROC_NEWCHAR_RMODE,//replace mode
    INPUTBOX_PROC_NEWCHAR_IMODE,//insert mode
    INPUTBOX_PROC_MOVE_BACK,//
    INPUTBOX_PROC_MOVE_FRONT,
    INPUTBOX_PROC_DEL_BACK,
    INPUTBOX_PROC_DEL_FRONT,
};
//==========================================================
// 1D array, list / vector
void uicom_obj_list(T_UICOM_COUNT len, T_UICOM_HANDL param, 
                          void (*paint)(T_UICOM_COUNT pos, T_UICOM_HANDL param), 
                          T_UICOM_ORDER_FLAG paintOrder);
// 2D array, table /picture
void uicom_obj_tab(T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_HANDL param, 
                          void (*paint)(T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_HANDL param), 
                          T_UICOM_ORDER_FLAG paintOrder);

// dymatic map
void uicom_obj_map(T_UICOM_COUNT start, T_UICOM_HANDL param, 
                            T_UICOM_COUNT (*getNext)(T_UICOM_COUNT *idx, T_UICOM_HANDL param), 
                            void (*paint)(T_UICOM_COUNT idx, T_UICOM_HANDL param),
                            T_UICOM_ORDER_FLAG paintOrder);

u_int8 uicom_font_getdata(u_int8 *ch, u_int8 chlen ,struct UICOM_1PP_BMP_INFO *info, T_UICOM_FONT_SIZE fontsize);
u_int8 uicom_inputbox_proc(enum INPUTBOX_PROC_TYPE evt, u_int8 *data, u_int8 focus, void *param);

#ifdef __cplusplus
}
#endif
#endif


