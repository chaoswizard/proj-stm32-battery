#include "uimenu_ctrl.h"

void ui_menu_init(void)
{
}

void ui_menu_deinit(void)
{
}

u_int8 ui_menu_bypass_proc(struct EVENT_NODE_ITEM *e)
{
    if (MSG_IS_CANCLE(e->sig))
    {
        ui_mmi_return(1);
        return 1;
    }
    
    return 0;
}

u_int8 ui_menu_shotcut_proc(struct EVENT_NODE_ITEM *e)
{
    if (MSG_IS_START(e->sig))
    {
        ui_mmi_enter(UI_NODE_LEAF_01, 1);
        return 1;
    }
    else if (MSG_IS_CANCLE(e->sig))
    {
        //ui_mmi_close();
    }
    
    return 0;
}


