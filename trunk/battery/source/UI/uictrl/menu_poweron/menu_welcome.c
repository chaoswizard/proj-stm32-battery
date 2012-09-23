#include "uimmi_ctrl.h"
#include "gui_menu_item.h"


#define THIS_MENU_NAME   "»¶Ó­"
//"Welcome"
static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuWelcome,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);


//==========================================================
#define WECLCOME_GONGSI_1_X  2
#define WECLCOME_GONGSI_1_Y  7
#define WECLCOME_GONGSI_1_W  120
#define WECLCOME_GONGSI_1_H  16

#define WECLCOME_GONGSI_2_X  21
#define WECLCOME_GONGSI_2_Y  (WECLCOME_GONGSI_1_Y + WECLCOME_GONGSI_1_H)
#define WECLCOME_GONGSI_2_W  WECLCOME_GONGSI_1_W
#define WECLCOME_GONGSI_2_H  WECLCOME_GONGSI_1_H
#define WECLCOME_GONGSI_3_X  0
#define WECLCOME_GONGSI_3_Y  (WECLCOME_GONGSI_2_Y + WECLCOME_GONGSI_1_H)
#define WECLCOME_GONGSI_3_W  WECLCOME_GONGSI_1_W
#define WECLCOME_GONGSI_3_H  WECLCOME_GONGSI_1_H


static void  menu_welcome_paint(void);

static void  menu_welcome_paint(void)
{
    struct SCREEN_ZONE rect;
    
    
    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_1_X, WECLCOME_GONGSI_1_Y, WECLCOME_GONGSI_1_W, WECLCOME_GONGSI_1_H);
    Screen_PrintString(&rect, UICOM_STR_GONGSIMING0, TEXT_BIG_BLACK);

    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_2_X, WECLCOME_GONGSI_2_Y, WECLCOME_GONGSI_2_W, WECLCOME_GONGSI_2_H);
    Screen_PrintString(&rect, UICOM_STR_CHANPINMING, TEXT_BIG_BLACK);

    SCREEN_ZONE_INIT(&rect, WECLCOME_GONGSI_3_X, WECLCOME_GONGSI_3_Y, WECLCOME_GONGSI_3_W, WECLCOME_GONGSI_3_H);
    Screen_PrintString(&rect, UICOM_STR_GONGSIDIANHUA, TEXT_BIG_BLACK);
}


//==========================================================

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
        menu_welcome_paint();
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




