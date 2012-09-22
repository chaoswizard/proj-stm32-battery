#include "uimmi_ctrl.h"
#include "uikeypad_ctrl.h"

void ui_keypad_init(void)
{

}

void ui_keypad_deinit(void)
{

}

void ui_keypad_scan(void)
{
    struct EVENT_NODE_ITEM e;
    
    if (keyboard_scan(&e))
    {
        ui_mmi_send_msg(&e);
    }
}


