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

#define MAIN_LCD_WIDTH   (128)
#define MAIN_LCD_HEIGHT  (64)

struct GUI_MENU_ITEM {
    POSD_OBJ_HEAD *objs;
    void (*initobj)(T_UICOM_OBJ_COUNT idx, POSD_OBJ_HEAD obj);
    T_UICOM_OBJ_COUNT objsCount;
};

typedef struct GUI_MENU_ITEM *PGUI_MENU_ITEM;

#define DEF_MENU_ITEM(item, objsTab, initobj)   \
struct GUI_MENU_ITEM item = \
{  \
    objsTab, initobj, ARRAY_SIZE(objsTab)\
}
#define LDEF_MENU_ITEM(item, objsTab, initobj)   \
    static DEF_MENU_ITEM(item, objsTab, initobj)


enum ITEM_INIT_STATUS{
    ITEM_STATUS_NORMAL,
    PAINT_STATUS_SKIP_ALL,
    PAINT_STATUS_SKIP_BORDER,
    PAINT_STATUS_SKIP_DATA,
};
void gmenu_item_draw(PGUI_MENU_ITEM item,  enum OSD_OBJ_DRAW_TYPE type);
void gmenu_win_clear(struct SCREEN_ZONE *win);
void gmenu_item_clear(PGUI_MENU_ITEM item);

//==========================================================
struct GMENU_ITEM_LIST {
    struct GUI_MENU_ITEM    *list;
    void (*inititem)(T_UICOM_OBJ_COUNT idx, PGUI_MENU_ITEM item);
    T_UICOM_OBJ_COUNT         len;
    T_UICOM_OBJ_COUNT         curFocus;//[0, len-1]
};

#define DEF_MENU_ITEM_LIST(list, itemTab, inititem)   \
struct GMENU_ITEM_LIST list = \
{  \
    itemTab, inititem, ARRAY_SIZE(itemTab), 0\
}
#define LDEF_MENU_ITEM_LIST(list, itemTab, inititem)   \
    static DEF_MENU_ITEM_LIST(list, itemTab, inititem)


void gmenu_list_draw(struct GMENU_ITEM_LIST *list, u_int8 focus, u_int8 drawAttr);
void gmenu_list_item_draw(struct GMENU_ITEM_LIST *list, u_int8 pos, enum OSD_OBJ_DRAW_TYPE type);

//==========================================================
struct GMENU_CONTENT_TAB {
    void   (*initzone)(struct SCREEN_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);
    u_int8 (*inititem)(PUICOM_DATA item, u_int8 *strbuf, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);
    T_UICOM_OBJ_COUNT          rowCount;
    T_UICOM_OBJ_COUNT          colCount;
    T_UICOM_OBJ_COUNT          rowFocus;//[1, rowCount], if row==0, means focus row
    T_UICOM_OBJ_COUNT          colFocus;//[1, colCount], if col==0, means focus col
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

#ifdef __cplusplus
}
#endif
#endif

