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

//==========================================================
struct GMENU_CONTENT_LIST {
    // return child count
    T_UICOM_OBJ_COUNT   (*initzone)(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT pos);
    // init children
    T_UICOM_PAINT_FLAG (*inititem)(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT pos, T_UICOM_OBJ_COUNT childIdx, enum T_UICOM_STATUS type);
    T_UICOM_OBJ_COUNT   len;
    T_UICOM_OBJ_COUNT   curFocus;
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

//==========================================================
struct GMENU_CONTENT_TAB {
    void   (*initzone)(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);
    T_UICOM_PAINT_FLAG (*inititem)(struct SCREEN_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, enum T_UICOM_STATUS type);
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
void gmenu_content_tab_clear_col(struct GMENU_CONTENT_TAB *table, u_int8 col1, u_int8 col2, u_int8 isClearBorder);
//==========================================================

#ifdef __cplusplus
}
#endif
#endif

