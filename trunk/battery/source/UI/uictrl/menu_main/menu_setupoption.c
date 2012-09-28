#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "设置选项"

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
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
#define SETUPOPTTAB_YY(y)   (SETUPOPTTAB_Y(y)+2)
static void setupmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_COUNT row, T_UICOM_COUNT col)
{
    struct SCREEN_ZONE cellPosTable[SETUPMENU_TAB_ROW_NUM][SETUPMENU_TAB_COL_NUM] = {
        {{SETUPOPTTAB_X(0),SETUPOPTTAB_Y(0),  61,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63),SETUPOPTTAB_Y(0), 61,SETUPOPTTAB_CELL_H}},
        {{SETUPOPTTAB_X(0),SETUPOPTTAB_Y(1), 61,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63),SETUPOPTTAB_Y(1), 61,SETUPOPTTAB_CELL_H}},
        {{SETUPOPTTAB_X(0),SETUPOPTTAB_Y(2), 61,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63),SETUPOPTTAB_Y(2), 61,SETUPOPTTAB_CELL_H}},
        {{SETUPOPTTAB_X(15),SETUPOPTTAB_YY(3), 30,SETUPOPTTAB_CELL_H},{SETUPOPTTAB_X(63+15),SETUPOPTTAB_YY(3), 30,SETUPOPTTAB_CELL_H}},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;

    memcpy(&zone->zone, &cellPosTable[row][col], sizeof(struct SCREEN_ZONE));
}



static u_int8 setupmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, enum T_UICOM_STATUS type);



LDEF_MENU_CONTENT_TAB(gSetupoptmenuTable, setupmenu_cell_zone_init, setupmenu_cell_data_init);
  
static void setupmenu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_tab_clear_row(&gSetupoptmenuTable, 1, SETUPMENU_TAB_ROW_NUM-1, 0);
    }
    gmenu_content_tab_draw(&gSetupoptmenuTable, SETUPMENU_TAB_ROW_NUM, SETUPMENU_TAB_COL_NUM, 0, 0);
}





static u_int8 setupmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, enum T_UICOM_STATUS type)
{
    zone->x = 2;
    zone->y = 3;

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_SMALL_BLACK);
    if (0 == col)
    {
        switch (row)
        {
            case 0:// 模式设置
                UICOM_DATA_FILL(item, UICOM_STR_MOSHIXUANXAIN);
                break;
            case 1://存储选项
                UICOM_DATA_FILL(item, UICOM_STR_CUNCHUXUANXIANG);
                break;
            case 2://还原出厂设置 
                UICOM_DATA_FILL(item, UICOM_STR_RESETFACTORY);
                break;
            case 3://确认 
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                zone->x += 6;
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
                UICOM_DATA_FILL(item, UICOM_STR_TONGDAOQIEHUAN);
                break;
            case 1://查询选项
                UICOM_DATA_FILL(item, UICOM_STR_CHAXUNOPTION);
                break;
            case 2://其他选项
                UICOM_DATA_FILL(item, UICOM_STR_QITAXUANXIANG);
                break;
            case 3://取消 
                UICOM_DATA_FILL(item, UICOM_STR_QUXIAO);
                zone->x += 4;
                break;
            default:
                break;
        }
    }
    
    return PAINT_FLAG_NORMAL; 
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

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    ui_mmi_debug_handle(THIS_MENU_NAME, me, e);
    
    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        setupmenu_paint(0);
    }
    else 
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            setupmenu_paint(1);
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




