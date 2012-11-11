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
#define DRAW_MODE_SKIP        (PAINT_FLAG_EMPTY)
#define DRAW_MODE_TEXT_BOX    (PAINT_FLAG_BORDER | PAINT_FLAG_DATA)
#define DRAW_MODE_TEXT_ONLY   (PAINT_FLAG_DATA)
#define DRAW_MODE_BOX_ONLY    (PAINT_FLAG_BORDER)
#define DRAW_MODE_CLEAR           (PAINT_FLAG_CLEAR)

//----------------------------------------------------------------------
#define DRAW_MODE_MAP_LINE    (PAINT_FLAG_MAP_LINE) 
//----------------------------------------------------------------------
#define TEXT_STYLE_LARGE_TRANS     TEXT_ATTR(FONT_SIZE_NORMAL, PIXEL_MODE_TRANS, PIXEL_MODE_TURN)
#define TEXT_STYLE_LARGE_SOLID     TEXT_ATTR(FONT_SIZE_NORMAL, PIXEL_MODE_CLEAR, PIXEL_MODE_SET)
#define TEXT_STYLE_SMALL_TRANS     TEXT_ATTR(FONT_SIZE_SMALL, PIXEL_MODE_TRANS,  PIXEL_MODE_TURN)
#define TEXT_STYLE_SMALL_SOLID     TEXT_ATTR(FONT_SIZE_SMALL, PIXEL_MODE_CLEAR,  PIXEL_MODE_SET)
#define TEXT_ORDER_LEFT_RIGHT      TEXT_ATTR_STR_LAYOUT(RECT_DIR_X_LR_Y_TB)
#define TEXT_ORDER_RIGHT_LEFT      TEXT_ATTR_STR_LAYOUT(RECT_DIR_X_RL_Y_TB)
#define TEXT_ORDER_TOP_BOTTOM      TEXT_ATTR_STR_LAYOUT(RECT_DIR_X_LR_Y_TB)
#define TEXT_ORDER_BOTTOM_TOP      TEXT_ATTR_STR_LAYOUT(RECT_DIR_X_RL_Y_BT)
//----------------------------------------------------------------------
#define TEXT_STYLE_DEFAULT         TEXT_STYLE_LARGE_TRANS
#define TEXT_ORDER_DEFAULT         TEXT_ORDER_LEFT_RIGHT
//==========================================================
#define DRAW_EVENT_INIT(type, subtype)     (((type)<<8) | ((subtype)&0xFF))
#define DRAW_EVENT_TYPE(type)              (((type)>>8)&0xFF)
#define DRAW_EVENT_SUBTYPE(type)           ((type)&0xFF)
//==========================================================

struct GMENU_CONTENT_LIST {
    // return child count
    T_UICOM_COUNT   (*initzone)(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
    // init children
    T_UICOM_DRAW_MODE (*inititem)(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *type);
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


#define LIST_EVENT_IS_NEXT(_msg_) ((EVENT_KEY_DOWN == (_msg_)) || (EVENT_KEY_RIGHT == (_msg_)))
void gmenu_content_list_draw(struct GMENU_CONTENT_LIST *list, u_int8 len, u_int8 focus);
void gmenu_content_list_clear(struct GMENU_CONTENT_LIST *list, u_int8 pos1, u_int8 pos2, u_int8 isClearBorder);
void gmenu_content_list_clear_all(struct GMENU_CONTENT_LIST *list, u_int8 isClearBorder);
void gmenu_content_list_changefocus(struct GMENU_CONTENT_LIST *list, u_int8 focus);
u_int8 gmenu_content_list_getfocus(struct GMENU_CONTENT_LIST *list);
void gmenu_content_list_movefocus(struct GMENU_CONTENT_LIST *list, s_int8 step, bool_t isloop);
void gmenu_content_list_update(struct GMENU_CONTENT_LIST *list, u_int8 focus,  u_int16 type);
void gmenu_content_list_cell_clear(struct GMENU_CONTENT_LIST *list, u_int8 pos, u_int8 isClearBorder);

//==========================================================
struct GMENU_CONTENT_TAB {
    void   (*initzone)(struct OSD_ZONE *zone, T_UICOM_COUNT row, T_UICOM_COUNT col);
    T_UICOM_DRAW_MODE (*inititem)(struct SCREEN_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_EVENT *type);
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
struct GMENU_MAP_PEN {
    T_UICOM_COUNT pre_y;
    T_UICOM_COUNT pre_x;
    T_UICOM_DRAW_MODE paintflg;
};
struct GMENU_VALUE_MAP {
    T_UICOM_DRAW_MODE (*mapinit)(T_UICOM_COUNT group, T_UICOM_COUNT *key, T_UICOM_COUNT *y);      
    T_UICOM_COUNT mapcount;
    struct GMENU_MAP_PEN *penTbl;
    T_UICOM_COUNT penCount;
    u_int32 penSwitchBitmap;
};


#define DEF_MENU_VALUE_MAP(map, inititem)   \
struct GMENU_VALUE_MAP map = \
{  \
    inititem, 0, NULL, 0, 0\
}
#define LDEF_MENU_VALUE_MAP(map, inititem)  \
    static DEF_MENU_VALUE_MAP(map, inititem)

void gmenu_value_map_draw(struct GMENU_VALUE_MAP *map,
                        T_UICOM_COUNT count, T_UICOM_COUNT penCount, u_int8 penswitch);


void gmenu_ruler_draw(struct OSD_ZONE *zone, enum PIXEL_COLOR pixel_mode, T_UICOM_DRAW_MODE (*initname_cbf)(T_UICOM_ORDER, u_int16, struct OSD_ZONE *, PUICOM_DATA));
//----------------------------------------------------------------------
struct gmenu_curve_config {
    u_int32 maxVal;
    u_int32 (*getMaxKey)(T_UICOM_COUNT group);
    u_int8 initCount;
    bool_t autoLoad;
    u_int8 (*fun)(T_UICOM_COUNT group, u_int32 x, u_int8 maxY, u_int8 rulerY);
    void (*curvetitle)(T_UICOM_COUNT grp, PUICOM_DATA item);
    void (*curveXname)(PUICOM_DATA item);
    void (*curveYname)(PUICOM_DATA item);
};

void gmenu_show_curve(struct gmenu_curve_config *param, bool_t pop);
//----------------------------------------------------------------------
struct gmenu_pop_yesno_param {
    void (*title)(PUICOM_DATA item);
    bool_t (*inputfinish)(bool_t isok);
    u_int8 defaultFocusIsOk;
};
void gmenu_pop_yesno(struct gmenu_pop_yesno_param *param);
//----------------------------------------------------------------------
enum GMENU_INPUT_TYPE {
    GMENU_INPUT_TYPE_CHAR = 0,//default type
    GMENU_INPUT_TYPE_VALUE,
    GMENU_INPUT_TYPE_PASSWD,
    GMENU_INPUT_TYPE_NUMBER,
};

//响应事件
#define INPUT_STATUS_KEY_ENTER    (1<<0)
#define INPUT_STATUS_FOCUS_LEAVE  (1<<1)
#define INPUT_STATUS_LEN_LIMIT    (1<<2)
#define INPUT_STATUS_DEL_ALL      (1<<3)
#define INPUT_STATUS_DEL_EMPTY    (1<<4)
#define INPUT_STATUS_ANY_EVENT    (0xFF)

//数据交换状态和响应状态
#define INPUT_CHECK_STATUS(exstatus, keystatus)  (((exstatus)<<8)| keystatus)
#define INPUT_EXCHANGE_STATUS(status)    ((status)>>8)
#define INPUT_BYPASS_STATUS(status)      ((status)&0xFF)

// 拒绝数据交换的事件和不响应的事件
#define INPUT_STATUS_EXCLUDE(status, evtreject, evtbypass)  (status&evtreject)?INPUT_CHECK_STATUS(0, evtbypass):INPUT_CHECK_STATUS(1, evtbypass)

//---------------------------------------------------------------------
#define INPUT_CHECK_DEFAULT  (INPUT_STATUS_FOCUS_LEAVE | INPUT_STATUS_KEY_ENTER | INPUT_STATUS_DEL_ALL | INPUT_STATUS_DEL_EMPTY)
#define INPUT_CHECK_LIMIT    (INPUT_CHECK_DEFAULT | INPUT_STATUS_LEN_LIMIT)

#define INPUT_IF_DEFAULT(type)    (0 != ((type)&INPUT_CHECK_DEFAULT))
#define INPUT_IF_LIMIT(type)      (0 != ((type)&INPUT_STATUS_LEN_LIMIT))
#define INPUT_IF_ENTER(type)      (0 != ((type)&INPUT_STATUS_KEY_ENTER))
#define INPUT_IF_DELALL(type)     (0 != ((type)&INPUT_STATUS_DEL_ALL))

#define INPUT_BYPASS_EVT_PUBLIC  (INPUT_STATUS_FOCUS_LEAVE | INPUT_STATUS_KEY_ENTER)
#define INPUT_STATUS_DEFAULT_ECHO(checktype) INPUT_STATUS_EXCLUDE(checktype, INPUT_STATUS_DEL_ALL, INPUT_STATUS_DEL_EMPTY | INPUT_BYPASS_EVT_PUBLIC)
#define INPUT_STATUS_NO_AUTOQUIT_ECHO(checktype)  INPUT_STATUS_EXCLUDE(checktype, 0,  INPUT_BYPASS_EVT_PUBLIC)
//---------------------------------------------------------------------


struct gmenu_input_config {
    enum GMENU_INPUT_TYPE type;
    bool_t isinsert;
    p_void param;
    u_int8 limitlen;
    //void (*title)(PUICOM_DATA item);//just use in pop menu
    u_int8 checktype;
    u_int16 (*inputdetector)(u_int8 checktype, p_void inputdata, u_int8 len, p_void usrparam);
};

struct gmenu_input_param {
    p_void initdata;
    bool_t isfocustail;
};


struct GMENU_INPUT {
    struct gmenu_input_config cfg;
    bool_t isInit;//0:close,1:open;, check the input whether first init
    u_int8 curfocus;
    u_int8 *charbuffer;
    u_int8 maxlen;
    u_int8 curcharnum;
    u_int8 appendflg;
};

//#define DEF_MENU_CHAR_INPUT(input, inititem)   \
//struct GMENU_INPUT input = \
//{  \
//    0, 1, 0, 1, 0, 0, 0\
//}
#define LDEF_MENU_VALUE_MAP(map, inititem)  \
    static DEF_MENU_VALUE_MAP(map, inititem)


bool_t gmenu_input_open(struct gmenu_input_config *cfg, struct gmenu_input_param *param, bool_t popMode);
bool_t gmenu_input_proc(T_INPUT_EVENT evt, p_void param);
u_int8 gmenu_input_makestr(enum GMENU_INPUT_TYPE type, u_int8 *buffer,p_void data);
u_int8 gmenu_input_getstr(u_int8 *buffer);
bool_t gmenu_input_getstatus(void_t);
void gmenu_input_close(void);
//----------------------------------------------------------------------
bool_t gmenu_input_default_proc(struct EVENT_NODE_ITEM *e);
//----------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif

