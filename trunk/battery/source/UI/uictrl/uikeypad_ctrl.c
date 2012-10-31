#include "stm32f10x.h"
#include "uimmi_ctrl.h"
#include "uikeypad_ctrl.h"
#include "global.h"
#include "input_output.h"

#define KEY_165_SCANF_TMR_DELAY   10

static SWTMR_NODE_HANDLE key_scanf_tmr;
struct KEYBOARD_KEY_VAL_MAP {
    unsigned int hdl;
    EVENT_NODE_HANDLE  sig;
};

static const unsigned int gUiDigitKeyMapTbl[] = {
    0x40000,  
    0x80000,  
    0x100000, 
    0x200000, 
    0x800,    
    0x1000,   
    0x2000,   
    0x8,      
    0x10,     
    0x20,     
};

static const struct KEYBOARD_KEY_VAL_MAP gUiKeyNodeMapTabl[] = {
    {0x10000,  EVENT_KEY_ADD},
    {0x4,      EVENT_KEY_SUB},
    {0x20000,  EVENT_KEY_UP},
    {0x80,     EVENT_KEY_DOWN},
    {0x100,    EVENT_KEY_LEFT},
    {0x400,    EVENT_KEY_RIGHT},
    {0x2,      EVENT_KEY_OK},
    {0x1,      EVENT_KEY_ESC},
    {0x400000, EVENT_KEY_RUN},
};

static u_int8 console_keyboard_map(unsigned int keyHdl, struct EVENT_NODE_ITEM *e)
{
    s_int32 i;

    for (i=0; i< ARRAY_SIZE(gUiKeyNodeMapTabl); i++)
    {
        if (keyHdl == gUiKeyNodeMapTabl[i].hdl)
        {
            e->sig = gUiKeyNodeMapTabl[i].sig;
            return 1;
        }
    }
    
    for (i=0; i< ARRAY_SIZE(gUiDigitKeyMapTbl); i++)
    {
        if (keyHdl == gUiDigitKeyMapTbl[i])
        {
            e->sig = EVENT_KEY_CHAR;
            KEY_EVENT_DIGIT_INIT(e->param, ('0' + i));
            return 1;
        }
    }
    
    return 0;
}

void Key165Scan(void *p)
{
    u_int32 tmp;
    //-------------------Check Key Press-----------------
    if(keyDelay==0)
    {
    	tmp = input_165();
    	if(tmp != 0)
    	{
    		if(preKey == tmp)	
    		{
    			preKey = 0;	
    			curKey = tmp;	
    			Flag_keyPressed = 1;                    //ÓÐÐ§°´¼ü
    			keyDelay = 20;                               // 200 ms
    			GPIOA->BSRR |= 0x0400;	/*PA10=SPK,·äÃù*/							
    		}
    		else 
    		{
    			preKey=tmp;
    		}
    	}
    }
    else 
    {
    	if(keyDelay-- == 14)	
    	{
    		GPIOA->BRR |= 0x0400;	/*PA10=SPK£¬Í£Ãù*/
    	}	
    }
}

u_int8 keyboard_scan(struct EVENT_NODE_ITEM *e)
{
    u_int8 ret = 0;

    //check whether any key press
    if(Flag_keyPressed)
    {
        if(curKey !=0) 
        {
            ret = console_keyboard_map(curKey, e);
            curKey=0;  
        }
        Flag_keyPressed=0; 
        preKey=0;
    }    

    return ret;
}





void ui_keypad_init(void)
{
  //  key_scanf_tmr = ui_mmi_start_timer(KEY_165_SCANF_TMR_DELAY,   \
     //                  Key165Scan,  NULL, TRUE);
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


