#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "查询选项"
//"SearchOption"



#define SEARCHOPT_LIST_NUM  (4)

#define SEARCHOPT_LIST_CELL_H  14
#define SEARCHOPT_LIST_X(x)   (x+2)
#define SEARCHOPT_LIST_Y(y)   ((y*(SEARCHOPT_LIST_CELL_H+4))+4)
#define SEARCHOPT_LIST_YY(y)   (SEARCHOPT_LIST_Y(y)+4)

static u_int8 searchopt_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT pos);
static u_int8 searchopt_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT pos, T_UICOM_OBJ_COUNT childIdx, enum T_UICOM_STATUS type);

LDEF_MENU_CONTENT_LIST(gSearchoptionsetupList, searchopt_menu_cell_zone_int, searchopt_menu_cell_data_int);

static u_int8 searchopt_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[SEARCHOPT_LIST_NUM] = {
        {SEARCHOPT_LIST_X(0),SEARCHOPT_LIST_Y(0),  124,SEARCHOPT_LIST_CELL_H},
        {SEARCHOPT_LIST_X(0),SEARCHOPT_LIST_Y(1),  124,SEARCHOPT_LIST_CELL_H},
        {SEARCHOPT_LIST_X(15)   ,SEARCHOPT_LIST_YY(2),  30,12},
        {SEARCHOPT_LIST_X(61+15),SEARCHOPT_LIST_YY(2),  30,12},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    switch (pos)
    {
        case 0://显示第几条曲线
            count =  3;
            break;
        case 1://一次显示曲线数
            count =  3;
            break;
        case 2://确认
            count =  1;
            break;
        case 3://取消
            count =  1;
        default:
            break;
    }
    
    return count;
}

#define SEARCHOPT_LIST_BOX_W  36
#define SEARCHOPT_LIST_BOX_H  10

static u_int8 searchopt_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT pos, T_UICOM_OBJ_COUNT childIdx, enum T_UICOM_STATUS type)
{
    u_int8 colPosIdx = 0, status  = DRAW_MODE_TEXT_ONLY;
    struct SCREEN_ZONE colPosTable[5] = {
        {1,  2,  60,SEARCHOPT_LIST_BOX_H},
        {50, 2,  24,SEARCHOPT_LIST_BOX_H},
        {55, 2,  16,SEARCHOPT_LIST_BOX_H},
        {80, 2,  SEARCHOPT_LIST_BOX_W,SEARCHOPT_LIST_BOX_H},
        {80, 2,  10,10},
    };
    

    SCREEN_BORDER_INIT(&zone->border, 1, 2, 1, 1);
    switch (childIdx)
    {
        case 0:
            colPosIdx = 0;
            break;
        case 1:
            if (pos == 0)
            {
                colPosIdx = 1;
                status = DRAW_MODE_TEXT_BOX;
            }
            else if (pos == 1)
            {
                colPosIdx = 2;
                status = DRAW_MODE_TEXT_BOX;
            }
            break;
        case 2:
            if (pos == 0)
            {
                colPosIdx = 3;
            }
            else if (pos == 1)
            {
                colPosIdx = 4;
            }
            break;
        default:
            break;
    }

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR(item, TEXT_SMALL_BLACK);

    if (0 == pos)//显示第几条曲线
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_XIANSHIDI);
                break;
            case 1:
                sprintf(UICOM_DATA_BUF(item), "%d", 100);
                break;
            case 2:
                UICOM_DATA_FILL(item, UICOM_STR_FANGDAINQUXIAN);
                break;
            default:
                break;
        }

    }
    else if (1 == pos)//一次显示曲线数
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_YICIXIANSHISHU);
                break;
            case 1:
                sprintf(UICOM_DATA_BUF(item), "%02d", 3);
                break;
            case 2:
                sprintf(UICOM_DATA_BUF(item), "%s(<4)", UICOM_STR_DANWEITIAO);
                break;
            default:
                break;
        }

    }
    else if (2 == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                zone->zone.x += 6;
                zone->border.t = 1;
                break;
            default:
                break;
        }
    }
    else if (3 == pos)//取消
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_QUXIAO);
                zone->zone.x += 4;
                zone->border.t = 1;
                break;
            default:
                break;
        }
    }
    
    return status;

}

static void searchoptsetup_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear(&gSearchoptionsetupList, 1, SEARCHOPT_LIST_NUM-1, 0);
    }
    gmenu_content_list_draw(&gSearchoptionsetupList, SEARCHOPT_LIST_NUM, 0);
}


static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuSearchOption,
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
        Screen_PrintClear(NULL);
        searchoptsetup_menu_paint(0);
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            searchoptsetup_menu_paint(1);
            break;
        default:
            break;
        
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




