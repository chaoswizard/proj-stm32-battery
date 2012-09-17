#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME  "MainFace"
//"主工作界面"

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static void menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuMain,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);

static void mainmenu_tab_init(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, struct OSD_ZONE *zone, PUICOM_DATA item, u_int8 *strbuf);


LDEF_MENU_CONTENT_TAB(gMainMenuTable, 0, 0, 64, 32, mainmenu_tab_init, 5, 5);

static void menu_main_paint(void)
{
    gmenu_content_tab_draw(&gMainMenuTable, 5, 5, 0, 0);
}

static void mainmenu_tab_init(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, struct OSD_ZONE *zone, PUICOM_DATA item, u_int8 *strbuf)
{
    if (0 == col)
    {
        UICOM_DATA_TEXT_INIT(item, NULL);
        sprintf(strbuf, "<%d, %d>", row, col);
    }
    else
    {
        UICOM_DATA_TEXT_INIT(item, UICOM_STR_BAOCUNCHENGGONG);
    }
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
        menu_main_paint();
    }
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
    
}




