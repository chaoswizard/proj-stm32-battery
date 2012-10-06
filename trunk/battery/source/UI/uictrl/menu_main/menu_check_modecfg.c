#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME            "监测模式设置选项"

#define THIS_MENU_SM_HANDLE      (gMenuCheckModeCfg)
#define THIS_MENU_UI_CONTAINER   (gCheckModeSetupMenu)

//"MonitorModeCfg"

#define MONITORSETUP_LIST_NUM  (6)


struct MENU_CHECKMODECFG_CTRL {
    u_int8 bitmapItemStatus;
    u_int16 valMin;
    u_int16 valmV;
};

static struct MENU_CHECKMODECFG_CTRL gMenuCheckModeCfgCtrl = {0};

#define CHECKMODE_ITEM_SET_STATUS(idx)  (gMenuCheckModeCfgCtrl.bitmapItemStatus = UTIL_BIT_SET(gMenuCheckModeCfgCtrl.bitmapItemStatus, (idx)))
#define CHECKMODE_ITEM_CLR_STATUS(idx)  (gMenuCheckModeCfgCtrl.bitmapItemStatus = UTIL_BIT_CLR(gMenuCheckModeCfgCtrl.bitmapItemStatus, (idx)))
#define CHECKMODE_ITEM_CPL_STATUS(idx)  (gMenuCheckModeCfgCtrl.bitmapItemStatus = UTIL_BIT_CPL(gMenuCheckModeCfgCtrl.bitmapItemStatus, (idx)))
#define CHECKMODE_ITEM_TST_STATUS(idx)  UTIL_BIT_TST(gMenuCheckModeCfgCtrl.bitmapItemStatus, (idx))

#define CHECKMODE_ITEM_SET_STATUS_RANGE(idxStart, idxEnd)  (gMenuCheckModeCfgCtrl.bitmapItemStatus = UTIL_BIT_SET_RANGE(MenuCheckModeCfgCtrl.bitmapItemStatus, idxStart, idxEnd))
#define CHECKMODE_ITEM_CLR_STATUS_RANGE(idxStart, idxEnd)  (gMenuCheckModeCfgCtrl.bitmapItemStatus = UTIL_BIT_CLR_RANGE(MenuCheckModeCfgCtrl.bitmapItemStatus, idxStart, idxEnd))
#define CHECKMODE_ITEM_TST_STATUS_RANGE(idxStart, idxEnd)   UTIL_BIT_TST_RANGE(gMenuCheckModeCfgCtrl.bitmapItemStatus, idxStart, idxEnd)

#define IDX_CKMODE_TIME_SEL   (0)
#define IDX_CKMODE_VOL_SEL    (1)
#define IDX_CKMODE_MIXED_SEL  (2)
#define IDX_CKMODE_TIME_VAL   (3)
#define IDX_CKMODE_VOL_VAL    (4)
#define IDX_CKMODE_OK         (5)





#define MONITORSET_LIST_CELL_H  18
#define MONITORSET_LIST_X(x)   (x+2)
#define MONITORSET_LIST_Y(y)   ((y*(MONITORSET_LIST_CELL_H+2))+2)
#define MONITORSET_LIST_YY(y)   (MONITORSET_LIST_Y(y)+1)

static u_int8 modesetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static T_UICOM_DRAW_MODE modesetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(THIS_MENU_UI_CONTAINER, modesetup_menu_cell_zone_int, modesetup_menu_cell_data_int);

static u_int8 modesetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[MONITORSETUP_LIST_NUM] = {
        {MONITORSET_LIST_X(0), MONITORSET_LIST_Y(0), 60, MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(0), MONITORSET_LIST_Y(1), 60, MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(0), MONITORSET_LIST_Y(2), 60, MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(63),MONITORSET_LIST_Y(0), 60, MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(63),MONITORSET_LIST_Y(1), 60, MONITORSET_LIST_CELL_H},
        {MONITORSET_LIST_X(63),MONITORSET_LIST_Y(2), 60, MONITORSET_LIST_CELL_H},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    switch (pos)
    {
        case IDX_CKMODE_TIME_SEL://按时间
            count =  2;
            break;
        case IDX_CKMODE_VOL_SEL://按电压
            count =  2;
            break;
        case IDX_CKMODE_MIXED_SEL://混合模式
            count =  2;
            break;
        case IDX_CKMODE_TIME_VAL:
            count =  2;
            break;
        case IDX_CKMODE_VOL_VAL:
            count =  2;
            break;
        case IDX_CKMODE_OK://确认
            count =  1;
        default:
            break;
    }
    
    return count;
}


u_int16 modesetup_val_apply(u_int8 checktype, p_void inputdata, u_int8 len, p_void param)
{
    u_int8 id;

    if (INPUT_IF_DEFAULT(checktype))
    {
        id = (u_int8)param;
        //MY_DEBUG("SET_VAL:%d, %d, %d\n", inputdata, id , param);
        if (IDX_CKMODE_TIME_VAL == id)
        {
            gMenuCheckModeCfgCtrl.valMin = (u_int16)inputdata;
        }
        else if (IDX_CKMODE_VOL_VAL == id)
        {
            gMenuCheckModeCfgCtrl.valmV = (u_int16)inputdata;
        }
    }
    
    return INPUT_STATUS_DEFAULT_ECHO(checktype);
}

static void modesetup_menu_option_on(PUICOM_DATA item, bool_t isOn)
{
    if (isOn)
    {
        UICOM_DATA_FILL(item, UICOM_STR_YUANQUAN);
    }
    else
    {
        UICOM_DATA_FILL(item, NULL);
    }
}

#define MONITORSET_LIST_BOX_W  28
#define MONITORSET_LIST_BOX_H  13
static T_UICOM_DRAW_MODE modesetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8 colPosIdx = 0, focus = 0;
    T_UICOM_DRAW_MODE status  = DRAW_MODE_TEXT_ONLY;
    T_UICOM_EVENT type = (*p_type);
    
    struct SCREEN_ZONE colPosTable[3] = {
        {1,   3,  MONITORSET_LIST_BOX_W+2,MONITORSET_LIST_BOX_H},
        {40,  2,  MONITORSET_LIST_BOX_W,MONITORSET_LIST_BOX_H},
        {48,  2,  MONITORSET_LIST_BOX_W,MONITORSET_LIST_BOX_H},
    };

    SCREEN_BORDER_INIT(&zone->border, 1, 1, 1, 1);
    switch (childIdx)
    {
        case 0:
            colPosIdx = 0;
            if ((pos == IDX_CKMODE_TIME_VAL) || (pos == IDX_CKMODE_VOL_VAL))
            {
                status = DRAW_MODE_TEXT_BOX;
            }
            break;
        case 1:
            if ((pos == IDX_CKMODE_MIXED_SEL) || (pos == IDX_CKMODE_TIME_SEL) || (pos == IDX_CKMODE_VOL_SEL))
            {
                colPosIdx = 2;
            }
            else
            {
                colPosIdx = 1;
            }
            break;
        default:
            break;
    }

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);

    if (IDX_CKMODE_TIME_SEL == pos)//按时间
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_ANSHIJIAN);
                break;
            case 1:
                modesetup_menu_option_on(item, CHECKMODE_ITEM_TST_STATUS(pos));
                break;
            default:
                break;
        }

    }
    else if (IDX_CKMODE_TIME_VAL == pos)
    {
        switch (childIdx)
        {
            case 0:
                if (UICOM_TST_SLECTED(type))
                {
                    focus = gmenu_input_getstr(UICOM_DATA_BUF(item));
                    UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(focus));
                }
                else
                {
                    gmenu_input_makestr(GMENU_INPUT_TYPE_VALUE, UICOM_DATA_BUF(item), (p_void)gMenuCheckModeCfgCtrl.valMin);
                }
                zone->zone.x   += 6;
                zone->border.l += 2;
                zone->border.t += 1;
                break;
            case 1:
                UICOM_DATA_FILL(item, UICOM_STR_DANWEIFENZHONG);
                break;
            default:
                break;
        }

    }
    else if (IDX_CKMODE_VOL_SEL == pos)//按电压
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_ANDIANYA);
                break;
            case 1:
                modesetup_menu_option_on(item, CHECKMODE_ITEM_TST_STATUS(pos));
                break;
            default:
                break;
        }
    }
    else if (IDX_CKMODE_VOL_VAL == pos)
    {
        switch (childIdx)
        {
            case 0:
                if (UICOM_TST_SLECTED(type))
                {
                    focus = gmenu_input_getstr(UICOM_DATA_BUF(item));
                    UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(focus));
                }
                else
                {
                    gmenu_input_makestr(GMENU_INPUT_TYPE_VALUE, UICOM_DATA_BUF(item), (p_void)gMenuCheckModeCfgCtrl.valmV);
                }
                zone->zone.x += 2;
                zone->zone.w += 6;
                zone->border.l += 2;
                zone->border.t += 1;
                break;
            case 1:
                UICOM_DATA_FILL(item, UICOM_STR_DANWEIHAOFU);
                break;
            default:
                break;
        }

    }
    else if (IDX_CKMODE_MIXED_SEL == pos)//混合模式
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_HUNHEMOSHI);
                break;
            case 1:
                modesetup_menu_option_on(item, CHECKMODE_ITEM_TST_STATUS(pos));
                break;
            default:
                break;
        }
    }
    else if (IDX_CKMODE_OK == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                zone->zone.x += 1;
                zone->border.t = 1;
                break;
            default:
                break;
        }
    }

    return status;

}

static void monitorsetup_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&THIS_MENU_UI_CONTAINER, 0);
    }
    gmenu_content_list_draw(&THIS_MENU_UI_CONTAINER, MONITORSETUP_LIST_NUM, 0);
}



static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(THIS_MENU_SM_HANDLE,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);




static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_SUSPEND(THIS_MENU_NAME, me, child);
}

static void menu_pub_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_RESUME(THIS_MENU_NAME, child, me);
}


static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    UIMMI_DEBUGSM_ENTER(THIS_MENU_NAME, parent, me);
    ui_mmi_reg_suspend(menu_pub_suspend);
    ui_mmi_reg_resume(menu_pub_resume);

    CHECKMODE_ITEM_CLR_STATUS(IDX_CKMODE_TIME_SEL);
    CHECKMODE_ITEM_SET_STATUS(IDX_CKMODE_VOL_SEL);
    CHECKMODE_ITEM_SET_STATUS(IDX_CKMODE_MIXED_SEL);

    gMenuCheckModeCfgCtrl.valMin = 123;
    gMenuCheckModeCfgCtrl.valmV  = 456;
}

static void monitorsetup_menu_update_focus(void)
{
    gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
}
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    u_int8 curfocus;
    
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);

    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        monitorsetup_menu_paint(0);
        return UI_PROC_RET_FINISH;
    }
    
    curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
    if ((curfocus == IDX_CKMODE_TIME_VAL) || (curfocus == IDX_CKMODE_VOL_VAL) )
    {
        if (gmenu_input_default_proc(e))
        {
            monitorsetup_menu_update_focus();
            return UI_PROC_RET_FINISH;
        }
    }

    switch (e->sig)
    {
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, (EVENT_KEY_DOWN == e->sig)?1:-1, 1);
        curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
        if ((curfocus == IDX_CKMODE_TIME_VAL) || (curfocus == IDX_CKMODE_VOL_VAL) )
        {
            struct gmenu_input_config  inputcfg = {0};
            struct gmenu_input_param   initparam = {0};
            
            
            inputcfg.type = GMENU_INPUT_TYPE_VALUE;
            inputcfg.param = (p_void)curfocus;
            inputcfg.inputdetector = modesetup_val_apply;
            inputcfg.checktype = INPUT_CHECK_DEFAULT;
            inputcfg.limitlen = 4;
            if (curfocus == IDX_CKMODE_TIME_VAL)
            {
                initparam.isfocustail = FALSE;
                initparam.initdata = (p_void)gMenuCheckModeCfgCtrl.valMin;
            }
            else
            {
                initparam.isfocustail = FALSE;
                initparam.initdata = (p_void)gMenuCheckModeCfgCtrl.valmV;
            }
            gmenu_input_open(&inputcfg, &initparam, 0);
            monitorsetup_menu_update_focus();
        }
        break;
        case EVENT_KEY_OK:
        CHECKMODE_ITEM_CPL_STATUS(curfocus);
        monitorsetup_menu_update_focus();
        if (curfocus == IDX_CKMODE_OK)
        {
            ui_mmi_return(1);
        }
        return UI_PROC_RET_FINISH;
        break;
        default:
            break;
        
    }
    return UI_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    UIMMI_DEBUGSM_EXIT(THIS_MENU_NAME, next, me);
    gmenu_input_close();
}




