#include "stm32f10x.h"
#include "global.h"
#include "uimmi_ctrl.h"
#include "uikeypad_ctrl.h"

#define UI_KEYPAD_TRACE(f, v, s)   MY_DEBUG(f, v, s)


typedef u_int32 UI_KEY_NODE_HANDLE;

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
    {0x10000,  EVENT_KEY_ADD},
    {0x4,      EVENT_KEY_SUB},
    {0x20000,  EVENT_KEY_UP},
    {0x80,     EVENT_KEY_DOWN},
    {0x100,    EVENT_KEY_LEFT},
    {0x400,    EVENT_KEY_RIGHT},
    {0x2,      EVENT_KEY_OK},
    {0x1,      EVENT_KEY_ESC},
    {0x400000, EVENT_KEY_RUN},
    {0x40000,  EVENT_KEY_NUM_0},
    {0x80000,  EVENT_KEY_NUM_1},
    {0x100000, EVENT_KEY_NUM_2},
    {0x200000, EVENT_KEY_NUM_3},
    {0x800,    EVENT_KEY_NUM_4},
    {0x1000,   EVENT_KEY_NUM_5},
    {0x2000,   EVENT_KEY_NUM_6},
    {0x8,      EVENT_KEY_NUM_7},
    {0x10,     EVENT_KEY_NUM_8},
    {0x20,     EVENT_KEY_NUM_9},
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


void ui_keypad_scan(void)
{
    struct EVENT_NODE_ITEM e;

    //check whether any key press
    if(Flag_keyPressed)
    {
        if(curKey!=0) 
        {
            //printf_debug("K", curKey, NULL);
            if (UI_KEY_TYPE_NULL != ui_keypad_map(curKey, &e))
            {
                ui_mmi_send_msg(&e);
            }
            curKey=0;  
        }
        Flag_keyPressed=0; 
        preKey=0;
    }    
}

