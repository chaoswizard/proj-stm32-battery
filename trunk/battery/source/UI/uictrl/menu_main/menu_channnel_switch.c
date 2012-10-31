#include "uimmi_ctrl.h"
#include "gui_menu_item.h"


#define THIS_MENU_NAME  "通道切换选择"
//"SwitchChSel"

#define MAX_CH_NUM   (SYS_AD_CH_MAX)
#define MIN_CH_NUM   (1)

#define THIS_MENU_SM_HANDLE      (gMenuChSwitch)
#define THIS_MENU_UI_CONTAINER   (gMenuChSwUiCon)

struct MENU_CHSWITCH_CTRL {
    u_int16 bitmapItemStatus;
    u_int16 curBaseCh;
    u_int16 curSelCh;
};

static struct MENU_CHSWITCH_CTRL gMenuChSwitchDataCtrl;
#define CHSWITCH_ITEM_SET_STATUS(idx)  (gMenuChSwitchDataCtrl.bitmapItemStatus = UTIL_BIT_SET(gMenuChSwitchDataCtrl.bitmapItemStatus, (idx)))
#define CHSWITCH_ITEM_CLR_STATUS(idx)  (gMenuChSwitchDataCtrl.bitmapItemStatus = UTIL_BIT_CLR(gMenuChSwitchDataCtrl.bitmapItemStatus, (idx)))
#define CHSWITCH_ITEM_CPL_STATUS(idx)  (gMenuChSwitchDataCtrl.bitmapItemStatus = UTIL_BIT_CPL(gMenuChSwitchDataCtrl.bitmapItemStatus, (idx)))
#define CHSWITCH_ITEM_TST_STATUS(idx)  UTIL_BIT_TST(gMenuChSwitchDataCtrl.bitmapItemStatus, (idx))

#define CHSWITCH_ITEM_SET_STATUS_RANGE(idxStart, idxEnd)  (gMenuChSwitchDataCtrl.bitmapItemStatus = UTIL_BIT_SET_RANGE(gMenuChSwitchDataCtrl.bitmapItemStatus, idxStart, idxEnd))
#define CHSWITCH_ITEM_CLR_STATUS_RANGE(idxStart, idxEnd)  (gMenuChSwitchDataCtrl.bitmapItemStatus = UTIL_BIT_CLR_RANGE(gMenuChSwitchDataCtrl.bitmapItemStatus, idxStart, idxEnd))
#define CHSWITCH_ITEM_TST_STATUS_RANGE(idxStart, idxEnd)   UTIL_BIT_TST_RANGE(gMenuChSwitchDataCtrl.bitmapItemStatus, idxStart, idxEnd)


#define CHANNELSW_NUM_ROW   4
#define CHANNELSW_NUM_COL   2

#define CHANNELSW_NUM_PER_PAGE   (CHANNELSW_NUM_COL*CHANNELSW_NUM_ROW)

//-----------------------------------------
#define CHANNELSW_LIST_NUM     (CHANNELSW_NUM_PER_PAGE+3)
#define CHNNELSW_NUM_LINE      CHANNELSW_NUM_ROW
#define IDX_CHANNELSW_GOTO     (CHANNELSW_NUM_PER_PAGE)
#define IDX_CHANNELSW_SEL_ALL  (CHANNELSW_NUM_PER_PAGE+1)
#define IDX_CHANNELSW_OK       (CHANNELSW_NUM_PER_PAGE+2)

#define CHANNELSW_LIST_CELL_H  12
#define CHANNELSW_LIST_CELL_W  (126/CHANNELSW_NUM_COL)
#define CHANNELSW_LIST_X(x)   ((x))
#define CHANNELSW_LIST_Y(y)   (((y)*(CHANNELSW_LIST_CELL_H)))
#define CHANNELSW_LIST_YY(y)  (CHANNELSW_LIST_Y(y)+2)

static u_int8 channelsw_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static T_UICOM_DRAW_MODE channelsw_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(THIS_MENU_UI_CONTAINER, channelsw_menu_cell_zone_int, channelsw_menu_cell_data_int);

static u_int8 channelsw_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  row = 0, col;
    
    zone->border.b = 1;
    zone->border.r = 1;
    zone->border.l = 1;
    zone->border.t = 1;
    
    if (pos < CHANNELSW_NUM_PER_PAGE)
    {
        col = (pos / CHNNELSW_NUM_LINE);
        row = (pos % CHNNELSW_NUM_LINE);
        SCREEN_ZONE_INIT(&zone->zone, CHANNELSW_LIST_X((col*CHANNELSW_LIST_CELL_W)),
            CHANNELSW_LIST_Y(row),  CHANNELSW_LIST_CELL_W, CHANNELSW_LIST_CELL_H);
    }
    else if (IDX_CHANNELSW_GOTO == pos)
    {
        SCREEN_ZONE_INIT(&zone->zone, CHANNELSW_LIST_X(1),CHANNELSW_LIST_YY(CHNNELSW_NUM_LINE),71,12);
    }
    else if (IDX_CHANNELSW_SEL_ALL == pos)
    {
        SCREEN_ZONE_INIT(&zone->zone, CHANNELSW_LIST_X(74),CHANNELSW_LIST_YY(CHNNELSW_NUM_LINE),25,12);
    }
    else if (IDX_CHANNELSW_OK == pos)
    {
        SCREEN_ZONE_INIT(&zone->zone, CHANNELSW_LIST_X(101),CHANNELSW_LIST_YY(CHNNELSW_NUM_LINE), 25,12);
    }

    if (pos < CHANNELSW_NUM_PER_PAGE)
    {
        return 3;//通道X
    }
    else if (pos == IDX_CHANNELSW_GOTO)
    {
        return 2;
    }
    else 
    {
        return 1;//确认//取消
    }
}

#define CHANNELSW_LIST_BOX_W  36
#define CHANNELSW_LIST_BOX_H  12

static T_UICOM_DRAW_MODE channelsw_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8 colPosIdx = 0;
    u_int8 cuSelCh = 0;
    T_UICOM_EVENT type = (*p_type);
    struct SCREEN_ZONE colPosTable[4] = {
        {2,   1,  16, CHANNELSW_LIST_BOX_H},
        {26,  1,  50, CHANNELSW_LIST_BOX_H},
        {50,  1,  9,  10},
        {40,  1,  48, CHANNELSW_LIST_BOX_H},
    };

    if ((childIdx == 1) && (IDX_CHANNELSW_GOTO == pos))
    {
        colPosIdx = 3;
    }
    else
    {
        colPosIdx = childIdx;
    }
    //MY_DEBUG("POS:%d_%d/%d\n", pos, childIdx, colPosIdx);

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);

    if (pos < CHANNELSW_NUM_PER_PAGE)//通道
    {
    cuSelCh =  gMenuChSwitchDataCtrl.curBaseCh + pos;
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_TONGDAO);
                break;
            case 1:
                sprintf(UICOM_DATA_BUF(item), "%03d", cuSelCh);
                break;
            case 2:
                if (cuSelCh) cuSelCh--;
                //xprintf("focus =%d, %d\n", cuSelCh, TST_AD_SAMP_SEL_TST(cuSelCh));
                if (TST_AD_SAMP_SEL_TST(cuSelCh))
                {
                    UICOM_DATA_FILL(item, UICOM_STR_YUANQUAN);
                }
                else
                {
                    UICOM_DATA_FILL(item, NULL);
                }
                break;
            default:
                break;
        }

    }
    else if (IDX_CHANNELSW_OK == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                break;
            default:
                break;
        }
    }
    else if (IDX_CHANNELSW_SEL_ALL == pos)
    {
        switch (childIdx)
        {
            case 0:
                if (CHSWITCH_ITEM_TST_STATUS(pos))
                {
                    UICOM_DATA_FILL(item, UICOM_STR_QUANBUXUAN);
                }
                else
                {
                    UICOM_DATA_FILL(item, UICOM_STR_QUANXUAN);
                }
                break;
            default:
                break;
        }
    }
    
    else if (IDX_CHANNELSW_GOTO == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_TIAOZHUANDAO);
                break;
            case 1:
                if (UICOM_TST_SLECTED(type))
                {
                    colPosIdx = gmenu_input_getstr(UICOM_DATA_BUF(item));
                    UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(colPosIdx));
                }
                else
                {
                    gmenu_input_makestr(GMENU_INPUT_TYPE_VALUE, UICOM_DATA_BUF(item), (p_void)gMenuChSwitchDataCtrl.curBaseCh);
                }
                break;
            default:
                break;
        }
    }
    
    return DRAW_MODE_TEXT_ONLY;

}

static void channelsw_menu_paint(u_int8 isClear, u_int8 focus)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&THIS_MENU_UI_CONTAINER,  0);
    }
    gmenu_content_list_draw(&THIS_MENU_UI_CONTAINER, CHANNELSW_LIST_NUM, focus);
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
    gMenuChSwitchDataCtrl.curBaseCh = MIN_CH_NUM;
    gMenuChSwitchDataCtrl.curSelCh = MIN_CH_NUM;
}

static void chnelsw_menu_tips(void)
{
    #if 0
    struct SCREEN_ZONE rect;
    
    SCREEN_ZONE_INIT(&rect, 2, 1, 128, 12);
    Screen_PrintString(&rect, UICOM_STR_TONGDAOXUANZETISHI, TEXT_STYLE_DEFAULT);

    SCREEN_ZONE_INIT(&rect, 70, 1, 128, 12);
    Screen_PrintString(&rect, UICOM_STR_TONGDAOXUANZETISHI1, TEXT_STYLE_DEFAULT);
    #endif
}

u_int16 channelsw_goto_apply(u_int8 checktype, p_void inputdata, u_int8 len, p_void param)
{
    u_int16 gotnum;
    
    if (INPUT_IF_DEFAULT(checktype))
    {
        gotnum = (u_int16)inputdata;
        if ((gotnum >= MIN_CH_NUM) && (gotnum <= MAX_CH_NUM))
        {
            gMenuChSwitchDataCtrl.curSelCh = gotnum;
            gMenuChSwitchDataCtrl.curBaseCh = gMenuChSwitchDataCtrl.curSelCh;
            if (gMenuChSwitchDataCtrl.curBaseCh > (MAX_CH_NUM - CHANNELSW_NUM_PER_PAGE))
            {
                gMenuChSwitchDataCtrl.curBaseCh = (MAX_CH_NUM - CHANNELSW_NUM_PER_PAGE + 1);
            }
        }
    }
    //MY_DEBUG("GOTO:%d/%d\n", gMenuChSwitchDataCtrl.curStartCh, inputdata);
    return INPUT_STATUS_DEFAULT_ECHO(checktype);
}

void_t channelsw_update_curfocus(void_t)
{
    gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
}


void_t channelsw_open_inputbox(u_int16 val)
{
    struct gmenu_input_config  inputcfg = {0};
    struct gmenu_input_param   initparam = {0};

    
    initparam.initdata    = (p_void)val;
    initparam.isfocustail = TRUE;
    
    inputcfg.type = GMENU_INPUT_TYPE_VALUE;
    inputcfg.limitlen = 3;
    inputcfg.inputdetector = channelsw_goto_apply;
    inputcfg.checktype = INPUT_CHECK_DEFAULT;
    gmenu_input_open(&inputcfg, &initparam, 0);
    gmenu_content_list_changefocus(&THIS_MENU_UI_CONTAINER, IDX_CHANNELSW_GOTO);
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    u_int8 curfocus;
    u_int8 tmp;

    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);
    
    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        chnelsw_menu_tips();
        channelsw_menu_paint(0, 0);
        return UI_PROC_RET_FINISH;
    }
    
    curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
    if ((curfocus != IDX_CHANNELSW_GOTO) && MSG_IS_CHAR(e->sig) && KEY_EVENT_IS_DIGIT(e->param))
    {
        if (!gmenu_input_getstatus())
        {
            channelsw_open_inputbox(KEY_EVENT_DIGIT(e->param));
            return UI_PROC_RET_FINISH;
        }
    }
    
    if (curfocus == IDX_CHANNELSW_GOTO)
    {
        if (gmenu_input_default_proc(e))
        {
            channelsw_update_curfocus();
            return UI_PROC_RET_FINISH;
        }
    }
    
    switch (e->sig)
    {
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, (EVENT_KEY_DOWN==e->sig)?1:-1, 1);
        curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
        gMenuChSwitchDataCtrl.curSelCh = gMenuChSwitchDataCtrl.curBaseCh + curfocus;
        if (curfocus == IDX_CHANNELSW_GOTO)
        {
            channelsw_open_inputbox(gMenuChSwitchDataCtrl.curSelCh);  
        }
        break;
        case EVENT_KEY_LEFT:
    
        break;
        case EVENT_KEY_RIGHT:
    
        break;
        case EVENT_KEY_OK:
        if (curfocus == IDX_CHANNELSW_SEL_ALL)
        {
            CHSWITCH_ITEM_CPL_STATUS(curfocus);
            if (CHSWITCH_ITEM_TST_STATUS(IDX_CHANNELSW_SEL_ALL))
            {
                TST_AD_SAMP_SEL_ALL_SET();
            }
            else
            {
                TST_AD_SAMP_SEL_ALL_CLR();
            }
            channelsw_menu_paint(1, curfocus);
        }
        else if (curfocus == IDX_CHANNELSW_GOTO)
        {
            channelsw_menu_paint(1, gMenuChSwitchDataCtrl.curSelCh - gMenuChSwitchDataCtrl.curBaseCh);
        }
        else if (curfocus == IDX_CHANNELSW_OK)
        {
            ui_mmi_return(1);
        }
        else
        {
            tmp =  gMenuChSwitchDataCtrl.curSelCh - 1; 
           // xprintf("cur focus =%d\n", tmp);
           //util_hex_dump("AA:", (gAdSampConfig.chsel),  sizeof (gAdSampConfig.chsel));
           TST_AD_SAMP_SEL_CPL(tmp);//
          // util_hex_dump("BB:", (gAdSampConfig.chsel),  sizeof (gAdSampConfig.chsel));
            channelsw_update_curfocus();
            curfocus = TST_AD_SAMP_SEL_CHK();
            //xprintf("Focus NUM:%d\n", curfocus);
            if (0 == curfocus)
            {
                CHSWITCH_ITEM_CLR_STATUS(IDX_CHANNELSW_SEL_ALL);
            }
            else if(MAX_CH_NUM == curfocus)
            {
                CHSWITCH_ITEM_SET_STATUS(IDX_CHANNELSW_SEL_ALL);
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




