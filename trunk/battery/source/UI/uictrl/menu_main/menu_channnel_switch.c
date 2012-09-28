#include "uimmi_ctrl.h"
#include "gui_menu_item.h"


#define THIS_MENU_NAME  "通道切换选择"
//"SwitchChSel"



#define CHANNELSW_NUM_ROW   4
#define CHANNELSW_NUM_COL   4

#define CHANNELSW_NUM_PER_PAGE   (CHANNELSW_NUM_COL*CHANNELSW_NUM_ROW)

//-----------------------------------------
#define CHANNELSW_LIST_NUM  (CHANNELSW_NUM_PER_PAGE+2)
#define CHNNELSW_NUM_LINE   CHANNELSW_NUM_ROW
#define CHANNELSW_OK_IDX    (CHANNELSW_NUM_PER_PAGE)
#define CHANNELSW_ESC_IDX   (CHANNELSW_NUM_PER_PAGE +1)

#define CHANNELSW_LIST_CELL_H  10
#define CHANNELSW_LIST_CELL_W  (126/CHANNELSW_NUM_COL)
#define CHANNELSW_LIST_X(x)   ((x)+1)
#define CHANNELSW_LIST_Y(y)   (((y)*(CHANNELSW_LIST_CELL_H))+10)
#define CHANNELSW_LIST_YY(y)  (CHANNELSW_LIST_Y(y)+2)

static u_int8 channelsw_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static u_int8 channelsw_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type);

LDEF_MENU_CONTENT_LIST(gSearchoptionsetupList, channelsw_menu_cell_zone_int, channelsw_menu_cell_data_int);

static u_int8 channelsw_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  row = 0, col;
    if (pos < CHANNELSW_NUM_PER_PAGE)
    {
        zone->border.b = 1;
        zone->border.r = 1;

        col = (pos / CHNNELSW_NUM_LINE);
        row = (pos % CHNNELSW_NUM_LINE);
        if (0 == col)
        {
            zone->border.l = 1;
        }
        if (0 == row)
        {
            zone->border.t = 1;
        }

        SCREEN_ZONE_INIT(&zone->zone, CHANNELSW_LIST_X((col*CHANNELSW_LIST_CELL_W)),
            CHANNELSW_LIST_Y(row),  CHANNELSW_LIST_CELL_W, CHANNELSW_LIST_CELL_H);
    }
    else if (CHANNELSW_OK_IDX == pos)
    {
        SCREEN_ZONE_INIT(&zone->zone, CHANNELSW_LIST_X(15),CHANNELSW_LIST_YY(CHNNELSW_NUM_LINE),  30,10);
        SCREEN_BORDER_INIT(&zone->border, 1, 1, 1, 1);
    }
    else if (CHANNELSW_ESC_IDX == pos)
    {
        SCREEN_ZONE_INIT(&zone->zone, CHANNELSW_LIST_X(61+15),CHANNELSW_LIST_YY(CHNNELSW_NUM_LINE),30,10);
        SCREEN_BORDER_INIT(&zone->border, 1, 1, 1, 1);
    }

    if (pos < CHANNELSW_NUM_PER_PAGE)
    {
        return 3;//通道X
    }
    else 
    {
        return 1;//确认//取消
    }
}

#define CHANNELSW_LIST_BOX_W  36
#define CHANNELSW_LIST_BOX_H  12

static u_int8 channelsw_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type)
{
    u_int8 colPosIdx = 0, status  = DRAW_MODE_TEXT_ONLY;
    struct SCREEN_ZONE colPosTable[3] = {
        {1,   2,  16, CHANNELSW_LIST_BOX_H},
        {12,  2,  50, CHANNELSW_LIST_BOX_H},
        {24,  2,  9,10},
    };
    
    colPosIdx = childIdx;
    //if ((2 == childIdx) && (pos < CHANNELSW_NUM_PER_PAGE))
    {
       // SCREEN_BORDER_INIT(&zone->border, 1, 1, 1, 1);
    }

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_SMALL_BLACK);

    if (pos < CHANNELSW_NUM_PER_PAGE)//通道
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_TONGDAO);
                break;
            case 1:
                sprintf(UICOM_DATA_BUF(item), "%02d", pos + 1);
                break;
            case 2:
                if (pos % 7)
                {
                    UICOM_DATA_FILL(item, UICOM_STR_YUANQUAN1);
                }
                else
                {
                    UICOM_DATA_FILL(item, NULL);
                }
                break;
            default:
                break;
        }

    }
    else if (CHANNELSW_OK_IDX == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                zone->zone.x += 6;
                zone->zone.y = 1;
                zone->border.t = 1;
                break;
            default:
                break;
        }
    }
    else if (CHANNELSW_ESC_IDX == pos)//取消
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_QUXIAO);
                zone->zone.x += 4;
                zone->zone.y = 1;
                zone->border.t = 1;
                break;
            default:
                break;
        }
    }
    
    return status;

}

static void channelsw_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&gSearchoptionsetupList,  0);
    }
    gmenu_content_list_draw(&gSearchoptionsetupList, CHANNELSW_LIST_NUM, 0);
}

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuChSwitch,
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

static void chnelsw_menu_tips(void)
{
    struct SCREEN_ZONE rect;
    
    SCREEN_ZONE_INIT(&rect, 2, 1, 128, 12);
    Screen_PrintString(&rect, UICOM_STR_TONGDAOXUANZETISHI, TEXT_SMALL_BLACK);

    SCREEN_ZONE_INIT(&rect, 70, 1, 128, 12);
    Screen_PrintString(&rect, UICOM_STR_TONGDAOXUANZETISHI1, TEXT_SMALL_BLACK);
}
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    ui_mmi_debug_handle(THIS_MENU_NAME, me, e);
    
    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        chnelsw_menu_tips();
        channelsw_menu_paint(0);
    }
    else 
    {
        u_int8 focus;
        
        switch (e->sig)
        {
            case EVENT_KEY_NUM_0:
            channelsw_menu_paint(1);
            break;
            case EVENT_KEY_RIGHT:
            case EVENT_KEY_DOWN:
            gmenu_content_list_movefocus(&gSearchoptionsetupList, 1, 1);
            break;
            case EVENT_KEY_UP:
                case EVENT_KEY_LEFT:
            gmenu_content_list_movefocus(&gSearchoptionsetupList, -1, 1);
            break;
            default:
            break;
        }
    }


    return SM_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




