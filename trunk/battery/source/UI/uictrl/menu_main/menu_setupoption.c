#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "StoreOption"
//"存储选项"

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuSetupOption,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);



#define  SETUPMENU_TAB_ROW_NUM   4
#define  SETUPMENU_TAB_COL_NUM   2

#define SETUPMENU_TAB_ORIGN_X   (2)
#define SETUPMENU_TAB_ORIGN_Y   (0)
#define SETUPMENU_TAB_W         MAIN_LCD_WIDTH
#define SETUPMENU_TAB_H         MAIN_LCD_HEIGHT


#define SETUPMENU_TAB_COL_H          12
#define SETUPMENU_TAB_COL_W          (SETUPMENU_TAB_W/SETUPMENU_TAB_COL_NUM)
#define SETUPMENU_TAB_COL0_W         60
#define SETUPMENU_TAB_COL1_W         60

#define SETUPMENU_TAB_PAD_H     4
#define SETUPMENU_TAB_PAD_V     4


static u_int8 setupmenu_cell_data_init(PUICOM_DATA item, u_int8 *strbuf, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);
static void setupmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);

static u_int8 gCurStartChNum = 0;

LDEF_MENU_CONTENT_TAB(gSetupoptmenuTable, setupmenu_cell_zone_init, setupmenu_cell_data_init);
  
static void setupmenu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_tab_clear_row(&gSetupoptmenuTable, 1, SETUPMENU_TAB_ROW_NUM-1, 0);
    }
    gmenu_content_tab_draw(&gSetupoptmenuTable, SETUPMENU_TAB_ROW_NUM, SETUPMENU_TAB_COL_NUM, 0, 0);
}


static void setupmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col)
{
    T_SCREEN_PIXEL cellWidth;
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;

    switch (col)
    {
        case 0:
            cellWidth    = SETUPMENU_TAB_COL0_W;
            zone->zone.x = (SETUPMENU_TAB_ORIGN_X);
            break;
        case 1:
            cellWidth = SETUPMENU_TAB_COL1_W;
            zone->zone.x = (SETUPMENU_TAB_ORIGN_X + SETUPMENU_TAB_PAD_H + SETUPMENU_TAB_COL0_W);
            break;
        default:
            cellWidth = SETUPMENU_TAB_COL_W;
            break;
    }

    zone->zone.w = cellWidth;
    zone->zone.h = SETUPMENU_TAB_COL_H;
    zone->zone.y = (SETUPMENU_TAB_ORIGN_Y + row*(SETUPMENU_TAB_COL_H + SETUPMENU_TAB_PAD_V));
    
}

static u_int8 setupmenu_cell_data_init(PUICOM_DATA item, u_int8 *strbuf, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col)
{
    if (0 == col)
    {
        switch (row)
        {
            case 0:// 模式设置
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_MOSHIXUANXAIN, FONT_SIZE_SMALL);
                break;
            case 1://存储选项
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_CUNCHUXUANXIANG, FONT_SIZE_SMALL);
                break;
            case 2://还原出厂设置 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_RESETFACTORY, FONT_SIZE_SMALL);
                break;
            case 3://确认 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_QUEREN, FONT_SIZE_SMALL);
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
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_TONGDAOQIEHUAN, FONT_SIZE_SMALL);
                break;
            case 1://查询选项
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_CHAXUNOPTION, FONT_SIZE_SMALL);
                break;
            case 2://其他选项
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_QITAXUANXIANG, FONT_SIZE_SMALL);
                break;
            case 3://取消 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_QUXIAO, FONT_SIZE_SMALL);
                break;
            default:
                break;
        }
    }
    
    return ITEM_STATUS_NORMAL; 
}


static void  menu_setup_paint(void);

static void  menu_setup_paint(void)
{
    gmenu_win_clear(NULL);
    setupmenu_paint(0);
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
    if (MSG_IS_ENTRY(e->sig))
    {
        menu_setup_paint();
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_1:
            setupmenu_paint(1);
            break;
        case EVENT_KEY_NUM_2:
            ui_mmi_enter(UI_NODE_STOPMENU, 0);
            break;
        default:
            break;
        
    }

    
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




