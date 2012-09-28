#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "弹出输入框"
//"inputbox"

#define INPUTBOX_MAX_CHAR_LEN  (10)

struct GMENU_POP_INPUTBOX_CTRL {
    struct gmenu_pop_input_param param;
    u_int8 curfocus;
    u_int8 data[INPUTBOX_MAX_CHAR_LEN];
};
static struct GMENU_POP_INPUTBOX_CTRL  gPopInputboxCtrl = {0};


#define INPUTBOX_POPMENU_W       88
#define INPUTBOX_POPMENU_H       48

#define INPUTBOX_POPMENU_X       ((128 - INPUTBOX_POPMENU_W)/2)
#define INPUTBOX_POPMENU_Y       ((64 - INPUTBOX_POPMENU_H)/2)

#define INPUTBOX_LIST_NUM            (3)

#define INPUTBOX_LIST_X(x)    ((x) + (INPUTBOX_POPMENU_X))
#define INPUTBOX_LIST_Y(y)    ((y) + (INPUTBOX_POPMENU_Y))
//==========================================================
#define IDX_INPUTBOX_BUTTON_BOX     (0)
#define IDX_INPUTBOX_BUTTON_OK      (1)
#define IDX_INPUTBOX_BUTTON_ESC     (2)
//==========================================================


static u_int8 inputbox_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static u_int8 inputbox_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type);

LDEF_MENU_CONTENT_LIST(gPopMenuInputBoxList, inputbox_menu_cell_zone_int, inputbox_menu_cell_data_int);

static u_int8 inputbox_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[INPUTBOX_LIST_NUM] = {
        {INPUTBOX_LIST_X(16),INPUTBOX_LIST_Y(14),    INPUTBOX_POPMENU_W-28,12},
        {INPUTBOX_LIST_X(8),INPUTBOX_LIST_Y(32),  28,10},
        {INPUTBOX_LIST_X((INPUTBOX_POPMENU_W/2) + 8),  INPUTBOX_LIST_Y(32),  28,10},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    
    return 1;
}


static u_int8 inputbox_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, enum T_UICOM_STATUS type)
{
    u_int8 colPosIdx = 0, status  = DRAW_MODE_TEXT_ONLY;


    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_SMALL_BLACK);

    if (IDX_INPUTBOX_BUTTON_BOX == pos)// inputbox
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 1, 3, 60, 12);
                if (UICOM_IS_FOCUS(type))
                {
                    UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(gPopInputboxCtrl.curfocus));
                }
                sprintf(UICOM_DATA_BUF(item), "%s", gPopInputboxCtrl.data);
                break;
            default:
                break;
        }

    }

    else if (IDX_INPUTBOX_BUTTON_OK == pos)//确认
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 6, 2, 60, 12);
                UICOM_DATA_FILL(item, UICOM_STR_QUEREN);
                break;
            default:
                break;
        }
    }
    else if (IDX_INPUTBOX_BUTTON_ESC == pos)//取消
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 4, 2, 60, 12);
                UICOM_DATA_FILL(item, UICOM_STR_QUXIAO);
                break;
            default:
                break;
        }
    }
    
    return status;

}

static void inputbox_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&gPopMenuInputBoxList, 0);
    }
    gmenu_content_list_draw(&gPopMenuInputBoxList, INPUTBOX_LIST_NUM, 0);

}

static void inputbox_menu_pop(u_int8 isClearBg)
{
    struct OSD_ZONE rect;
    struct UICOM_DATA content, str[64];

    content.data = str;
    SCREEN_ZONE_INIT(&rect.zone, INPUTBOX_POPMENU_X, INPUTBOX_POPMENU_Y, INPUTBOX_POPMENU_W, INPUTBOX_POPMENU_H);
    if (isClearBg)
    {
        Screen_PrintClear(&rect.zone);
    }
    
    SCREEN_BORDER_INIT(&rect.border, 1, 1, 1, 1);
    Screen_PrintRect(&rect, PIXEL_MODE_SET);

    SCREEN_ZONE_INIT(&rect.zone, INPUTBOX_LIST_X(4), INPUTBOX_LIST_Y(4), 32, 14);

    UICOM_DATA_TEXT_ATTR_RST(&content, TEXT_SMALL_BLACK);
    
    if (gPopInputboxCtrl.param.title)
    {
        (*gPopInputboxCtrl.param.title)(&content);
    }
    else
    {
        sprintf(UICOM_DATA_BUF(&content), "Input");
    }
    gui_osd_data_draw(&content, &rect.zone);

}

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gPopMenuInputBoxMenu,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);




static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    ui_mmi_debug_suspend(THIS_MENU_NAME, me, child);
}

static void menu_pub_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    ui_mmi_debug_resume(THIS_MENU_NAME, child, me);
}


static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    ui_mmi_debug_enter(THIS_MENU_NAME, parent, me);
    ui_mmi_reg_suspend(menu_pub_suspend);
    ui_mmi_reg_resume(menu_pub_resume);
    if (0 == gPopInputboxCtrl.param.maxlen)
    {
        memset(&gPopInputboxCtrl, 0, sizeof(gPopInputboxCtrl));
        gPopInputboxCtrl.param.maxlen = INPUTBOX_MAX_CHAR_LEN - 1;
    }
}
static void popmenu_inputbox_update_focus(void)
{
    gmenu_content_list_movefocus(&gPopMenuInputBoxList, 0, 0);
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    u_int8 curfocus = gmenu_content_list_getfocus(&gPopMenuInputBoxList);
    u_int8 ch;
    
    if (MSG_IS_ENTRY(e->sig))
    {
        inputbox_menu_pop(1);
        inputbox_menu_paint(0);
    }
    else if ((curfocus == IDX_INPUTBOX_BUTTON_BOX) && MSG_IS_DIGIT_KEY(e->sig))
    {
        ch = e->param.key.digit.val + '0';
        gPopInputboxCtrl.curfocus = uicom_inputbox_proc(INPUTBOX_PROC_NEWCHAR_RMODE, 
            gPopInputboxCtrl.data, gPopInputboxCtrl.curfocus,  &ch);
        popmenu_inputbox_update_focus();
    }
    else
    switch (e->sig)
    {
        case EVENT_KEY_DOWN:
        gmenu_content_list_movefocus(&gPopMenuInputBoxList, 1, 1);
        gPopInputboxCtrl.curfocus = 0;
        break;
        case EVENT_KEY_UP:
        gmenu_content_list_movefocus(&gPopMenuInputBoxList, -1, 1);
        gPopInputboxCtrl.curfocus = 0;
        break;
        case EVENT_KEY_RIGHT:
            if (curfocus == IDX_INPUTBOX_BUTTON_BOX)
            {
                gPopInputboxCtrl.curfocus = uicom_inputbox_proc(INPUTBOX_PROC_MOVE_BACK, 
                    gPopInputboxCtrl.data, gPopInputboxCtrl.curfocus, NULL);
                popmenu_inputbox_update_focus();
            }
            break;
        case EVENT_KEY_LEFT:
            if (curfocus == IDX_INPUTBOX_BUTTON_BOX)
            {
                gPopInputboxCtrl.curfocus = uicom_inputbox_proc(INPUTBOX_PROC_MOVE_FRONT, 
                    gPopInputboxCtrl.data, gPopInputboxCtrl.curfocus, NULL);
                popmenu_inputbox_update_focus();
            }
            break;
        case EVENT_KEY_OK:
            if (curfocus == IDX_INPUTBOX_BUTTON_OK)
            {
                if (gPopInputboxCtrl.param.inputfinish)
                {
                    if ((*gPopInputboxCtrl.param.inputfinish)(gPopInputboxCtrl.data, util_strlen(gPopInputboxCtrl.data)))
                    {
                        ui_mmi_return(1);
                    }
                }
                else
                {
                    ui_mmi_return(1);
                }
            }
            else if (curfocus == IDX_INPUTBOX_BUTTON_ESC)
            {
                ui_mmi_return(1);
            }
            break;
        case EVENT_KEY_ESC:
            #if 0
            if (curfocus == IDX_INPUTBOX_BUTTON_BOX && '\0' != gPopInputboxCtrl.data[0])
            {
                gPopInputboxCtrl.curfocus = uicom_inputbox_proc(INPUTBOX_PROC_DEL_FRONT, 
                    gPopInputboxCtrl.data, gPopInputboxCtrl.curfocus, NULL);
                popmenu_inputbox_update_focus();
                return SM_PROC_RET_FINISH;
            }
            #endif
            break;
        default:
            break;
        
    }

    return SM_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    memset(&gPopInputboxCtrl, 0, sizeof(gPopInputboxCtrl));
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}


void gmenu_pop_inputbox(struct gmenu_pop_input_param *param, u_int8 *defaulttext)
{
    memset(&gPopInputboxCtrl, 0, sizeof(gPopInputboxCtrl));
    gPopInputboxCtrl.param.maxlen = INPUTBOX_MAX_CHAR_LEN - 1;
    
    if (param)
    {
        gPopInputboxCtrl.param.inputfinish = param->inputfinish;
        gPopInputboxCtrl.param.title = param->title;
        
        if (gPopInputboxCtrl.param.maxlen >= INPUTBOX_MAX_CHAR_LEN)
        {
            gPopInputboxCtrl.param.maxlen = INPUTBOX_MAX_CHAR_LEN - 1;
        }
        strncpy(gPopInputboxCtrl.data, defaulttext, gPopInputboxCtrl.param.maxlen);
    }
    
    ui_mmi_enter(UI_NODE_POP_INPUTBOX, 0);
}





