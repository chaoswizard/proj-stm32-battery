#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME  "主工作界面"
//"MainFace"
#define THIS_MENU_SM_HANDLE      (gMenuMainSm)
#define THIS_MENU_UI_CONTAINER   (gMainMenuTable)

#define  MAINMENU_TAB_ROW_NUM   5
#define  MAINMENU_TAB_COL_NUM   3

#define MAINMENU_TAB_ORIGN_X   (1)
#define MAINMENU_TAB_ORIGN_Y   (1)
#define MAINMENU_TAB_W         126


#define MAINMENU_TAB_ROW_H          12
#define MAINMENU_TAB_ROW0_H         13
#define MAINMENU_TAB_COL0_W         26
#define MAINMENU_TAB_COL1_W         48
#define MAINMENU_TAB_COL2_W         50
#define MAINMENU_TAB_COL3_W         32


struct MAIN_MENU_CTRL {
    bool_t ui_init;
    u_int8 tstVal;
};

static  struct MAIN_MENU_CTRL gMainInterfaceCtrl;

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(THIS_MENU_SM_HANDLE,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);

static T_UICOM_DRAW_MODE mainmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_EVENT *p_type);
static void mainmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_COUNT row, T_UICOM_COUNT col);


LDEF_MENU_CONTENT_TAB(THIS_MENU_UI_CONTAINER, mainmenu_cell_zone_init, mainmenu_cell_data_init);
  
static void menu_main_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_tab_clear_row(&THIS_MENU_UI_CONTAINER, 1, MAINMENU_TAB_ROW_NUM-1, 0);
    }
    gmenu_content_tab_draw(&THIS_MENU_UI_CONTAINER, MAINMENU_TAB_ROW_NUM, MAINMENU_TAB_COL_NUM, 0, 0);
}


static void mainmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_COUNT row, T_UICOM_COUNT col)
{
    T_SCREEN_PIXEL cellWidth;
  
    zone->border.l = 1;
    zone->border.t = 1;
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
            #if 0
        case 3://通道切换
            cellWidth = MAINMENU_TAB_COL3_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X + MAINMENU_TAB_COL0_W + MAINMENU_TAB_COL1_W + MAINMENU_TAB_COL2_W);
            break;
            #endif
        default:
            cellWidth = 1;
            break;
    }

    zone->zone.w = cellWidth;
    if (0 == row)
    {
        zone->zone.h = MAINMENU_TAB_ROW0_H;
        zone->zone.y = (MAINMENU_TAB_ORIGN_Y);
    }
    else
    {
        zone->zone.h = MAINMENU_TAB_ROW_H;
        zone->zone.y = (MAINMENU_TAB_ORIGN_Y + MAINMENU_TAB_ROW0_H + (row - 1)*MAINMENU_TAB_ROW_H);
    }
    
}

static T_UICOM_DRAW_MODE mainmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_EVENT *p_type)
{
    if (gMainInterfaceCtrl.ui_init && (0 == row))
    {
        return DRAW_MODE_SKIP;
    }
    zone->x = 2;
    zone->y = 1;
    
    UICOM_CLR_SLECTED((*p_type));
    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT)
    if (0 == row)
    {
   
        switch (col)
        {
            case 0:// 序号
                UICOM_DATA_FILL(item, UICOM_STR_XUHAO);
                break;
            case 1://当前值
                UICOM_DATA_FILL(item, UICOM_STR_DANGQIANZHI);
                break;
            case 2://工作状态 
                UICOM_DATA_FILL(item, UICOM_STR_GONGZUOZHUANGTAI);
                break;
            case 3://通道切换
                UICOM_DATA_FILL(item, UICOM_STR_TONGDAOQIEHUAN);
                break;
            default:
                break;
        }
        return DRAW_MODE_TEXT_BOX; 
    }

    //===========非首行============
    if (0 == col)
    {
        sprintf(UICOM_DATA_BUF(item), "%04d", gMainInterfaceCtrl.tstVal+row);
    }
    else if (1 == col)
    {
        sprintf(UICOM_DATA_BUF(item), "%d.%03d", col, row+gMainInterfaceCtrl.tstVal);
    }
    else if (2 == col)
    {
        if (((((row)% ((gMainInterfaceCtrl.tstVal%MAINMENU_TAB_ROW_NUM) + 1)))&&(gMainInterfaceCtrl.tstVal%MAINMENU_TAB_ROW_NUM)))
        {
            UICOM_DATA_FILL(item, UICOM_STR_FANGDIANZHONG);
        }
        else
        {
            UICOM_SET_SLECTED((*p_type));
            UICOM_DATA_FILL(item, UICOM_STR_FANGDIANJIESHU);
        }
    }
    #if 0
    else if (3 == col)
    {
        if (((row)% 2) && (gMainInterfaceCtrl.tstVal%7))
        {
            UICOM_DATA_FILL(item, UICOM_STR_QIYONG);
        }
        else
        {
            UICOM_DATA_FILL(item, UICOM_STR_TINGYONG);
        }
    }
    #endif
    return DRAW_MODE_TEXT_BOX; 
}


static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_SUSPEND(THIS_MENU_NAME, me, child);
    gMainInterfaceCtrl.ui_init = 0;
}

static void menu_pub_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_RESUME(THIS_MENU_NAME, child, me);
}


static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    UIMMI_DEBUGSM_ENTER(THIS_MENU_NAME, parent, me);
    ui_mmi_reg_suspend(menu_pub_suspend);
    ui_mmi_reg_resume(menu_pub_resume);
    gMainInterfaceCtrl.ui_init = 0;
    
    gMainInterfaceCtrl.tstVal = 0;
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);
    
    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        menu_main_paint(0);
        gMainInterfaceCtrl.ui_init = 1;
    }
    else
    switch (e->sig)
    {
        case EVENT_KEY_SUB:
            gMainInterfaceCtrl.tstVal += (MAINMENU_TAB_ROW_NUM-1);
            menu_main_paint(1);
            break;
        default:
            break;
        
    }
    
    return UI_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    UIMMI_DEBUGSM_EXIT(THIS_MENU_NAME, next, me);
}




