#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "存储选项"
//"StoreOption"




#define STORESETUP_LIST_NUM  (5)

#define STORESETUP_LIST_CELL_H  14
#define STORESETUP_LIST_X(x)   (x+2)
#define STORESETUP_LIST_Y(y)   ((y*(STORESETUP_LIST_CELL_H+2))+1)
#define STORESETUP_LIST_YY(y)   (STORESETUP_LIST_Y(y)+1)

static u_int8 storesetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT pos);
static u_int8 storesetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT pos, T_UICOM_OBJ_COUNT childIdx, enum OSD_OBJ_DRAW_TYPE type);

LDEF_MENU_CONTENT_LIST(gStoreOptionSetupList, storesetup_menu_cell_zone_int, storesetup_menu_cell_data_int);

static u_int8 storesetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[STORESETUP_LIST_NUM] = {
        {STORESETUP_LIST_X(0),STORESETUP_LIST_Y(0),  124,STORESETUP_LIST_CELL_H},
        {STORESETUP_LIST_X(0),STORESETUP_LIST_Y(1),  124,STORESETUP_LIST_CELL_H},
        {STORESETUP_LIST_X(0),STORESETUP_LIST_Y(2),  124,STORESETUP_LIST_CELL_H},
        {STORESETUP_LIST_X(15)   ,STORESETUP_LIST_YY(3),  30,12},
        {STORESETUP_LIST_X(61+15),STORESETUP_LIST_YY(3),  30,12},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    switch (pos)
    {
        case 0://存储介质
            count =  3;
            break;
        case 1://最大存储量为
            count =  3;
            break;
        case 2://可否覆盖
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

#define STORESETUP_LIST_BOX_W  36
#define STORESETUP_LIST_BOX_H  10

static u_int8 storesetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_OBJ_COUNT pos, T_UICOM_OBJ_COUNT childIdx, enum OSD_OBJ_DRAW_TYPE type)
{
    u_int8 colPosIdx = 0 , status  = PAINT_STATUS_TEXT_ONLY;
    struct SCREEN_ZONE colPosTable[6] = {
        {1,   2,  60,STORESETUP_LIST_BOX_H},
        {40,  2,  50,STORESETUP_LIST_BOX_H},
        {48,  2,  16,STORESETUP_LIST_BOX_H},
        {66,  2,  STORESETUP_LIST_BOX_W,STORESETUP_LIST_BOX_H},
        {90,  2,  STORESETUP_LIST_BOX_W,STORESETUP_LIST_BOX_H},
        {110, 2,  10,10},
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
                status = PAINT_STATUS_TEXT_BOX;
            }
            else if (pos == 1)
            {
                colPosIdx = 2;
                status = PAINT_STATUS_TEXT_BOX;
            }
            else if (pos == 2)
            {
                colPosIdx = 5;
                status = PAINT_STATUS_TEXT_BOX;
            }
            break;
        case 2:
            if (pos == 0)
            {
                colPosIdx = 4;
            }
            else if (pos == 1)
            {
                colPosIdx = 3;
            }
            break;
        default:
            break;
    }

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR(item, TEXT_SMALL_BLACK);

    if (0 == pos)//存储介质
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_CUNCHUJIEZHI);
                break;
            case 1:
                UICOM_DATA_FILL(item, UICOM_STR_MEDIASPI);
                break;
            case 2:
                sprintf(UICOM_DATA_BUF(item), "(%s)", UICOM_STR_MEDIASPI);
                break;
            default:
                break;
        }

    }
    else if (1 == pos)//最大存储量为
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_ZUIDACUNCHULIANG);
                break;
            case 1:
                sprintf(UICOM_DATA_BUF(item), "%02d", 99);
                break;
            case 2:
                sprintf(UICOM_DATA_BUF(item), "%s(>1)", UICOM_STR_DANWEIZU);
                break;
            default:
                break;
        }

    }
    else if (2 == pos)//可否覆盖
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_FUGAIMA);
                break;
            case 1:
                UICOM_DATA_FILL(item, UICOM_STR_YUANQUAN);
                zone->border.l += 1;
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

static void storageoptsetup_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear(&gStoreOptionSetupList, 1, STORESETUP_LIST_NUM-1, 0);
    }
    gmenu_content_list_draw(&gStoreOptionSetupList, STORESETUP_LIST_NUM, 0);
}



static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gStoreOptionSetupMenu,
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
        storageoptsetup_menu_paint(0);
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            storageoptsetup_menu_paint(1);
            break;
        default:
            break;
        
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




