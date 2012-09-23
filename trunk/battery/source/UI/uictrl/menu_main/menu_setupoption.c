#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "设置选项"

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuSetupOption,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);



#define  SETUPMENU_TAB_ROW_NUM   4
#define  SETUPMENU_TAB_COL_NUM   2

#define SETUPOPTTAB_CELL_H  12
#define SETUPOPTTAB_X(x)   (x+1)
#define SETUPOPTTAB_Y(y)   ((y*(SETUPOPTTAB_CELL_H+2))+4)
static void setupmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col)
{
    struct SCREEN_ZONE cellPosTable[SETUPMENU_TAB_ROW_NUM][SETUPMENU_TAB_COL_NUM] = {
        {{SETUPOPTTAB_X(0),SETUPOPTTAB_Y(0),  61,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63),SETUPOPTTAB_Y(0), 61,SETUPOPTTAB_CELL_H}},
        {{SETUPOPTTAB_X(0),SETUPOPTTAB_Y(1), 61,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63),SETUPOPTTAB_Y(1), 61,SETUPOPTTAB_CELL_H}},
        {{SETUPOPTTAB_X(0),SETUPOPTTAB_Y(2), 61,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63),SETUPOPTTAB_Y(2), 61,SETUPOPTTAB_CELL_H}},
        {{SETUPOPTTAB_X(15),SETUPOPTTAB_Y(3), 30,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63+15),SETUPOPTTAB_Y(3), 30,SETUPOPTTAB_CELL_H}},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;

    memcpy(&zone->zone, &cellPosTable[row][col], sizeof(struct SCREEN_ZONE));
}



static u_int8 setupmenu_cell_data_init(PUICOM_DATA item, u_int8 *strbuf, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);



LDEF_MENU_CONTENT_TAB(gSetupoptmenuTable, setupmenu_cell_zone_init, setupmenu_cell_data_init);
  
static void setupmenu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_tab_clear_row(&gSetupoptmenuTable, 1, SETUPMENU_TAB_ROW_NUM-1, 0);
    }
    gmenu_content_tab_draw(&gSetupoptmenuTable, SETUPMENU_TAB_ROW_NUM, SETUPMENU_TAB_COL_NUM, 0, 0);
}





static u_int8 setupmenu_cell_data_init(PUICOM_DATA item, u_int8 *strbuf, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col)
{
    if (0 == col)
    {
        switch (row)
        {
            case 0:// 模式设置
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_MOSHIXUANXAIN, TEXT_SMALL_BLACK);
                break;
            case 1://存储选项
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_CUNCHUXUANXIANG, TEXT_SMALL_BLACK);
                break;
            case 2://还原出厂设置 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_RESETFACTORY, TEXT_SMALL_BLACK);
                break;
            case 3://确认 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_QUEREN, TEXT_SMALL_BLACK);
                break;
            default:
                break;
        }
    }

    else if (1 == col)
    {
        switch (row)
        {
            case 0:// 通道切换
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_TONGDAOQIEHUAN, TEXT_SMALL_BLACK);
                break;
            case 1://查询选项
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_CHAXUNOPTION, TEXT_SMALL_BLACK);
                break;
            case 2://其他选项
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_QITAXUANXIANG, TEXT_SMALL_BLACK);
                break;
            case 3://取消 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_QUXIAO, TEXT_SMALL_BLACK);
                break;
            default:
                break;
        }
    }
    
    return ITEM_STATUS_NORMAL; 
}



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
        setupmenu_paint(0);
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            setupmenu_paint(1);
            break;
        default:
            break;
        
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




