#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "弹出确认框"
//"inputbox"


#define YESNO_POPMENU_W       100
#define YESNO_POPMENU_H       32

#define YESNO_POPMENU_X       ((128 - YESNO_POPMENU_W)/2)
#define YESNO_POPMENU_Y       ((64 - YESNO_POPMENU_H)/2)

#define INPUTBOX_LIST_NUM            (2)

#define INPUTBOX_LIST_X(x)    ((x) + (YESNO_POPMENU_X))
#define INPUTBOX_LIST_Y(y)    ((y) + (YESNO_POPMENU_Y))
 
static u_int8 yesno_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static u_int8 yesno_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type);

LDEF_MENU_CONTENT_LIST(gPopMenuYesNoMenu, yesno_menu_cell_zone_int, yesno_menu_cell_data_int);

static u_int8 yesno_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[INPUTBOX_LIST_NUM] = {
        {INPUTBOX_LIST_X(8),INPUTBOX_LIST_Y(16),  28,10},
        {INPUTBOX_LIST_X((YESNO_POPMENU_W/2) + 8),  INPUTBOX_LIST_Y(16),  28,10},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    
    return 1;
}


static u_int8 yesno_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type)
{
    u_int8 colPosIdx = 0, status  = DRAW_MODE_TEXT_ONLY;


    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_SMALL_BLACK);
    if (0 == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 6, 2, 60, 12);
                UICOM_DATA_FILL(item, UICOM_STR_YES);
                break;
            default:
                break;
        }
    }
    else if (1 == pos)//取消
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 6, 2, 60, 12);
                UICOM_DATA_FILL(item, UICOM_STR_NO);
                break;
            default:
                break;
        }
    }
    
    return status;

}

static void yesno_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&gPopMenuYesNoMenu,  0);
    }
    gmenu_content_list_draw(&gPopMenuYesNoMenu, INPUTBOX_LIST_NUM, 0);
}

static void yesno_menu_pop(u_int8 isClearBg)
{
    struct OSD_ZONE rect;
    
    SCREEN_ZONE_INIT(&rect.zone, YESNO_POPMENU_X, YESNO_POPMENU_Y, YESNO_POPMENU_W, YESNO_POPMENU_H);
    if (isClearBg)
    {
        Screen_PrintClear(&rect.zone);
    }
    
    SCREEN_BORDER_INIT(&rect.border, 1, 1, 1, 1);
    Screen_PrintRect(&rect, PIXEL_MODE_SET);

    SCREEN_ZONE_INIT(&rect.zone, INPUTBOX_LIST_X(4), INPUTBOX_LIST_Y(4), 32, 14);
    Screen_PrintString(&rect.zone, "Are You Sure?", TEXT_SMALL_BLACK);

}

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gPopMenuYesOrNoMenu,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);




static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    ui_mmi_debug_suspend(THIS_MENU_NAME, me, child);
}

static void menu_pub_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    ui_mmi_debug_resume(THIS_MENU_NAME, child, me);
}


static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    ui_mmi_debug_enter(THIS_MENU_NAME, parent, me);
    ui_mmi_reg_suspend(menu_pub_suspend);
    ui_mmi_reg_resume(menu_pub_resume);
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    ui_mmi_debug_handle(THIS_MENU_NAME, me, e);
    if (MSG_IS_ENTRY(e->sig))
    {
        yesno_menu_pop(1);
        yesno_menu_paint(0);
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
        yesno_menu_paint(1);
        break;
        case EVENT_KEY_RIGHT:
        case EVENT_KEY_DOWN:
        gmenu_content_list_movefocus(&gPopMenuYesNoMenu, 1, 1);
        break;
        case EVENT_KEY_UP:
        case EVENT_KEY_LEFT:
        gmenu_content_list_movefocus(&gPopMenuYesNoMenu, -1, 1);
        break;
        default:
            break;
        
    }

    return SM_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}


void gmenu_pop_yesno(struct gmenu_pop_yesno_param *param)
{

}


