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
*  |..........................H
*  |..........................|
*/
    T_SCREEN_PIXEL x;
    T_SCREEN_PIXEL y;
    T_SCREEN_PIXEL w; // r a
    T_SCREEN_PIXEL h; //r  b 
};

#define SCREEN_ZONE_INIT(zone, xx, yy, ww, hh) {\
    (zone)->x = (xx);(zone)->y = (yy);(zone)->w = (ww);(zone)->h = (hh);\
}

#define SCREEN_BORDER_INIT(border, ll, tt, rr, bb) {\
    (border)->l = (ll);(border)->t = (tt);(border)->r = (rr);(border)->b = (bb);\
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
typedef u_int8 T_UICOM_ORDER;
//0:H  1:V
#define UICOM_DIR_BIT_H_V          (0x1<<0)
//0:L->R/T->B  1:R->L/B->T
#define UICOM_DIR_BIT_ORDER        (0x1<<1)

// external  flag for endian pos.
#define UICOM_DIR_BIT_START        (0x1<<2) 
#define UICOM_DIR_BIT_END          (0x1<<3) 

// line dir use 2 bit
#define LINE_DIR_X_LR               (0 | 0)
#define LINE_DIR_X_RL               (0 | UICOM_DIR_BIT_ORDER)
#define LINE_DIR_Y_TB               (UICOM_DIR_BIT_H_V | 0 )
#define LINE_DIR_Y_BT               (UICOM_DIR_BIT_H_V | UICOM_DIR_BIT_ORDER)
#define LINE_DIR_BIT_ENDIAN         (0 | UICOM_DIR_BIT_END) 

#define LINE_DIR_IS_X(dir)            (0 == ((dir)&UICOM_DIR_BIT_H_V))
#define LINE_DIR_IS_Y(dir)            (0 != ((dir)&UICOM_DIR_BIT_H_V))
#define LINE_DIR_IS_END(dir)          (0 != ((dir)&LINE_DIR_BIT_ENDIAN)) 

// rect dir use 4 bit
#define RECT_DIR_X_LR_Y_TB          (LINE_DIR_X_LR | (LINE_DIR_Y_TB<<2))
#define RECT_DIR_X_LR_Y_BT          (LINE_DIR_X_LR | (LINE_DIR_Y_BT<<2))
#define RECT_DIR_X_RL_Y_TB          (LINE_DIR_X_RL | (LINE_DIR_Y_TB<<2))
#define RECT_DIR_X_RL_Y_BT          (LINE_DIR_X_RL | (LINE_DIR_Y_BT<<2))



//==========================================================
typedef u_int8 T_UICOM_DRAW_MODE;
#define PAINT_FLAG_EMPTY                0
#define PAINT_FLAG_BORDER             0x1
#define PAINT_FLAG_DATA               0x2
//----------------------------------------------------------------------
#define PAINT_FLAG_MAP_PIXEL          0x1
#define PAINT_FLAG_MAP_LINE           0x2
#define PAINT_FLAG_MAP_BAR            0x4
#define PAINT_FLAG_MAP_PIE            0x8
#define PAINT_FLAG_MAP_STACKED_LINE   0x16
//==========================================================

enum T_UICOM_STATUS {
    UICOM_STATUS_NORMAL,
    UICOM_STATUS_SELECTED,
    UICOM_STATUS_DISABLE,
};

enum T_UICOM_ACTION {
    UICOM_ACTION_NULL,
    UICOM_ACTION_FOCUS,
    UICOM_ACTION_ENTER,
};
//----------------------------------------------------------------------
typedef u_int16 T_UICOM_EVENT;

#define UICOM_PROC_EVENT(act, status) (((act)<<8) | ((status)&0xFF))
#define UICOM_STATUS(evt)             ((evt)&0xFF)
#define UICOM_ACTION(evt)             (((evt)>>8)&0xFF)
//==========================================================
#define UICOM_TST_SLECTED(evt)        (UICOM_STATUS_SELECTED == UICOM_STATUS(evt))
#define UICOM_CLR_SLECTED(evt)        ((evt) = UICOM_PROC_EVENT(UICOM_ACTION(evt), UICOM_STATUS_NORMAL))
#define UICOM_SET_SLECTED(evt)        ((evt) = UICOM_PROC_EVENT(UICOM_ACTION(evt), UICOM_STATUS_SELECTED))
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
#define TEXT_ATTR(size, bgColor, fgColor)            (((size)&0x3) | (((bgColor)&0x7)<<2) | (((fgColor)&0x7)<<5))
// bit[1:0]
#define TEXT_ATTR_GET_SIZE(attr)                     ((attr)& 0x3)
// bit[4:2]
#define TEXT_ATTR_GET_COLOR_BG(attr)                 (((attr)>>2)&0x7)
// bit[7:5]
#define TEXT_ATTR_GET_COLOR_FG(attr)                 (((attr)>>5)&0x7)
//---------------
// bit[27:8]  (Only Suport Focus 20 Chars, If focus more, div long string to short string please)
#define TEXT_ATTR_GET_FOCUS(attr)                    (((attr)>>8)&0xFFFFF)
#define TEXT_ATTR_FOCUS_RANGE(startFocus, endFocus)  ((util_bit32_set_range(startFocus, endFocus)& 0xFFFFF) << 8)
#define TEXT_ATTR_FOCUS(focus)                       ((util_bit32_set_range(focus, focus)& 0xFFFFF) << 8)
// bit[31:28]
#define TEXT_ATTR_GET_ORDER(attr)                    (((attr)>>28)&0xF)
#define TEXT_ATTR_STR_LAYOUT(attr)                   (((attr)&0xF)<<28)
//----------------------------------------------------------------------
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
#define UICOM_TEXT_STR_REF(text)  ((p_str)(guicom_str##text))
#define UICOM_TEXT_DECLAER(text)  extern p_cstr guicom_str##text
#define DEF_UICOM_CONTENT_TEXT(text, str)   \
const u_int8 *guicom_str##text = str

#define UICOM_PIC_DATA_REF(bmp)   ((p_str)(guicom_bmp##bmp))
#define UICOM_PIC_DECLAER(bmp)    extern p_cstr guicom_bmp##bmp
#define DEF_UICOM_CONTENT_PIC(bmp, data)   \
p_cstr guicom_bmp##bmp = data
//==========================================================
struct UICOM_1PP_FONT_INFO {
    u_int8 *data;
    u_int8 right;
    u_int8 left;
    u_int8 bottom;
    u_int8 width;
    u_int8 height;
};

//==========================================================
enum {
    INPUT_PROC_NULL,
    INPUT_PROC_NEW,
    INPUT_PROC_DEL,
    INPUT_PROC_MOVE,
    INPUT_PROC_UPDATE,
};
typedef u_int16 T_INPUT_EVENT;
#define INPUT_PROC_SET(type, subtype)  ((type)&0xFF | ((subtype)<<8))
#define INPUT_PROC_TYPE(type)          ((type)&0xFF)
#define INPUT_PROC_SUBTYPE(type)       ((type)>>8)
//==========================================================
// 1D array, list / vector
void uicom_obj_list(T_UICOM_COUNT len, T_UICOM_HANDL param, 
                          void (*paint)(T_UICOM_COUNT pos, T_UICOM_HANDL param), 
                          T_UICOM_ORDER paintOrder);
// 2D array, table /picture
void uicom_obj_tab(T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_HANDL param, 
                          void (*paint)(T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_HANDL param), 
                          T_UICOM_ORDER paintOrder);

// dymatic map
void uicom_obj_map(T_UICOM_COUNT start, T_UICOM_HANDL param, 
                            T_UICOM_COUNT (*getNext)(T_UICOM_COUNT *idx, T_UICOM_HANDL param), 
                            void (*paint)(T_UICOM_COUNT idx, T_UICOM_HANDL param),
                            T_UICOM_ORDER paintOrder);

u_int16 uicom_obj_input(T_INPUT_EVENT evt, u_int8 *data, u_int8 *pfocus,u_int8 maxnum, void *param);
u_int8 uicom_font_getdata(u_int8 *ch, u_int8 chlen ,struct UICOM_1PP_FONT_INFO *info, T_UICOM_FONT_SIZE fontsize);

#ifdef __cplusplus
}
#endif
#endif


