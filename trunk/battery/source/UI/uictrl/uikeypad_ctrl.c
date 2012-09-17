#include "uimmi_ctrl.h"
#include "uikeypad_ctrl.h"


#define UI_KEYPAD_TRACE(f, v, s)   MY_DEBUG(f, v, s)


typedef char UI_KEY_NODE_HANDLE;

enum UI_KEY_TYPE {
    UI_KEY_TYPE_NULL,
    UI_KEY_TYPE_DIGIT,
    UI_KEY_TYPE_CTRL,
};


struct UI_KEY_NODE_MAP {
    UI_KEY_NODE_HANDLE hdl;
    EVENT_NODE_HANDLE  sig;
};

static const char *gKeyEventName[] = {
    "ADD",
    "SUB",
    "UP",
    "DOWN",
    "LEFT",
    "RIGHT",
    "OK",
    "ESC",
    "RUN",
    "STOP",
    "RESET",
};

static const struct UI_KEY_NODE_MAP gUiKeyNodeMapTabl[] = {
    {'=', EVENT_KEY_ADD},
    {'-', EVENT_KEY_SUB},
    {'k', EVENT_KEY_UP},
    {'j', EVENT_KEY_DOWN},
    {'h', EVENT_KEY_LEFT},
    {'l', EVENT_KEY_RIGHT},
    {'y', EVENT_KEY_OK},
    {'q', EVENT_KEY_ESC},
    {'g', EVENT_KEY_RUN},
    {'p', EVENT_KEY_STOP},
    {'r', EVENT_KEY_RESET},
};

void ui_keypad_init(void)
{

}

void ui_keypad_deinit(void)
{

}

static enum UI_KEY_TYPE ui_keypad_map(UI_KEY_NODE_HANDLE keyHdl, struct EVENT_NODE_ITEM *e)
{
    enum UI_KEY_TYPE type = UI_KEY_TYPE_NULL;
    
    
    if ((keyHdl >= '0') && (keyHdl <= '9'))
    {
        e->param.key.digit.val = keyHdl - '0';
        e->sig = e->param.key.digit.val + EVENT_KEY_NUM_0;
        type = UI_KEY_TYPE_DIGIT;
        UI_KEYPAD_TRACE("[KEY_NUM_%d]:%d\n", e->param.key.digit.val, e->sig);
    }

    // if not digit key
    if (UI_KEY_TYPE_NULL == type)
    {
        s_int32 i;
        for (i=0; i< ARRAY_SIZE(gUiKeyNodeMapTabl); i++)
        {
            if (keyHdl == gUiKeyNodeMapTabl[i].hdl)
            {
                e->sig = gUiKeyNodeMapTabl[i].sig;
                type = UI_KEY_TYPE_CTRL;
                UI_KEYPAD_TRACE("[KEY_Fn_%s]:%d\n", gKeyEventName[i], e->sig);
                break;
            }
        }
    }
    
    if (UI_KEY_TYPE_NULL == type)
    {
        //UI_KEYPAD_TRACE("[KEY_NOMAP:%c]:%x\n", keyHdl, keyHdl);
    }
    
    return type;
}

u_int8 kbhit(void)
{
    return 0;
}

void ui_keypad_scan(void)
{
    struct EVENT_NODE_ITEM e;

    //check whether any key press
    if (!kbhit())
    {
        return;
    }

    if (UI_KEY_TYPE_NULL != ui_keypad_map(getchar(), &e))
    {
        ui_mmi_send_msg(&e);
    }
}

