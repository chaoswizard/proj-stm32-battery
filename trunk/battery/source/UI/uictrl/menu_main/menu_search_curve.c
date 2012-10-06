#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "查询选项"
//"SearchOption"

#define MAX_DISP_CURSE_NUM (4)


#define MENU_CURSE_EVT_NEXT_ITEM  (0x88)
#define MENU_CURSE_EVT_PRE_ITEM   (0x22)

struct MENU_CURSE_DISP_CFG_CTRL {
    u_int16 startNo;
    u_int16 endNo;
    u_int16 curseTab[MAX_DISP_CURSE_NUM];
};

static struct MENU_CURSE_DISP_CFG_CTRL gMenuCurseDisCfgCtr = {0};


#define THIS_MENU_SM_HANDLE      (gMenuSearchOption)
#define THIS_MENU_UI_CONTAINER   (gSearchoptionsetupList)


#define SEARCHOPT_LIST_NUM  (6)


#define IDX_EARCHOPT_VAL_START (0)
#define IDX_EARCHOPT_VAL_END   (1)
#define IDX_EARCHOPT_VAL_C1    (2)
#define IDX_EARCHOPT_VAL_C2    (3)
#define IDX_EARCHOPT_VAL_C3    (4)
#define IDX_EARCHOPT_VAL_C4    (5)



#define SEARCHOPT_LIST_CELL_H  13
#define SEARCHOPT_LIST_CELL_W  29
#define SEARCHOPT_LIST_X(x)   (x*(SEARCHOPT_LIST_CELL_W+2)+3)
#define SEARCHOPT_LIST_Y(y)   ((y*(SEARCHOPT_LIST_CELL_H+2))+1)

static u_int8 searchopt_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static T_UICOM_DRAW_MODE searchopt_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(THIS_MENU_UI_CONTAINER, searchopt_menu_cell_zone_int, searchopt_menu_cell_data_int);

static u_int8 searchopt_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    struct SCREEN_ZONE cellPosTable[SEARCHOPT_LIST_NUM] = {
        {SEARCHOPT_LIST_X(1)-4,  SEARCHOPT_LIST_Y(1), SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H},
        {SEARCHOPT_LIST_X(3)-4, SEARCHOPT_LIST_Y(1),  SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H},
        {SEARCHOPT_LIST_X(0),  SEARCHOPT_LIST_Y(3)+1, SEARCHOPT_LIST_CELL_W-1, SEARCHOPT_LIST_CELL_H},
        {SEARCHOPT_LIST_X(1)-1,  SEARCHOPT_LIST_Y(3)+1, SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H},
        {SEARCHOPT_LIST_X(2)-1,  SEARCHOPT_LIST_Y(3)+1, SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H},
        {SEARCHOPT_LIST_X(3)-1,  SEARCHOPT_LIST_Y(3)+1, SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    
    return 1;
}

#define SEARCHOPT_LIST_BOX_W  36
#define SEARCHOPT_LIST_BOX_H  13

static T_UICOM_DRAW_MODE searchopt_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8  focus;
    u_int16 val;
    T_UICOM_EVENT type = (*p_type);
    
    //SCREEN_BORDER_INIT(&zone->border, 1, 0, 0, 0);
    SCREEN_ZONE_INIT(&zone->zone, 1, 2, 30, SEARCHOPT_LIST_BOX_H);

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);


    if (IDX_EARCHOPT_VAL_START == pos)
    {
        val = gMenuCurseDisCfgCtr.startNo;
    }
    else if (IDX_EARCHOPT_VAL_END== pos)
    {
        val = gMenuCurseDisCfgCtr.endNo;
    }
    else if ((pos >= IDX_EARCHOPT_VAL_C1) && pos <= IDX_EARCHOPT_VAL_C4)
    {
        val = gMenuCurseDisCfgCtr.curseTab[pos - IDX_EARCHOPT_VAL_C1];
    }

    if (UICOM_TST_SLECTED(type))
    {
        focus = gmenu_input_getstr(UICOM_DATA_BUF(item));
        UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(focus));
        //MY_DEBUG("LCD<%d>: [%s] %d\n", pos, UICOM_DATA_BUF(item), focus);
    }
    else
    {
        gmenu_input_makestr(GMENU_INPUT_TYPE_VALUE, UICOM_DATA_BUF(item), (p_void)val);
    }

    return DRAW_MODE_TEXT_ONLY;//DRAW_MODE_TEXT_ONLY;

}

static void searchoptsetup_menu_border(bool_t en1, bool_t en2)
{
    struct OSD_ZONE zone;

    SCREEN_ZONE_INIT(&zone.zone, 1,  1, 125, 30);
    SCREEN_BORDER_INIT(&zone.border, 1,  1, 1, 1);
    Screen_PrintRect(&zone, en1?PIXEL_MODE_SET:PIXEL_MODE_CLEAR);

    SCREEN_ZONE_INIT(&zone.zone, 1,  33, 125, 29);
    SCREEN_BORDER_INIT(&zone.border, 1,  1, 1, 1);
    Screen_PrintRect(&zone, en2?PIXEL_MODE_SET:PIXEL_MODE_CLEAR);
}


static void searchoptsetup_menu_paint(u_int8 isClear, u_int8 focus)
{
    struct SCREEN_ZONE zone;

    SCREEN_ZONE_INIT(&zone, SEARCHOPT_LIST_X(0),  SEARCHOPT_LIST_Y(0)+2, SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H);
    Screen_PrintString(&zone, "连续曲线显示(1-4条)", TEXT_STYLE_DEFAULT);

    SCREEN_ZONE_INIT(&zone, SEARCHOPT_LIST_X(0),  SEARCHOPT_LIST_Y(1), SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H);
    Screen_PrintString(&zone, "起始:", TEXT_STYLE_DEFAULT);

    SCREEN_ZONE_INIT(&zone, SEARCHOPT_LIST_X(2),  SEARCHOPT_LIST_Y(1), SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H);
    Screen_PrintString(&zone, "结束:", TEXT_STYLE_DEFAULT );


    SCREEN_ZONE_INIT(&zone, SEARCHOPT_LIST_X(0),  SEARCHOPT_LIST_Y(2)+4, SEARCHOPT_LIST_CELL_W, SEARCHOPT_LIST_CELL_H);
    Screen_PrintString(&zone, "非连续曲线显示", TEXT_STYLE_DEFAULT);

    if (isClear)
    {
       gmenu_content_list_clear_all(&THIS_MENU_UI_CONTAINER, 0);
    }
    gmenu_content_list_draw(&THIS_MENU_UI_CONTAINER, SEARCHOPT_LIST_NUM, focus);
    searchoptsetup_menu_border(1, 1);
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
    //memset(&gMenuCurseDisCfgCtr, 0, sizeof(gMenuCurseDisCfgCtr));
}


u_int16 searchoptsetup_val_apply(u_int8 checktype, p_void inputdata, u_int8 len, p_void param)
{
    u_int8 id = (u_int8)param;
    u_int16 val = (u_int16)inputdata;
    struct EVENT_NODE_ITEM e;
    

    
    //MY_DEBUG("SET_VAL:%d, %d, %d\n", inputdata, id , param);
    if (id == IDX_EARCHOPT_VAL_START)
    {
        gMenuCurseDisCfgCtr.startNo = val;
    }
    else if (IDX_EARCHOPT_VAL_END == id)
    {
        gMenuCurseDisCfgCtr.endNo = val;
    }
    else if ((id >= IDX_EARCHOPT_VAL_C1) && id <= IDX_EARCHOPT_VAL_C4)
    {
        gMenuCurseDisCfgCtr.curseTab[id - IDX_EARCHOPT_VAL_C1] = val;;
    }

    if (INPUT_IF_LIMIT(checktype))
    {
        if ((id != IDX_EARCHOPT_VAL_END) && (id != IDX_EARCHOPT_VAL_C4))
        {
            e.sig = EVENT_USR_MMI;
            USR_EVENT_MMI_INIT(e.param, MENU_CURSE_EVT_NEXT_ITEM);
            ui_mmi_send_msg(&e);
        }
    }
    else if (INPUT_IF_DELALL(checktype))
    {
        if ((id != IDX_EARCHOPT_VAL_START) && (id != IDX_EARCHOPT_VAL_C1))
        {
            e.sig = EVENT_USR_MMI;
            USR_EVENT_MMI_INIT(e.param, MENU_CURSE_EVT_PRE_ITEM);
            ui_mmi_send_msg(&e);
        }
    }
    
    if ((id == IDX_EARCHOPT_VAL_START) || (id == IDX_EARCHOPT_VAL_C1))
    {
        return INPUT_STATUS_DEFAULT_ECHO(checktype);

    }
    else
    {
        return  INPUT_STATUS_NO_AUTOQUIT_ECHO(checktype);
    }
}

static void searchoptsetup_menu_inputbox_open(u_int8 curfocus, bool_t focusTail)
{
    struct gmenu_input_config  inputcfg = {0};
    struct gmenu_input_param   initparam = {0};
    
    initparam.isfocustail = focusTail;
    inputcfg.type = GMENU_INPUT_TYPE_VALUE;
    inputcfg.param = (p_void)curfocus;
    inputcfg.inputdetector = searchoptsetup_val_apply;
    inputcfg.checktype = INPUT_CHECK_LIMIT;
    inputcfg.limitlen = 4;
    if (curfocus == IDX_EARCHOPT_VAL_START)
    {
        initparam.initdata = (p_void)gMenuCurseDisCfgCtr.startNo;
    }
    else if (curfocus == IDX_EARCHOPT_VAL_END)
    {
        initparam.initdata = (p_void)gMenuCurseDisCfgCtr.endNo;
    }
    else if ((curfocus >= IDX_EARCHOPT_VAL_C1) && curfocus <= IDX_EARCHOPT_VAL_C4)
    {
        initparam.initdata = (p_void)gMenuCurseDisCfgCtr.curseTab[curfocus - IDX_EARCHOPT_VAL_C1];
    }
    else
    {
        return;
    }
    
    gmenu_input_open(&inputcfg, &initparam, 0);
}


static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);

    if (MSG_IS_ENTRY(e->sig))
    {
        //u_int8 focus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
        
        Screen_PrintClear(NULL);
        searchoptsetup_menu_inputbox_open(0, FALSE);
        searchoptsetup_menu_paint(FALSE, 0);
        return UI_PROC_RET_FINISH;
    }

    if (gmenu_input_default_proc(e))
    {
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
        return UI_PROC_RET_FINISH;
    }

    switch (e->sig)
    {
        case EVENT_KEY_OK:
            gmenu_show_curve(NULL, TRUE);
            return UI_PROC_RET_FINISH;
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, (EVENT_KEY_DOWN == e->sig)?1:-1, 1);
        searchoptsetup_menu_inputbox_open(gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER), FALSE);
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
        break;
        case EVENT_USR_MMI:
        {
            u_int32 myevt = USR_EVENT_MMI(e->param);

            gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 
                (MENU_CURSE_EVT_NEXT_ITEM == (myevt))?1:-1, 0);
            searchoptsetup_menu_inputbox_open(gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER), 
                (MENU_CURSE_EVT_PRE_ITEM == (myevt)));
            gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
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




