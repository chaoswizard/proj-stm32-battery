#include "uimmi_ctrl.h"
#include "uikeypad_ctrl.h"
#include "uimenu_ctrl.h"

#define UI_MMI_TRACE(f, v)   MY_DEBUG(f, v)


static struct SM_NODE_ITEM    gSmTraceBuf[4];
static struct EVENT_NODE_ITEM gEventQuenue[10];
static struct SWTMR_NODE_ITEM gSwTmrList[4];

struct UI_MMI_CTRL {
    handle_t smHandle;
    handle_t evtHandle;
    handle_t tmrHandle;
};

static struct UI_MMI_CTRL gUiMmiCtrlInstance;
static struct UI_MMI_CTRL *gUiMmiCtrl = NULL;

static u_int8 ui_mmi_bypass_proc(struct EVENT_NODE_ITEM *e);


//=================== menu func map =============================
DECLARE_SM_NODE_MAP(gMenuWelcome);
DECLARE_SM_NODE_MAP(gMenuMainSm);
DECLARE_SM_NODE_MAP(gMenuCheckModeCfg);
DECLARE_SM_NODE_MAP(gMenuSearchOption);
DECLARE_SM_NODE_MAP(gMenuSetupOption);
DECLARE_SM_NODE_MAP(gMenuChSwitch);
DECLARE_SM_NODE_MAP(gStoreOptionSetupMenu);
DECLARE_SM_NODE_MAP(gPopMenuInputBoxMenu);
DECLARE_SM_NODE_MAP(gPopMenuYesOrNoMenu);
DECLARE_SM_NODE_MAP(gMenuShowCurveSm);
DECLARE_SM_NODE_MAP(gMenuParamSetupSm);
DECLARE_SM_NODE_MAP(gMenuSearchInfoSm);
DECLARE_SM_NODE_MAP(gSearchWarnSm);






static const struct SM_NODE_MAP  gFuncMapTab[] = {
    UI_NODE_WELCOME, &gMenuWelcome, 
    UI_NODE_MAINMENU, &gMenuMainSm, 
    UI_NODE_CHECKSEUP, &gMenuCheckModeCfg, 
    UI_NODE_SEARCHOPT, &gMenuSearchOption, 
    UI_NODE_SETUPOPT, &gMenuSetupOption, 
    UI_NODE_CHSWITCH, &gMenuChSwitch, 
    UI_NODE_POP_INPUTBOX, &gPopMenuInputBoxMenu,
    UI_NODE_POP_YESORNO, &gPopMenuYesOrNoMenu,
    UI_NODE_SHOWCURVE, &gMenuShowCurveSm,
    UI_NODE_PARAMSETUP, &gMenuParamSetupSm,
    UI_NODE_SEARCHINFO, &gMenuSearchInfoSm,
    UI_NODE_WARNINFO, &gSearchWarnSm,
};



void ui_mmi_init(void)
{
    if (NULL != gUiMmiCtrl)
    {
        return;
    }
        gUiMmiCtrl = &gUiMmiCtrlInstance;
    
    {
        struct SMMGR_INIT_PARAM  smParam = {
            (struct SM_NODE_MAP *)gFuncMapTab, ARRAY_SIZE(gFuncMapTab),
            gSmTraceBuf, ARRAY_SIZE(gSmTraceBuf)
        };
        
        memset(&gSmTraceBuf, 0, sizeof(gSmTraceBuf));
        gUiMmiCtrl->smHandle  = SmMgr_Open(&smParam);
    }

        {
        struct EVTMGR_INIT_PARAM  evtParm = {
            gEventQuenue, ARRAY_SIZE(gEventQuenue),
            NULL, NULL, NULL, NULL
        };
        gUiMmiCtrl->evtHandle = EventMgr_Open(&evtParm);
    }
    {
        struct SWTMR_INIT_PARAM  tmrParm = {
            gSwTmrList, ARRAY_SIZE(gSwTmrList)
        };
        memset(&gSwTmrList, 0, sizeof(gSwTmrList));
        gUiMmiCtrl->tmrHandle = SwTmrMgr_Open(&tmrParm);
    }
    ui_keypad_init();
    ui_menu_init();
    }

void ui_mmi_start(void)
{
    struct EVENT_NODE_ITEM e = {0};

    e.sig = EVENT_SYS_INIT;
    ui_mmi_send_msg(&e);
}

void ui_mmi_enter(enum UI_NODE_NAME name, u_int8 noSaveCurrentToHistory)
{
    struct EVENT_NODE_ITEM e = {0};

    
    if (SMNODE_IS_INVALID(name))
    {
        return;
    }
#ifdef  ENABLE_SM_DEBUG_MODE
    MY_DEBUG("SM_ENTER->%d[%d]\n", name, noSaveCurrentToHistory);
#endif
    
    e.sig = EVENT_SM_TRANS;
    if (noSaveCurrentToHistory)
    {
        SM_EVENT_TRANS_INIT(e.param, name, SM_EXIT_CUR);
    }
    else
    {
        SM_EVENT_TRANS_INIT(e.param, name, 0);
    }
    ui_mmi_send_msg(&e);
}


void ui_mmi_return(u_int8 retLvl)
{
    struct EVENT_NODE_ITEM e = {0};

    if (SMNODE_IS_INVALID(SmMgr_GetCurrent(gUiMmiCtrl->smHandle)))
    {
        return;
    }
#ifdef  ENABLE_SM_DEBUG_MODE
    MY_DEBUG("SM_RETURN[%d]\n", retLvl);
#endif
    
    e.sig = EVENT_SM_EXIT;
    ui_mmi_send_msg(&e);
    
    e.sig = EVENT_SM_RETURN;
    SM_EVENT_RETUEN_INIT(e.param, retLvl);
    ui_mmi_send_msg(&e);
}

void ui_mmi_reg_suspend(void (*suspend)(SM_NODE_HANDLE me, SM_NODE_HANDLE child))
{
    if (NULL == gUiMmiCtrl)
    {
        return;
    }
    
    if (0 == gUiMmiCtrl->smHandle)
    {
        return;
    }
    
    SmMgr_RegSuspend(gUiMmiCtrl->smHandle, suspend);
}

void ui_mmi_reg_resume(void (*resume)(SM_NODE_HANDLE child, SM_NODE_HANDLE me))
{
    if (NULL == gUiMmiCtrl)
    {
        return;
    }
    
    if (0 == gUiMmiCtrl->smHandle)
    {
        return;
    }
    
    SmMgr_RegResume(gUiMmiCtrl->smHandle, resume);
}


static u_int8 ui_mmi_bypass_proc(struct EVENT_NODE_ITEM *e)
{
    u_int8 ret = 0;
    
    if (MSG_IS_SYS(e->sig))
    {
        switch (e->sig)
        {
            case EVENT_SYS_INIT:
                ui_mmi_enter(UI_MENU_DEFAULT, 1);
                ret = 1;
                break;
            default:
                break;
        }
    }
    else if(MSG_IS_SM(e->sig))
    {
        struct EVENT_NODE_ITEM ne;
        SM_NODE_HANDLE dest;
        
#ifdef  ENABLE_SM_DEBUG_MODE
        MY_DEBUG("SM_CORE[%d]\n", e->sig);
#endif
        
        switch (e->sig)
        {
        case EVENT_SM_TRANS:
            dest = SM_EVENT_TRANS_TARGET(e->param);
            if (SMNODE_IS_VALID(dest))
            {
                // new state first trans or close then reopen
                if (SM_EXIT_NEW  & SmMgr_Trans(gUiMmiCtrl->smHandle, dest, SM_EVENT_TRANS_MODE(e->param)))
                {
                    ne.sig = EVENT_SM_INIT;
                    ui_mmi_send_msg(&ne);
                }
            }
            ret = 1;
            break;
        case EVENT_SM_RETURN:
            SmMgr_Return(gUiMmiCtrl->smHandle, SM_EVENT_RETURN_LVL(e->param));
            ret = 1;
            break;
        default:
            break;
        } 

        if (ret)
        {
            if (SMNODE_IS_VALID(SmMgr_GetCurrent(gUiMmiCtrl->smHandle)))
            {
                ne.sig = EVENT_SM_ENTRY;
                ui_mmi_send_msg(&ne);
            }
            else
            {
                ui_menu_reset();
            }
        }
    }
    
    return ret;
}



u_int8 ui_mmi_proc(void)
{
    struct EVENT_NODE_ITEM e;

    if (NULL == gUiMmiCtrl)
    {
        return 0;
    }

    if (gUiMmiCtrl->tmrHandle)
    {
        SwTmrMgr_Proc(gUiMmiCtrl->tmrHandle);
    }
    
    ui_keypad_scan();
    
    if (0 == EventMgr_Read(gUiMmiCtrl->evtHandle, &e, EVENT_OPT_MODE_FIFO))
    {
        return 0;
    }

    //UI_MMI_TRACE("<Recv>:%d\n", e.sig);
    if (ui_mmi_bypass_proc(&e))
    { 
        return 1;
    }

    if (SMNODE_IS_INVALID(SmMgr_GetCurrent(gUiMmiCtrl->smHandle)))
    {
        ui_menu_shotcut_proc(&e);
        return 1;
    }

    if (UI_PROC_NEED_BYPASS(SmMgr_Proc(gUiMmiCtrl->smHandle, &e)))
    {
        //MY_DEBUG("\n\t\tBY PASS...\n");
        ui_menu_bypass_proc(&e);
    }

    return 1;
}

void ui_mmi_close(void)
{
    ui_mmi_deinit();
}

void ui_mmi_open(void)
{
   ui_mmi_init();
}

void ui_mmi_deinit(void)
{
    if (NULL == gUiMmiCtrl)
    {
        return;
    }
    
    ui_menu_deinit();
    ui_keypad_deinit();

    if (gUiMmiCtrl->tmrHandle)
    {
        SwTmrMgr_Close(gUiMmiCtrl->tmrHandle);
        gUiMmiCtrl->tmrHandle = 0;
    }

    
    if (gUiMmiCtrl->smHandle)
    {
        SmMgr_Close(gUiMmiCtrl->smHandle);
        gUiMmiCtrl->smHandle = 0;
    }

    if (gUiMmiCtrl->evtHandle)
    {
        EventMgr_Close(gUiMmiCtrl->evtHandle);
        gUiMmiCtrl->evtHandle = 0;
    }

    gUiMmiCtrl = NULL;
}


void ui_mmi_send_msg(struct EVENT_NODE_ITEM *e)
{
    if (gUiMmiCtrl && gUiMmiCtrl->evtHandle && e)
    {
        EventMgr_Write(gUiMmiCtrl->evtHandle, e, EVENT_OPT_MODE_FIFO);
    }
}


SWTMR_NODE_HANDLE ui_mmi_start_timer(u_int32 ms, void(*callbackFunc)(void *), void *callbackArg, u_int8 isLoop)
{
    if (gUiMmiCtrl->tmrHandle && callbackFunc)
    {
        return SwTmrMgr_Start(gUiMmiCtrl->tmrHandle, ms*1000, 
                              callbackFunc, callbackArg, isLoop);
    }
    return SWTMR_INVALID_NODE;
}

void ui_mmi_stop_timer(SWTMR_NODE_HANDLE tmrHdl)
{
    if (gUiMmiCtrl->tmrHandle)
    {
        SwTmrMgr_Stop(gUiMmiCtrl->tmrHandle, tmrHdl);
    }
}

void ui_mmi_poweroff(void)
{

}


//==========================================================
void uimmi_debug_sm_enter(char *nameStr, SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
#ifdef  ENABLE_SM_DEBUG_MODE
    MY_DEBUG("进入 (%s) parent[%d] -> me[%d]\n", nameStr, parent, me);
#endif
}
void uimmi_debug_sm_exit(char *nameStr, SM_NODE_HANDLE next, SM_NODE_HANDLE me)
{
#ifdef  ENABLE_SM_DEBUG_MODE
    MY_DEBUG("退出 (%s) reurn[%d] <- me[%d]\n", nameStr, next, me);
#endif
}

void uimmi_debug_sm_suspend(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
#ifdef  ENABLE_SM_DEBUG_MODE
    MY_DEBUG("暂停 (%s): me[%d] -> to[%d]\n", nameStr, me, child);
#endif
}

void uimmi_debug_sm_resume(char *nameStr, SM_NODE_HANDLE child, SM_NODE_HANDLE me)
{
#ifdef  ENABLE_SM_DEBUG_MODE
    MY_DEBUG("继续 (%s): from[%d] -> me[%d]\n", nameStr, child, me);
#endif
}



u_int8 uimmi_debug_sm_handle(char *nameStr, SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{   
#ifdef  ENABLE_SM_DEBUG_MODE
	SM_NODE_HANDLE targetSm = 0;
    MY_DEBUG("处理中 (%s): me[%d]  evt[%d]\n", nameStr, me, e->sig);
#if 0
    if (MSG_IS_CHAR(e->sig))
    {
        if (KEY_EVENT_IS_DIGIT(e->param))
        {
            switch (KEY_EVENT_DIGIT(e->param))
            {
                case 0:
                    targetSm = UI_NODE_CHECKSEUP;
                    break;
                case 1:
                    targetSm = UI_NODE_MAINMENU;
                    break;
                case 2:
                    targetSm = UI_NODE_SEARCHOPT;
                    break;
                case 3:
                    targetSm = UI_NODE_SETUPOPT;
                    break;
                case 4:
                    targetSm = UI_NODE_CHSWITCH;
                    break;
                case 5:
                    targetSm = UI_NODE_SEARCHINFO;
                    break;
                case 6:
                    targetSm = UI_NODE_PARAMSETUP;
                    break;
                case 7:
                    targetSm = UI_NODE_WARNINFO;
                    break;
                case 8:
                    targetSm = UI_NODE_SHOWCURVE;
                    break;
                default:
                    break;
            }
        }
    }
    else
    switch (e->sig)
    {
        case EVENT_KEY_OK:
            targetSm = UI_NODE_WELCOME;
            break;
        default:
            break;
        
    }

    if (targetSm > 0)
        ui_mmi_enter(targetSm, 1);
#endif
#endif
    return UI_PROC_RET_DFT;
    
}

//==========================================================


