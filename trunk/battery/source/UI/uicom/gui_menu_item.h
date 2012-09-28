/***********************************************************************
 *
 * MODULE NAME:  
 * DESCRIPTION:  menu item基本绘制
 * AUTHOR:     wangxi (中山医院岗顶)
 * DATE: 2012/09/11
 *
 ***********************************************************************/
#ifndef __GUI_MENU_ITEM_H__
#define __GUI_MENU_ITEM_H__
#include "common_define.h"
#include "gui_osd_obj.h"
#include "ui_com_obj.h"
#include "uimmi_content.h"

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------
#define DRAW_MODE_TEXT_BOX    (PAINT_FLAG_BORDER | PAINT_FLAG_DATA)
#define DRAW_MODE_TEXT_ONLY   (PAINT_FLAG_DATA)
#define DRAW_MODE_BOX_ONLY    (PAINT_FLAG_BORDER)
//----------------------------------------------------------------------
#define PAINT_MAP_NORAML         PAINT_MAP_LINE
#define PAINT_MAP_SKIP           PAINT_MAP_NULL
//==========================================================
typedef enum {
    GMENU_ACTION_NULL,
    GMENU_ACTION_FOCUS,
    GMENU_ACTION_ENTER,
} T_GMENU_ACTION;

//==========================================================
struct GMENU_CONTENT_LIST {
    // return child count
    T_UICOM_COUNT   (*initzone)(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
    // init children
    T_UICOM_PAINT_FLAG (*inititem)(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type);
    T_UICOM_COUNT   len;
    T_UICOM_COUNT   curFocus;
};

#define DEF_MENU_CONTENT_LIST(list, initzone, inititem)   \
struct GMENU_CONTENT_LIST list = \
{  \
    initzone, inititem, 1, 0\
}
#define LDEF_MENU_CONTENT_LIST(list, initzone, inititem)   \
    static DEF_MENU_CONTENT_LIST(list, initzone, inititem)


void gmenu_content_list_draw(struct GMENU_CONTENT_LIST *list, u_int8 len, u_int8 focus);
void gmenu_content_list_clear(struct GMENU_CONTENT_LIST *list, u_int8 pos1, u_int8 pos2, u_int8 isClearBorder);
void gmenu_content_list_clear_all(struct GMENU_CONTENT_LIST *list, u_int8 isClearBorder);
void gmenu_content_list_changefocus(struct GMENU_CONTENT_LIST *list, u_int8 focus);
u_int8 gmenu_content_list_getfocus(struct GMENU_CONTENT_LIST *list);
void gmenu_content_list_movefocus(struct GMENU_CONTENT_LIST *list, s_int8 step, bool_t isloop);
//==========================================================
struct GMENU_CONTENT_TAB {
    void   (*initzone)(struct OSD_ZONE *zone, T_UICOM_COUNT row, T_UICOM_COUNT col);
    T_UICOM_PAINT_FLAG (*inititem)(struct SCREEN_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, enum T_UICOM_STATUS type);
    T_UICOM_COUNT          rowCount;
    T_UICOM_COUNT          colCount;
    T_UICOM_COUNT          rowFocus;//[1, rowCount], if row==0, means focus row
    T_UICOM_COUNT          colFocus;//[1, colCount], if col==0, means focus col
};

#define DEF_MENU_CONTENT_TAB(tab, initzone, inititem)   \
struct GMENU_CONTENT_TAB tab = \
{  \
    initzone, inititem, 1, 1, 0, 0\
}
#define LDEF_MENU_CONTENT_TAB(tab, initzone, inititem)  \
    static DEF_MENU_CONTENT_TAB(tab, initzone, inititem)

void gmenu_content_tab_draw(struct GMENU_CONTENT_TAB *table, u_int8 row, u_int8 col, u_int8 rowfocus, u_int8 colfocus);
void gmenu_content_tab_clear_row(struct GMENU_CONTENT_TAB *table, u_int8 row1, u_int8 row2, u_int8 isClearBorder);
void gmenu_content_tab_clear_col(struct GMENU_CONTENT_TAB *table, u_int8 col1, u_int8 col2, u_int8 isClearBorder);
void gmenu_content_tab_changefocus(struct GMENU_CONTENT_TAB *table,  T_UICOM_COUNT row, T_UICOM_COUNT col);
bool_t gmenu_content_tab_getfocus(struct GMENU_CONTENT_TAB *table, T_UICOM_COUNT *row, T_UICOM_COUNT *col);
//==========================================================
struct GMENU_VALUE_MAP {
    T_UICOM_PAINT_FLAG (*init)(T_UICOM_COUNT group, T_UICOM_COUNT *key, T_UICOM_COUNT *y);      
    T_UICOM_COUNT steps;
    T_UICOM_COUNT curCh;//
    T_UICOM_COUNT count;
    T_UICOM_COUNT pre_y;
    T_UICOM_COUNT pre_x;
    T_UICOM_PAINT_FLAG paintflg;
};


#define DEF_MENU_VALUE_MAP(map, inititem)   \
struct GMENU_VALUE_MAP map = \
{  \
    inititem, 1, 0, 1, 0, 0, 0\
}
#define LDEF_MENU_VALUE_MAP(map, inititem)  \
    static DEF_MENU_VALUE_MAP(map, inititem)

void gmenu_value_map_draw(struct GMENU_VALUE_MAP *map, struct OSD_ZONE *zone,
                        T_UICOM_COUNT count, T_UICOM_COUNT unit,
                        T_UICOM_COUNT chCount) ;

//----------------------------------------------------------------------
struct gmenu_curve_param {
    void (*curvetitle)(PUICOM_DATA item);
    void (*curveXname)(PUICOM_DATA item);
    void (*curveYname)(PUICOM_DATA item);
    void (*curveOname)(PUICOM_DATA item);
};

void gmenu_show_curve(struct gmenu_curve_param *param);
//----------------------------------------------------------------------
struct gmenu_pop_yesno_param {
    void (*title)(PUICOM_DATA item);
    bool_t (*inputfinish)(bool_t isok);
    u_int8 defaultFocusIsOk;
};
void gmenu_pop_yesno(struct gmenu_pop_yesno_param *param);
//----------------------------------------------------------------------
enum {
    GMENU_INPUTBOX_TYPE_NULL,
    GMENU_INPUTBOX_TYPE_PASSWD,
    GMENU_INPUTBOX_TYPE_VALUE,
    GMENU_INPUTBOX_TYPE_NUMBER,
    GMENU_INPUTBOX_TYPE_CHAR,
};

struct gmenu_pop_input_param {
    u_int8 type;
    u_int8 maxlen;
    void (*title)(PUICOM_DATA item);
    bool_t (*inputfinish)(u_int8 *inputdata, u_int8 len);
};

void gmenu_pop_inputbox(struct gmenu_pop_input_param *param, u_int8 *defaulttext);
//----------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif

