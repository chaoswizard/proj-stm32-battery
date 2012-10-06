/***********************************************************************
 *
 * MODULE NAME:  
 * DESCRIPTION:  osd obj基本绘制
 * AUTHOR:     wangxi (中山医院岗顶)
 * DATE: 2012/09/11
 *
 ***********************************************************************/
#ifndef __GUI_OSD_OBJ_H__
#define __GUI_OSD_OBJ_H__
#include "brush_print.h"
#include "ui_com_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

//==========================================================
enum OSD_OBJ_TYPE {
    OSD_OBJ_RECT,//矩形框
    OSD_OBJ_CONTENT,//数据: 文本，图片 
    OSD_OBJ_BUTTON,//按钮(RECT + TEXT)
};


//----------------------------------------------------------------------
// type: obj types
// attr: [ other | color | align ]
#define _OSD_OBJECT_HEADER_  \
    enum OSD_OBJ_TYPE    type;\
    u_int8 attr

struct OSD_OBJ_HEAD {
    _OSD_OBJECT_HEADER_;
};

typedef struct OSD_OBJ_HEAD *POSD_OBJ_HEAD;

//==========================================================

enum OSD_OBJ_ALIGN_TYPE {
    OSD_OBJ_ALIGN_LEFT,
    OSD_OBJ_ALIGN_RIGHT,
    OSD_OBJ_ALIGN_TOP,
    OSD_OBJ_ALIGN_BOTTOM,
    OSD_OBJ_ALIGN_H_CENTER,
    OSD_OBJ_ALIGN_V_CENTER,
};

struct OSD_OBJ_RECT_INFO {
    _OSD_OBJECT_HEADER_;
    struct OSD_ZONE rect;
};



struct OSD_OBJ_CONTENT_INFO {
    _OSD_OBJECT_HEADER_;
    struct OSD_ZONE rect;
    struct UICOM_DATA content;
};

struct OSD_OBJ_BUTTON_INFO {
    _OSD_OBJECT_HEADER_;
    struct OSD_ZONE rect;  
    struct UICOM_DATA content;
};


union OSD_OBJ_UNION {
    struct OSD_OBJ_RECT_INFO     rect;
    struct OSD_OBJ_CONTENT_INFO  content;
    struct OSD_OBJ_BUTTON_INFO   button;
};

//----------------------------------------------------------------------
#define DEF_OSD_RECT(rect, x, y, w, h,  attr)   \
struct OSD_OBJ_RECT_INFO rect = \
{  \
    OSD_OBJ_RECT, attr,\
    {{x, y, w, h},{1, 1, 1, 1}}\
}
#define LDEF_OSD_RECT(rect, x, y, w, h,  attr)   \
    static DEF_OSD_RECT(rect, x, y, w, h,  attr)
//----------------------------------------------------------------------
#define DEF_OSD_TEXT(text, x, y, w, h)   \
struct OSD_OBJ_CONTENT_INFO text = \
{  \
    OSD_OBJ_CONTENT, 0,\
    {{x, y, w, h},{0, 0, 0, 0}},\
    {UICOM_DATA_TYPE_TEXT, NULL}\
}

#define LDEF_OSD_TEXT(text, x, y, w, h)   \
    static DEF_OSD_TEXT(text, x, y, w, h)

#define OSD_TEXT_INIT(text, str)   UICOM_DATA_FILL((&((text)->content)), str)

#define DEF_OSD_EDIT_TEXT(text, x, y, w, h)   \
struct OSD_OBJ_CONTENT_INFO text = \
{  \
    OSD_OBJ_CONTENT, 0,\
    {{x, y, w, h},{1, 1, 1, 1}},\
    {UICOM_DATA_TYPE_TEXT, NULL}\
}

#define LDEF_OSD_EDIT_TEXT(text, x, y, w, h)   \
    static DEF_OSD_EDIT_TEXT(text, x, y, w, h)
//----------------------------------------------------------------------

#define DEF_OSD_BUTTON_TEXT(button, x, y, w, h, text, attr)   \
struct OSD_OBJ_BUTTON_INFO button = \
{  \
    OSD_OBJ_BUTTON, attr,\
    {{x, y, w, h},{1, 1, 1, 1}},\
    {UICOM_DATA_TYPE_TEXT, text}\
}

#define LDEF_OSD_BUTTON_TEXT(button, x, y, w, h, text, attr)   \
    static DEF_OSD_BUTTON_TEXT(button, x, y, w, h, text, attr)

#define OSD_BUTTON_TEXT_SET(button, str)   UICOM_DATA_FILL((&((button)->content)), str)
//----------------------------------------------------------------------
#define DEF_OSD_BUTTON_PIC(button, x, y, w, h, text, attr)   \
struct OSD_OBJ_BUTTON_INFO button = \
{  \
    OSD_OBJ_BUTTON, attr,\
    {{x, y, w, h},{1, 1, 1, 1}},\
    {UICOM_DATA_TYPE_PICTURE, text}\
}

#define LDEF_OSD_BUTTON_PIC(button, x, y, w, h, text, attr)   \
    static DEF_OSD_BUTTON_PIC(button, x, y, w, h, text, attr)

//==========================================================
void gui_osd_obj_draw(POSD_OBJ_HEAD obj, enum T_UICOM_STATUS drawType);
void gui_osd_obj_clear(POSD_OBJ_HEAD obj);
void gui_osd_rect_draw(struct OSD_OBJ_RECT_INFO *rect, enum T_UICOM_STATUS drawType);
void gui_osd_content_draw(struct OSD_OBJ_CONTENT_INFO *content, enum T_UICOM_STATUS drawType);
void gui_osd_button_draw(struct OSD_OBJ_BUTTON_INFO *button, enum T_UICOM_STATUS drawType);
void gui_osd_data_draw(struct UICOM_DATA *data, struct SCREEN_ZONE *zone);

//==========================================================
#ifdef __cplusplus
}
#endif
#endif


