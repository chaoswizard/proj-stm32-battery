#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "Stop"
//"ֹͣ"

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuStopCheck,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);

//===========================================================
#define STOP_TEXT_1_X  0
#define STOP_TEXT_1_Y  0
#define STOP_TEXT_1_W  120
#define STOP_TEXT_1_H  32

#define STOP_TEXT_2_X  0
#define STOP_TEXT_2_Y  (STOP_TEXT_1_Y + STOP_TEXT_1_H)
#define STOP_TEXT_2_W  STOP_TEXT_1_W
#define STOP_TEXT_2_H  STOP_TEXT_1_H
#define STOP_TEXT_3_X  0
#define STOP_TEXT_3_Y  (STOP_TEXT_2_Y + STOP_TEXT_1_H)
#define STOP_TEXT_3_W  STOP_TEXT_1_W
#define STOP_TEXT_3_H  STOP_TEXT_1_H


static void  menu_stop_paint(void);

static void  menu_stop_paint(void)
{
    gmenu_win_clear(NULL);
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
        menu_stop_paint();
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




