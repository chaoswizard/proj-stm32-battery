#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "监测模式设置选项"
//"MonitorModeCfg"




#define MONITORSETUP_LIST_NUM  (5)

#define MONITORSET_LIST_CELL_H  14
#define MONITORSET_LIST_X(x)   (x+2)
#define MONITORSET_LIST_Y(y)   ((y*(MONITORSET_LIST_CELL_H+2))+1)
#define MONITORSET_LIST_YY(y)   (MONITORSET_LIST_Y(y)+1)

static u_int8 modesetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static u_int8 modesetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type);

LDEF_MENU_CONTENT_LIST(gCheckModeSetupMenu, modesetup_menu_cell_zone_int, modesetup_menu_cell_data_int);

static u_int8 modesetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[MONITORSETUP_LIST_NUM] = {
        {MONITORSET_LIST_X(0),MONITORSET_LIST_Y(0),  124,MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(0),MONITORSET_LIST_Y(1),  124,MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(0),MONITORSET_LIST_Y(2),  124,MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(15)   ,MONITORSET_LIST_YY(3),  30,12},
        {MONITORSET_LIST_X(61+15),MONITORSET_LIST_YY(3),  30,12},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    switch (pos)
    {
        case 0://按时间
            count =  4;
            break;
        case 1://按电压
            count =  4;
            break;
        case 2://混合模式
            count =  2;
            break;
        case 3://确认
            count =  1;
            break;
        case 4://取消
            count =  1;
        default:
            break;
    }
    
    return count;
}

#define MONITORSET_LIST_BOX_W  36
#define MONITORSET_LIST_BOX_H  10


static void modesetup_menu_option_on(PUICOM_DATA item, bool_t isOn)
{
    if (isOn)
    {
        UICOM_DATA_FILL(item, UICOM_STR_YUANQUAN);
    }
    else
    {
        UICOM_DATA_FILL(item, NULL);
    }
}

static u_int8 modesetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type)
{
    u_int8 colPosIdx = 0, status  = DRAW_MODE_TEXT_ONLY;
    struct SCREEN_ZONE colPosTable[4] = {
        {1,   2,  MONITORSET_LIST_BOX_W,MONITORSET_LIST_BOX_H},
        {48,  2,  MONITORSET_LIST_BOX_W,MONITORSET_LIST_BOX_H},
        {84,  2,  MONITORSET_LIST_BOX_W,MONITORSET_LIST_BOX_H},
        {110, 2,  10,10},
    };
    
    SCREEN_BORDER_INIT(&zone->border, 1, 2, 1, 1);
    switch (childIdx)
    {
        case 0:
            colPosIdx = 0;
            break;
        case 1:
            if ((pos == 0) || (pos == 1))
            {
                colPosIdx = 1;
                status = DRAW_MODE_TEXT_BOX;
            }
            else if (pos == 2)
            {
                colPosIdx = 3;
                zone->border.l = 2;
                status = DRAW_MODE_TEXT_BOX;
            }
            break;
        case 2:
            if ((pos == 0) || (pos == 1))
            {
                colPosIdx = 2;
            }
            break;
        case 3:
            if ((pos == 0) || (pos == 1))
            {
                colPosIdx = 3;
                zone->border.l = 2;
                status = DRAW_MODE_TEXT_BOX;
            }
            break;
        default:
            break;
    }

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_SMALL_BLACK);

    if (0 == pos)//按时间
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_ANSHIJIAN);
                break;
            case 1:
                sprintf(UICOM_DATA_BUF(item), "%02d", childIdx);
                break;
            case 2:
                UICOM_DATA_FILL(item, UICOM_STR_DANWEIFENZHONG);
                break;
            case 3:
                modesetup_menu_option_on(item, UICOM_IS_FOCUS(type));
                break;
            default:
                break;
        }

    }
    else if (1 == pos)//按电压
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_ANDIANYA);
                break;
            case 1:
                sprintf(UICOM_DATA_BUF(item), "%02d", pos);
                break;
            case 2:
                UICOM_DATA_FILL(item, UICOM_STR_DANWEIHAOFU);
                break;
            case 3:
                modesetup_menu_option_on(item, UICOM_IS_FOCUS(type));
                break;
            default:
                break;
        }

    }
    else if (2 == pos)//混合模式
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_HUNHEMOSHI);
                break;
            case 1:
                modesetup_menu_option_on(item, UICOM_IS_FOCUS(type));
                break;
            default:
                break;
        }
    }
    else if (3 == pos)//确认
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
    else if (4 == pos)//取消
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

static void monitorsetup_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&gCheckModeSetupMenu, 0);
    }
    gmenu_content_list_draw(&gCheckModeSetupMenu, MONITORSETUP_LIST_NUM, 0);
}



static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuCheckModeCfg,
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
        Screen_PrintClear(NULL);
        monitorsetup_menu_paint(0);
    }
    else
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            monitorsetup_menu_paint(1);
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




