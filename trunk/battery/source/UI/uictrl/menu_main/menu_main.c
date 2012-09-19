#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME  "MainFace"
//"主工作界面"

#define  MAINMENU_TAB_ROW_NUM   6
#define  MAINMENU_TAB_COL_NUM   4

#define MAINMENU_TAB_ORIGN_X   (0)
#define MAINMENU_TAB_ORIGN_Y   (0)
#define MAINMENU_TAB_W         MAIN_LCD_WIDTH
#define MAINMENU_TAB_H         MAIN_LCD_HEIGHT


#define MAINMENU_TAB_COL_H          12
#define MAINMENU_TAB_COL_W          (MAINMENU_TAB_W/MAINMENU_TAB_COL_NUM)
#define MAINMENU_TAB_COL0_W         28
#define MAINMENU_TAB_COL1_W         32
#define MAINMENU_TAB_COL2_W         28
#define MAINMENU_TAB_COL3_W         32

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuMain,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);

static u_int8 mainmenu_cell_data_init(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, PUICOM_DATA item, u_int8 *strbuf);
static void mainmenu_cell_zone_init(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col,struct OSD_ZONE *zone);

static u_int8 gCurStartChNum = 0;

LDEF_MENU_CONTENT_TAB(gMainMenuTable, mainmenu_cell_zone_init, mainmenu_cell_data_init);
  
static void menu_main_paint(u_int8 isFirst)
{
    if (!isFirst)
    {
        gmenu_content_tab_clear_row(&gMainMenuTable, 1, MAINMENU_TAB_COL_NUM-1);
    }
    gmenu_content_tab_draw(&gMainMenuTable, MAINMENU_TAB_ROW_NUM, MAINMENU_TAB_COL_NUM, 0, 0);
}


static void mainmenu_cell_zone_init(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col,struct OSD_ZONE *zone)
{
    T_SCREEN_PIXEL cellWidth;

    if (0 == col)
    {
        zone->border.l = 1;
    }

    if (0 == row)
    {
        zone->border.t = 1;
    }
    
    zone->border.b = 1;
    zone->border.r = 1;
    switch (col)
    {
        case 0:// 序号
            cellWidth    = MAINMENU_TAB_COL0_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X);
            break;
        case 1://当前值
            cellWidth = MAINMENU_TAB_COL1_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X + MAINMENU_TAB_COL0_W);
            break;
        case 2://工作状态 
            cellWidth = MAINMENU_TAB_COL2_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X + MAINMENU_TAB_COL0_W + MAINMENU_TAB_COL1_W);
            break;
        case 3://通道切换
            cellWidth = MAINMENU_TAB_COL3_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X + MAINMENU_TAB_COL0_W + MAINMENU_TAB_COL1_W + MAINMENU_TAB_COL2_W);
            break;
        default:
            cellWidth = MAINMENU_TAB_COL_W;
            break;
    }

    zone->zone.w = cellWidth;
    zone->zone.h = MAINMENU_TAB_COL_H;
    zone->zone.y = (MAINMENU_TAB_ORIGN_Y + row*MAINMENU_TAB_COL_H);
    
}

static u_int8 mainmenu_cell_data_init(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, PUICOM_DATA item, u_int8 *strbuf)
{
    if (0 == row)
    {
        if (0 != gCurStartChNum)//是否非第一次绘制
        {
            //return PAINT_STATUS_SKIP_ALL;
        }
        
        switch (col)
        {
            case 0:// 序号
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_XUHAO, FONT_SIZE_SMALL);
                break;
            case 1://当前值
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_DANGQIANZHI, FONT_SIZE_SMALL);
                break;
            case 2://工作状态 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_GONGZUOZHUANGTAI, FONT_SIZE_SMALL);
                break;
            case 3://通道切换
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_TONGDAOQIEHUAN, FONT_SIZE_SMALL);
                break;
            default:
                break;
        }
        return ITEM_STATUS_NORMAL; 
    }

    //===========非首行============
    if (0 == col)
    {
        UICOM_DATA_TEXT_INIT(item, NULL, FONT_SIZE_SMALL);
        sprintf(strbuf, "04%d", gCurStartChNum+row);
    }
    else if (1 == col)
    {
        UICOM_DATA_TEXT_INIT(item, NULL, FONT_SIZE_SMALL);
        sprintf(strbuf, "%d.%d", row, col);
    }
    else if (2 == col)
    {
        if (row % 2)
        {
            UICOM_DATA_TEXT_INIT(item, UICOM_STR_FANGDIANZHONG, FONT_SIZE_SMALL);
        }
        else
        {
            UICOM_DATA_TEXT_INIT(item, UICOM_STR_FANGDIANJIESHU, FONT_SIZE_SMALL);
        }
    }
    else if (3 == col)
    {
        if (row % 2)
        {
            UICOM_DATA_TEXT_INIT(item, UICOM_STR_QIYONG, FONT_SIZE_SMALL);
        }
        else
        {
            UICOM_DATA_TEXT_INIT(item, UICOM_STR_TINGYONG, FONT_SIZE_SMALL);
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
    gCurStartChNum = 0;
}

static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    //ui_mmi_debug_handle(THIS_MENU_NAME, me, e);
    if (MSG_IS_ENTRY(e->sig))
    {
        menu_main_paint(1);
    }
    
    switch (e->sig)
    {
        case EVENT_KEY_NUM_1:
            gCurStartChNum += 4;
            menu_main_paint(0);
            break;
        default:
            break;
        
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




