#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "Stop"
//"停止"

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuStopCheck,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);

//===========================================================

#define  STOPMENU_TAB_ROW_NUM   4
#define  STOPMENU_TAB_COL_NUM   2

#define STOPMENU_TAB_ORIGN_X   (2)
#define STOPMENU_TAB_ORIGN_Y   (0)
#define STOPMENU_TAB_W         MAIN_LCD_WIDTH
#define STOPMENU_TAB_H         MAIN_LCD_HEIGHT


#define STOPMENU_TAB_COL_H          12
#define STOPMENU_TAB_COL_W          (STOPMENU_TAB_W/STOPMENU_TAB_COL_NUM)
#define STOPMENU_TAB_COL0_W         60
#define STOPMENU_TAB_COL1_W         60

#define STOPMENU_TAB_PAD_H     4
#define STOPMENU_TAB_PAD_V     4


static u_int8 stopmenu_cell_data_init(PUICOM_DATA item, u_int8 *strbuf, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);
static void stopmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col);

static u_int8 gCurStartChNum = 0;

LDEF_MENU_CONTENT_TAB(gStopMenuTable, stopmenu_cell_zone_init, stopmenu_cell_data_init);
  
static void stop_main_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_tab_clear_row(&gStopMenuTable, 1, STOPMENU_TAB_ROW_NUM-1, 0);
    }
    gmenu_content_tab_draw(&gStopMenuTable, STOPMENU_TAB_ROW_NUM, STOPMENU_TAB_COL_NUM, 0, 0);
}


static void stopmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col)
{
    T_SCREEN_PIXEL cellWidth;
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;

    switch (col)
    {
        case 0:
            cellWidth    = STOPMENU_TAB_COL0_W;
            zone->zone.x = (STOPMENU_TAB_ORIGN_X);
            break;
        case 1:
            cellWidth = STOPMENU_TAB_COL1_W;
            zone->zone.x = (STOPMENU_TAB_ORIGN_X + STOPMENU_TAB_PAD_H + STOPMENU_TAB_COL0_W);
            break;
        default:
            cellWidth = STOPMENU_TAB_COL_W;
            break;
    }

    zone->zone.w = cellWidth;
    zone->zone.h = STOPMENU_TAB_COL_H;
    zone->zone.y = (STOPMENU_TAB_ORIGN_Y + row*(STOPMENU_TAB_COL_H + STOPMENU_TAB_PAD_V));
    
}

static u_int8 stopmenu_cell_data_init(PUICOM_DATA item, u_int8 *strbuf, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col)
{
    if (0 == col)
    {
        switch (row)
        {
            case 0:// 已检测产品数量
                UICOM_DATA_TEXT_INIT(item, NULL, TEXT_SMALL_BLACK);
                sprintf(strbuf, "%s %x %s", UICOM_STR_YIJIANCESHULIANF, strbuf, UICOM_STR_DANWEIGE);
                break;
            case 1://合格率
                UICOM_DATA_TEXT_INIT(item, NULL, TEXT_SMALL_BLACK);
                sprintf(strbuf, "%s %d.%d", UICOM_STR_HEGELV, row, col);
                break;
            case 2://选项设置 
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_XUANXIANGSHEZHI, TEXT_SMALL_BLACK);
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
            case 0:// 合格数
                UICOM_DATA_TEXT_INIT(item, NULL, TEXT_SMALL_BLACK);
                sprintf(strbuf, "%s %x %s", UICOM_STR_HEGESHU, item, UICOM_STR_DANWEIGE);
                break;
            case 1://存储数据分组计数值清零
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_JISHUZHIQINGLING, TEXT_SMALL_BLACK);
                break;
            case 2://串口输出结果
                UICOM_DATA_TEXT_INIT(item, UICOM_STR_CHUANKOUSHUCHU, TEXT_SMALL_BLACK);
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

static void  menu_stop_paint(void);

static void  menu_stop_paint(void)
{
    gmenu_win_clear(NULL);
    stop_main_paint(0);
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
        menu_stop_paint();
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_1:
            stop_main_paint(1);
            break;
        case EVENT_KEY_NUM_2:
            ui_mmi_enter(UI_NODE_LEAF_00, 1);
            break;
        default:
            break;
        
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




