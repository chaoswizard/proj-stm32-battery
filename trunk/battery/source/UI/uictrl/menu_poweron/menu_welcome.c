#include "uimmi_ctrl.h"
#include "gui_menu_item.h"


#define THIS_MENU_NAME   "欢迎"
//"Welcome"
static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuWelcome,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);


//==========================================================
#define WECLCOME_GONGSI_1_X  1
#define WECLCOME_GONGSI_1_Y  1
#define WECLCOME_GONGSI_1_W  120
#define WECLCOME_GONGSI_1_H  13

#define WECLCOME_GONGSI_2_X  8
#define WECLCOME_GONGSI_2_Y  (WECLCOME_GONGSI_1_Y + WECLCOME_GONGSI_1_H)
#define WECLCOME_GONGSI_2_W  WECLCOME_GONGSI_1_W
#define WECLCOME_GONGSI_2_H  WECLCOME_GONGSI_1_H
#define WECLCOME_GONGSI_3_X  20
#define WECLCOME_GONGSI_3_Y  (WECLCOME_GONGSI_2_Y + WECLCOME_GONGSI_1_H - 1)
#define WECLCOME_GONGSI_3_W  WECLCOME_GONGSI_1_W
#define WECLCOME_GONGSI_3_H  WECLCOME_GONGSI_1_H

#define WECLCOME_GONGSI_4_X  16
#define WECLCOME_GONGSI_4_Y  (WECLCOME_GONGSI_3_Y + WECLCOME_GONGSI_1_H)
#define WECLCOME_GONGSI_4_W  WECLCOME_GONGSI_1_W
#define WECLCOME_GONGSI_4_H  WECLCOME_GONGSI_1_H

#define WECLCOME_GONGSI_5_X  4
#define WECLCOME_GONGSI_5_Y  (WECLCOME_GONGSI_4_Y + WECLCOME_GONGSI_1_H)
#define WECLCOME_GONGSI_5_W  WECLCOME_GONGSI_1_W
#define WECLCOME_GONGSI_5_H  WECLCOME_GONGSI_1_H

static void  menu_welcome_paint(void);

void menu_show_help(void)
{
    struct SCREEN_ZONE rect;
    
    SCREEN_ZONE_INIT(&rect, 12, WECLCOME_GONGSI_2_Y, WECLCOME_GONGSI_2_W, WECLCOME_GONGSI_2_H);
    Screen_PrintString(&rect, "SYSTEM  STANDBY!", TEXT_STYLE_DEFAULT);

    SCREEN_ZONE_INIT(&rect, 2, WECLCOME_GONGSI_3_Y, WECLCOME_GONGSI_3_W, WECLCOME_GONGSI_3_H);
    Screen_PrintString(&rect, "Press[ESC]Power OFF.", TEXT_STYLE_DEFAULT );
    
    SCREEN_ZONE_INIT(&rect, 2, WECLCOME_GONGSI_4_Y, WECLCOME_GONGSI_4_W, WECLCOME_GONGSI_4_H);
    Screen_PrintString(&rect, "Press [OK] MainMenu.", TEXT_STYLE_DEFAULT );
}


static void  menu_welcome_paint(void)
{
    struct SCREEN_ZONE rect;
    
    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_1_X, WECLCOME_GONGSI_1_Y, WECLCOME_GONGSI_1_W, WECLCOME_GONGSI_1_H);
    Screen_PrintString(&rect, "佛山市惠龙自动化有限公司", TEXT_STYLE_DEFAULT);
    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_2_X, WECLCOME_GONGSI_2_Y, WECLCOME_GONGSI_2_W, WECLCOME_GONGSI_2_H);
    Screen_PrintString(&rect, "全自动高速预放电系统", TEXT_STYLE_DEFAULT);

    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_3_X, WECLCOME_GONGSI_3_Y, WECLCOME_GONGSI_3_W, WECLCOME_GONGSI_3_H);
    Screen_PrintString(&rect, "电话:13825553998", TEXT_STYLE_DEFAULT );

    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_4_X, WECLCOME_GONGSI_4_Y, WECLCOME_GONGSI_4_W, WECLCOME_GONGSI_4_H);
    Screen_PrintString(&rect, "佛山市顺德区陈村镇", TEXT_STYLE_DEFAULT);

    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_5_X, WECLCOME_GONGSI_5_Y, WECLCOME_GONGSI_5_W, WECLCOME_GONGSI_5_H);
    Screen_PrintString(&rect, "顺联国际机械城19栋A05", TEXT_STYLE_DEFAULT);
}


//==========================================================

static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_SUSPEND(THIS_MENU_NAME, me, child);
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
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);

    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        menu_welcome_paint();
    }
    return UI_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    UIMMI_DEBUGSM_EXIT(THIS_MENU_NAME, next, me);
}




