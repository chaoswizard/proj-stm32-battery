#include "uimmi_ctrl.h"

#define THIS_MENU_NAME   "¿ª»ú"
// "PowerOn"

static void menu_poweron_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_poweron_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_poweron_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenu_PowerOn,
                                  menu_poweron_enter,
                                  menu_poweron_handle,
                                  menu_poweron_exit);




static void menu_poweron_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    ui_mmi_debug_suspend(THIS_MENU_NAME, me, child);
}

static void menu_poweron_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    ui_mmi_debug_resume(THIS_MENU_NAME, child, me);
}


static void menu_poweron_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    ui_mmi_debug_enter(THIS_MENU_NAME, parent, me);
    ui_mmi_reg_suspend(menu_poweron_suspend);
    ui_mmi_reg_resume(menu_poweron_resume);
}

static void menu_poweron_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    ui_mmi_debug_handle(THIS_MENU_NAME, me, e);

}

static void menu_poweron_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




