#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "弹出输入框"
//"inputbox"


#define INPUTBOX_POPMENU_W       88
#define INPUTBOX_POPMENU_H       48

#define INPUTBOX_POPMENU_X       ((128 - INPUTBOX_POPMENU_W)/2)
#define INPUTBOX_POPMENU_Y       ((64 - INPUTBOX_POPMENU_H)/2)

#define INPUTBOX_LIST_NUM            (3)

#define INPUTBOX_LIST_X(x)    ((x) + (INPUTBOX_POPMENU_X))
#define INPUTBOX_LIST_Y(y)    ((y) + (INPUTBOX_POPMENU_Y))
 
static u_int8 inputbox_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT pos);
static u_int8 inputbox_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT pos, T_UICOM_OBJ_COUNT childIdx, enum OSD_OBJ_DRAW_TYPE type);

LDEF_MENU_CONTENT_LIST(gPopMenuInputBoxList, inputbox_menu_cell_zone_int, inputbox_menu_cell_data_int);

static u_int8 inputbox_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[INPUTBOX_LIST_NUM] = {
        {INPUTBOX_LIST_X(16),INPUTBOX_LIST_Y(14),    INPUTBOX_POPMENU_W-28,12},
        {INPUTBOX_LIST_X(8),INPUTBOX_LIST_Y(32),  28,10},
        {INPUTBOX_LIST_X((INPUTBOX_POPMENU_W/2) + 8),  INPUTBOX_LIST_Y(32),  28,10},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    
    return 1;
}


static u_int8 inputbox_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT pos, T_UICOM_OBJ_COUNT childIdx, enum OSD_OBJ_DRAW_TYPE type)
{
    u_int8 colPosIdx = 0, status  = PAINT_STATUS_TEXT_ONLY;


    UICOM_DATA_TEXT_ATTR(item, TEXT_SMALL_BLACK);

    if (0 == pos)// inputbox
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 1, 4, 60, 12);
                sprintf(UICOM_DATA_BUF(item), "%d", 19850909);
                break;
            default:
                break;
        }

    }

    else if (1 == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 6, 2, 60, 12);
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                break;
            default:
                break;
        }
    }
    else if (2 == pos)//取消
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 4, 2, 60, 12);
                UICOM_DATA_FILL(item, UICOM_STR_QUXIAO);
                break;
            default:
                break;
        }
    }
    
    return status;

}

static void inputbox_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear(&gPopMenuInputBoxList, 1, INPUTBOX_LIST_NUM-1, 0);
    }
    gmenu_content_list_draw(&gPopMenuInputBoxList, INPUTBOX_LIST_NUM, 0);
}

static void inputbox_menu_pop(u_int8 isClearBg)
{
    struct OSD_ZONE rect;
    
    SCREEN_ZONE_INIT(&rect.zone, INPUTBOX_POPMENU_X, INPUTBOX_POPMENU_Y, INPUTBOX_POPMENU_W, INPUTBOX_POPMENU_H);
    if (isClearBg)
    {
        Screen_PrintClear(&rect.zone);
    }
    
    SCREEN_BORDER_INIT(&rect.border, 1, 1, 1, 1);
    Screen_PrintRect(&rect, PIXEL_MODE_SET);

    SCREEN_ZONE_INIT(&rect.zone, INPUTBOX_LIST_X(4), INPUTBOX_LIST_Y(4), 32, 14);
    Screen_PrintString(&rect.zone, "Input:", TEXT_SMALL_BLACK);

}

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gPopMenuInputBoxMenu,
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

static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    ui_mmi_debug_handle(THIS_MENU_NAME, me, e);
    if (MSG_IS_ENTRY(e->sig))
    {
        inputbox_menu_pop(1);
        inputbox_menu_paint(0);
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            inputbox_menu_paint(1);
            break;
        default:
            break;
        
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




