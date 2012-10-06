#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME            "参数设置"

#define THIS_MENU_SM_HANDLE      (gMenuParamSetupSm)
#define THIS_MENU_UI_CONTAINER   (gMenuParamSetupMenu)

#define MENU_PARAMSETUP_EVT_NEXT_ITEM    (0x44)


#define PARAMSETUP_LIST_NUM  (3)


struct MENU_PARAMSETUP_CTRL {
    u_int16 upVal;
    u_int16 reverseVal;
};

static struct MENU_PARAMSETUP_CTRL gMenuParamSetupCtrl = {0};

#define IDX_PARAMSETUP_LOWER_LIMIT    (0)
#define IDX_PARAMSETUP_REVERSE_VOL    (1)
#define IDX_PARAMSETUP_OK             (2)





#define PARAMSETUP_LIST_CELL_H  18
#define PARAMSETUP_LIST_X(x)   (x+2)
#define PARAMSETUP_LIST_Y(y)   ((y*(PARAMSETUP_LIST_CELL_H+2))+2)
#define PARAMSETUP_LIST_YY(y)   (PARAMSETUP_LIST_Y(y)+1)

static u_int8 paramsetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static T_UICOM_DRAW_MODE paramsetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(THIS_MENU_UI_CONTAINER, paramsetup_menu_cell_zone_int, paramsetup_menu_cell_data_int);

static u_int8 paramsetup_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[PARAMSETUP_LIST_NUM] = {
        {PARAMSETUP_LIST_X(0), PARAMSETUP_LIST_Y(0), 122, PARAMSETUP_LIST_CELL_H},
        {PARAMSETUP_LIST_X(0), PARAMSETUP_LIST_Y(1), 122, PARAMSETUP_LIST_CELL_H},
        {PARAMSETUP_LIST_X(30), PARAMSETUP_LIST_Y(2), 60, PARAMSETUP_LIST_CELL_H},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    switch (pos)
    {
        case IDX_PARAMSETUP_LOWER_LIMIT:
            count =  3;
            break;
        case IDX_PARAMSETUP_REVERSE_VOL:
            count =  3;
            break;
        case IDX_PARAMSETUP_OK://确认
            count =  1;
        default:
            break;
    }
    
    return count;
}


u_int16 paramsetup_val_apply(u_int8 checktype, p_void inputdata, u_int8 len, p_void param)
{
    u_int8 id;

    if (INPUT_IF_DEFAULT(checktype))
    {
        id = (u_int8)param;
        //MY_DEBUG("SET_VAL:%d, %d, %d\n", inputdata, id , param);
        if (IDX_PARAMSETUP_LOWER_LIMIT == id)
        {
            gMenuParamSetupCtrl.upVal = (u_int16)inputdata;
        }
        else if (IDX_PARAMSETUP_REVERSE_VOL == id)
        {
            gMenuParamSetupCtrl.reverseVal = (u_int16)inputdata;
        }
    }

    if (INPUT_IF_ENTER(checktype))
    {
        struct EVENT_NODE_ITEM e;
        
        e.sig = EVENT_USR_MMI;
        USR_EVENT_MMI_INIT(e.param, MENU_PARAMSETUP_EVT_NEXT_ITEM);
        ui_mmi_send_msg(&e);
    }
    
    
    return INPUT_STATUS_DEFAULT_ECHO(checktype);
}


#define PARAMSETUP_LIST_BOX_W  16
#define PARAMSETUP_LIST_BOX_H  13
static T_UICOM_DRAW_MODE paramsetup_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8 colPosIdx = 0, focus = 0;
    T_UICOM_DRAW_MODE status  = DRAW_MODE_TEXT_ONLY;
    T_UICOM_EVENT type = (*p_type);
    
    struct SCREEN_ZONE colPosTable[3] = {
        {1,   3,  PARAMSETUP_LIST_BOX_W+2,PARAMSETUP_LIST_BOX_H},
        {60,  3,  PARAMSETUP_LIST_BOX_W,PARAMSETUP_LIST_BOX_H},
        {78,  3,  PARAMSETUP_LIST_BOX_W,PARAMSETUP_LIST_BOX_H},
    };

    SCREEN_BORDER_INIT(&zone->border, 1, 1, 1, 1);
    colPosIdx = childIdx;
    switch (childIdx)
    {
        case 0:
            break;
        case 1:
            if ((pos == IDX_PARAMSETUP_LOWER_LIMIT) || (pos == IDX_PARAMSETUP_REVERSE_VOL))
            {
                status = DRAW_MODE_TEXT_BOX;
            }
            break;
        case 2:
            
            break;
        default:
            break;
    }

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);

    if (IDX_PARAMSETUP_LOWER_LIMIT == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_JIXIANZHIXIAXIAN);
                break;
            case 2:
                UICOM_DATA_FILL(item, UICOM_STR_DANWEIFU);
                zone->zone.x   += 16;
                break;
            case 1:
                if (UICOM_TST_SLECTED(type))
                {
                    focus = gmenu_input_getstr(UICOM_DATA_BUF(item));
                    UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(focus));
                }
                else
                {
                    gmenu_input_makestr(GMENU_INPUT_TYPE_VALUE, UICOM_DATA_BUF(item), (p_void)gMenuParamSetupCtrl.upVal);
                }
                zone->zone.x   += 16;
                zone->border.l += 2;
                zone->border.t += 1;
                break;
            default:
                break;
        }

    }
    else if (IDX_PARAMSETUP_REVERSE_VOL == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_FANXIANGDIANYA);
                break;
            case 2:
                UICOM_DATA_FILL(item, UICOM_STR_DANWEIFU);
                break;
            case 1:
                if (UICOM_TST_SLECTED(type))
                {
                    focus = gmenu_input_getstr(UICOM_DATA_BUF(item));
                    UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(focus));
                }
                else
                {
                    gmenu_input_makestr(GMENU_INPUT_TYPE_VALUE, UICOM_DATA_BUF(item), (p_void)gMenuParamSetupCtrl.reverseVal);
                }
                zone->zone.x += 2;
                //zone->zone.w += 6;
                zone->border.l += 2;
                zone->border.t += 1;
                break;
            default:
                break;
        }

    }
    else if (IDX_PARAMSETUP_OK == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                zone->zone.x += 16;
                zone->border.t = 1;
                break;
            default:
                break;
        }
    }

    return status;

}

static void paramsetup_menu_paint(u_int8 curfocus, u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&THIS_MENU_UI_CONTAINER, 0);
    }
    gmenu_content_list_draw(&THIS_MENU_UI_CONTAINER, PARAMSETUP_LIST_NUM, curfocus);
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


    gMenuParamSetupCtrl.upVal = 2;
    gMenuParamSetupCtrl.reverseVal  = 5;
}

static void paramsetup_menu_update_focus(void)
{
    gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
}

static void paramsetup_menu_inputbox_open(u_int8 curfocus, bool_t focusTail)
{
    struct gmenu_input_config  inputcfg = {0};
    struct gmenu_input_param   initparam = {0};
    
    inputcfg.type = GMENU_INPUT_TYPE_VALUE;
    inputcfg.param = (p_void)curfocus;
    inputcfg.inputdetector = paramsetup_val_apply;
    inputcfg.checktype = INPUT_CHECK_DEFAULT;
    inputcfg.limitlen = 1;
    if (curfocus == IDX_PARAMSETUP_LOWER_LIMIT)
    {
        initparam.isfocustail = focusTail;
        initparam.initdata = (p_void)gMenuParamSetupCtrl.upVal;
    }
    else
    {
        initparam.isfocustail = focusTail;
        initparam.initdata = (p_void)gMenuParamSetupCtrl.reverseVal;
    }
    gmenu_input_open(&inputcfg, &initparam, 0);
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    u_int8 curfocus;

    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        paramsetup_menu_inputbox_open(0, FALSE);
        paramsetup_menu_paint(0, FALSE);
        return UI_PROC_RET_FINISH;
    }
    
    curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
    if ((curfocus == IDX_PARAMSETUP_LOWER_LIMIT) || (curfocus == IDX_PARAMSETUP_REVERSE_VOL) )
    {
        if (gmenu_input_default_proc(e))
        {
            paramsetup_menu_update_focus();
            return UI_PROC_RET_FINISH;
        }
    }

    switch (e->sig)
    {
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, (EVENT_KEY_DOWN == e->sig)?1:-1, 1);
        curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
        if ((curfocus == IDX_PARAMSETUP_LOWER_LIMIT) || (curfocus == IDX_PARAMSETUP_REVERSE_VOL) )
        {
            paramsetup_menu_inputbox_open(curfocus, FALSE);
            paramsetup_menu_update_focus();
        }
        break;
        case EVENT_KEY_OK:
        paramsetup_menu_update_focus();
        if (curfocus == IDX_PARAMSETUP_OK)
        {
            ui_mmi_return(1);
        }
        return UI_PROC_RET_FINISH;
        break;
        case EVENT_USR_MMI:
        {
            u_int32 myevt = USR_EVENT_MMI(e->param);

            if (MENU_PARAMSETUP_EVT_NEXT_ITEM == myevt)
            {
                gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 1, 0);
                curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
                if ((curfocus == IDX_PARAMSETUP_LOWER_LIMIT) || (curfocus == IDX_PARAMSETUP_REVERSE_VOL) )
                {
                    paramsetup_menu_inputbox_open(curfocus, FALSE);
                    paramsetup_menu_update_focus();
                }
            }
        }
        return UI_PROC_RET_FINISH;
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








