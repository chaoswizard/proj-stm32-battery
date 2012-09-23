#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "ֹͣ"
//"Stop"

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

#define STOPTAB_CELL_H  12
#define STOPTAB_X(x)   (x+1)
#define STOPTAB_Y(y)   ((y*(STOPTAB_CELL_H+2))+4)
#define STOPTAB_YY(y)   (STOPTAB_Y(y)+2)
static void stopmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col)
{
    struct SCREEN_ZONE cellPosTable[STOPMENU_TAB_ROW_NUM][STOPMENU_TAB_COL_NUM] = {
        {{STOPTAB_X(0),STOPTAB_Y(0),  61,STOPTAB_CELL_H},{STOPTAB_X(63),STOPTAB_Y(0), 61,STOPTAB_CELL_H}},
        {{STOPTAB_X(0),STOPTAB_Y(1), 61,STOPTAB_CELL_H},{STOPTAB_X(63),STOPTAB_Y(1), 61,STOPTAB_CELL_H}},
        {{STOPTAB_X(0),STOPTAB_Y(2), 61,STOPTAB_CELL_H},{STOPTAB_X(63),STOPTAB_Y(2), 61,STOPTAB_CELL_H}},
        {{STOPTAB_X(15),STOPTAB_YY(3), 30,STOPTAB_CELL_H},{STOPTAB_X(63+15),STOPTAB_YY(3), 30,STOPTAB_CELL_H}},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;

    memcpy(&zone->zone, &cellPosTable[row][col], sizeof(struct SCREEN_ZONE));
}


static u_int8 stopmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, enum OSD_OBJ_DRAW_TYPE type);
LDEF_MENU_CONTENT_TAB(gStopMenuTable, stopmenu_cell_zone_init, stopmenu_cell_data_init);
  
static void stop_main_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_tab_clear_row(&gStopMenuTable, 1, STOPMENU_TAB_ROW_NUM-1, 0);
    }
    gmenu_content_tab_draw(&gStopMenuTable, STOPMENU_TAB_ROW_NUM, STOPMENU_TAB_COL_NUM, 0, 0);
}


static u_int8 stopmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, enum OSD_OBJ_DRAW_TYPE type)
{
    zone->x = 2;
    zone->y = 3;
    UICOM_DATA_TEXT_ATTR(item, TEXT_SMALL_BLACK);
    if (0 == col)
    {
        switch (row)
        {
            case 0:// �Ѽ���Ʒ����
                sprintf(UICOM_DATA_BUF(item), "%s %d", UICOM_STR_YIJIANCESHULIANF, 80);
                break;
            case 1://�ϸ���
                sprintf(UICOM_DATA_BUF(item), "%s %d%%", UICOM_STR_HEGELV, 100);
                break;
            case 2://ѡ������ 
                UICOM_DATA_FILL(item, UICOM_STR_XUANXIANGSHEZHI);
                break;
            case 3://ȷ�� 
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
            case 0:// �ϸ���
                sprintf(UICOM_DATA_BUF(item), "%s %d", UICOM_STR_HEGESHU, 80);
                break;
            case 1://�洢���ݷ������ֵ����
                UICOM_DATA_FILL(item, UICOM_STR_JISHUZHIQINGLING);
                break;
            case 2://����������
                UICOM_DATA_FILL(item, UICOM_STR_CHUANKOUSHUCHU);
                break;
            case 3://ȡ�� 
                UICOM_DATA_FILL(item, UICOM_STR_QUXIAO);
                zone->x += 4;
                break;
            default:
                break;
        }
    }
    
    return PAINT_STATUS_NORMAL; 
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
        stop_main_paint(0);
    }
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            stop_main_paint(1);
            break;
        default:
            break;
        
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




