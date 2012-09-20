#include "uimmi_ctrl.h"
#include "uikeypad_ctrl.h"
#include "uimenu_ctrl.h"

#define UI_MMI_TRACE(f, v)   MY_DEBUG(f, v)


static struct SM_NODE_ITEM    gSmTraceBuf[4];
static struct EVENT_NODE_ITEM gEventQuenue[10];
static struct SWTMR_NODE_ITEM gSwTmrList[4];

struct UI_MMI_CTRL {
    HANDLE smHandle;
    HANDLE evtHandle;
    HANDLE tmrHandle;
};

static struct UI_MMI_CTRL gUiMmiCtrlInstance;
static struct UI_MMI_CTRL *gUiMmiCtrl = NULL;

static u_int8 ui_mmi_bypass_proc(struct EVENT_NODE_ITEM *e);


//=================== menu func map =============================
DECLARE_SM_NODE_MAP(gMenu_PubBranchMenu);
DECLARE_SM_NODE_MAP(gMenu_PowerOn);
DECLARE_SM_NODE_MAP(gMenuWelcome);
DECLARE_SM_NODE_MAP(gMenuStopCheck);
DECLARE_SM_NODE_MAP(gMenuMain);
DECLARE_SM_NODE_MAP(gMenuCheckModeCfg);
DECLARE_SM_NODE_MAP(gMenuSearchOption);
DECLARE_SM_NODE_MAP(gMenuSetupOption);
DECLARE_SM_NODE_MAP(gMenuChSwitch);

static const struct SM_NODE_MAP  gFuncMapTab[] = {
    UI_NODE_BRANCH_00, &gMenu_PowerOn,
    UI_NODE_BRANCH_01, &gMenuStopCheck,
    UI_NODE_LEAF_00, &gMenuWelcome, 
    UI_NODE_LEAF_01, &gMenuMain, 
    UI_NODE_LEAF_02, &gMenuCheckModeCfg, 
    UI_NODE_LEAF_03, &gMenuSearchOption, 
    UI_NODE_LEAF_04, &gMenuSetupOption, 
    UI_NODE_LEAF_05, &gMenuChSwitch, 
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
            (struct SM_NODE_MAP  *)gFuncMapTab, ARRAY_SIZE(gFuncMapTab),
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
    
    e.sig = EVENT_SM_TRANS;
    e.param.sm.trans.target      = name;
    e.param.sm.trans.quitCurrent = noSaveCurrentToHistory;

    ui_mmi_send_msg(&e);
}


void ui_mmi_return(u_int8 retLvl)
{
    SM_NODE_HANDLE current = SmMgr_GetCurrent(gUiMmiCtrl->smHandle);
    struct EVENT_NODE_ITEM e = {0};

    if (SMNODE_IS_INVALID(current))
    {
        return;
    }
    
    e.sig = EVENT_SM_EXIT;
    ui_mmi_send_msg(&e);
    
    e.sig = EVENT_SM_RETURN;
    e.param.sm.ret.retLvl = retLvl;
    
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
    u_int8 ret = 0, flag = 0;
    
    if (MSG_IS_SYS(e->sig))
    {
        switch (e->sig)
        {
            case EVENT_SYS_INIT:
                ui_mmi_enter(UI_NODE_LEAF_00, 1);
                ret = 1;
                break;
            default:
                break;
        }
    }
    else if(MSG_IS_SM(e->sig))
    {
        union SM_EVENT_PARAM *smEvt = (union SM_EVENT_PARAM *)&(e->param);
        struct EVENT_NODE_ITEM ne;
        SM_NODE_HANDLE  node;
        
        switch (e->sig)
        {
        case EVENT_SM_TRANS:
            if (SMNODE_IS_VALID(smEvt->trans.target))
            {
                flag = SmMgr_IsInHistory(gUiMmiCtrl->smHandle, smEvt->trans.target);
                node = SmMgr_Trans(gUiMmiCtrl->smHandle, smEvt->trans.target, smEvt->trans.quitCurrent);
                // trans sm node, and trans successuful
                if (SMNODE_IS_VALID(node) && (node == smEvt->trans.target))
                {
                    if ((!flag) || (flag && (smEvt->trans.quitCurrent)))// new state first trans or close then reopen
                    {
                        ne.sig = EVENT_SM_INIT;
                        ui_mmi_send_msg(&ne);
                    }
                    flag = 1;
                }
                else
                {
                    MY_DEBUG("\nSys Trans Failed.\n");
                    flag = 0;
                }
            }
            ret = 1;
            break;
        case EVENT_SM_RETURN:
            SmMgr_Return(gUiMmiCtrl->smHandle, smEvt->ret.retLvl);
            ret = 1;
            flag = 1;
            break;
        default:
            break;
        } 

        if (flag && SMNODE_IS_VALID(SmMgr_GetCurrent(gUiMmiCtrl->smHandle)))
        {
            ne.sig = EVENT_SM_ENTRY;
            ui_mmi_send_msg(&ne);
        }
    }
    
    return ret;
}



void ui_mmi_proc(void)
{
    struct EVENT_NODE_ITEM e;

    if (NULL == gUiMmiCtrl)
    {
        return;
    }

    if (gUiMmiCtrl->tmrHandle)
    {
        SwTmrMgr_Proc(gUiMmiCtrl->tmrHandle);
    }
    
    ui_keypad_scan();
    
    if (0 == EventMgr_Read(gUiMmiCtrl->evtHandle, &e, EVENT_OPT_MODE_FIFO))
    {
        return;
    }

    //UI_MMI_TRACE("<Recv>:%d\n", e.sig);
    if (ui_mmi_bypass_proc(&e))
    { 
        return;
    }

    if (SMNODE_IS_INVALID(SmMgr_GetCurrent(gUiMmiCtrl->smHandle)))
    {
        ui_menu_shotcut_proc(&e);
        return;
    }
    
    if (ui_menu_bypass_proc(&e))
    {
        return;
    }
    
    SmMgr_Proc(gUiMmiCtrl->smHandle, &e);

    
}

void ui_mmi_close(void)
{
    ui_mmi_deinit();
    //exit(0);
}

void ui_mmi_open(void)
{
    Screen_PrintInit();
    Screen_PrintClear(NULL);
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


enum UI_NODE_TYPE ui_mmi_nodetype(enum UI_NODE_NAME name)
{
    if ((name > UI_NODE_START) && (name < UI_NODE_SEPERATE))
    {
        return UI_NODE_TYPE_BRANCH;
    }
    else if ((name > UI_NODE_SEPERATE) && (name < UI_NODE_END))
    {
        return UI_NODE_TYPE_LEAF;
    }
    else
    {
        return UI_NODE_TYPE_NULL;
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


//==========================================================
void ui_mmi_debug_enter(char *nameStr, SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    printf_debug("Open", me,nameStr);
}
void ui_mmi_debug_exit(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    printf_debug("Close", me,nameStr);
}

void ui_mmi_debug_suspend(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    printf_debug("Suspend", me,nameStr);
}

void ui_mmi_debug_resume(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    printf_debug("Resume", me,nameStr);
}



void ui_mmi_debug_handle(char *nameStr, SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    printf_debug("Proc", me,nameStr);

    switch (e->sig)
    {
        case EVENT_KEY_STOP:
            ui_mmi_enter(UI_NODE_BRANCH_01, 0);
            break;
        
        case EVENT_KEY_OK:
            ui_mmi_enter(UI_NODE_LEAF_00, 0);
            break;
        case EVENT_KEY_NUM_1:
            ui_mmi_enter(UI_NODE_LEAF_01, 0);
            break;
        case EVENT_KEY_NUM_2:
            ui_mmi_enter(UI_NODE_LEAF_02, 0);
            break;
        case EVENT_KEY_NUM_3:
            ui_mmi_enter(UI_NODE_LEAF_03, 0);
            break;
        case EVENT_KEY_NUM_4:
            ui_mmi_enter(UI_NODE_LEAF_04, 0);
            break;
        case EVENT_KEY_NUM_5:
            ui_mmi_enter(UI_NODE_LEAF_05, 0);
            break;
        case EVENT_KEY_NUM_6:
            ui_mmi_enter(UI_NODE_BRANCH_01, 0);
            break;
        case EVENT_KEY_NUM_7:
            ui_mmi_enter(UI_NODE_LEAF_01, 1);
            break;
        case EVENT_KEY_NUM_8:
            ui_mmi_enter(UI_NODE_LEAF_02, 1);
            break;
            
        case EVENT_KEY_NUM_9:
            ui_mmi_enter(UI_NODE_LEAF_03, 1);
            break;
            
        case EVENT_KEY_SUB:
            ui_mmi_return(1);
            break;
        case EVENT_KEY_NUM_0:
            ui_mmi_return(3);
            break;
        default:
            break;
        
    }
}

//==========================================================


