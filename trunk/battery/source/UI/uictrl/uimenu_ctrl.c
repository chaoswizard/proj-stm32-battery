#include "uimenu_ctrl.h"
#include "brush_print.h"


void ui_menu_init(void)
{
    Screen_PrintInit();
    ui_menu_reset();
}

void ui_menu_deinit(void)
{
}

void ui_menu_reset(void)
{
    extern  menu_show_help();
    
    Screen_PrintClear(NULL);
    
    menu_show_help();
}

u_int8 ui_menu_bypass_proc(struct EVENT_NODE_ITEM *e)
{
    switch (e->sig)
    {
        case EVENT_KEY_ESC:
        ui_mmi_enter(UI_NODE_MAINMENU, 1);
        return 1;
        case EVENT_KEY_ADD:
        ui_mmi_enter(UI_NODE_SETUPOPT, 1);
        return 1;
        case EVENT_KEY_SUB:
        ui_mmi_enter(UI_NODE_SEARCHINFO, 1);
        return 1;
        default:
            break;
        
    }
    
    return 0;
}

u_int8 ui_menu_shotcut_proc(struct EVENT_NODE_ITEM *e)
{
    if (ui_menu_bypass_proc(e))
    {
        return 1;
    }
    // common key map, mutiifunction
    if (MSG_IS_START(e->sig))
    {
        ui_mmi_enter(UI_NODE_MAINMENU, 1);
        return 1;
    }
    else if (MSG_IS_CANCLE(e->sig))
    {
        ui_mmi_poweroff();
    }
    else if (MSG_IS_ENTER(e->sig))
    {
        ui_mmi_enter(UI_NODE_MAINMENU, 1);
    }
    
    return 0;
}


