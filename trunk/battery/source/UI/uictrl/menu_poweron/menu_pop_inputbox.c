#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define THIS_MENU_NAME   "弹出输入框"
//"inputbox"




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
static T_UICOM_DRAW_MODE inputbox_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(gPopMenuInputBoxList, inputbox_menu_cell_zone_int, inputbox_menu_cell_data_int);

static u_int8 inputbox_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[INPUTBOX_LIST_NUM] = {
        {INPUTBOX_LIST_X(16),INPUTBOX_LIST_Y(14),    INPUTBOX_POPMENU_W-28,14},
        {INPUTBOX_LIST_X(8),INPUTBOX_LIST_Y(32),  28,13},
        {INPUTBOX_LIST_X((INPUTBOX_POPMENU_W/2) + 8),  INPUTBOX_LIST_Y(32),  28,13},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    
    return 1;
}


static T_UICOM_DRAW_MODE inputbox_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8 focus = 0;
    T_UICOM_EVENT type = (*p_type);


    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);

    if (IDX_INPUTBOX_BUTTON_BOX == pos)// inputbox
    {
        switch (childIdx)
        {
            case 0:
                SCREEN_ZONE_INIT(&zone->zone, 1, 3, 60, 12);
                focus = gmenu_input_getstr(UICOM_DATA_BUF(item));
                if (UICOM_TST_SLECTED(type))
                {
                    UICOM_DATA_TEXT_ATTR_ADD(item, TEXT_ATTR_FOCUS(focus));
                }
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
                SCREEN_ZONE_INIT(&zone->zone, 4, 2, 60, 12);
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
    
    return DRAW_MODE_TEXT_ONLY;

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

    SCREEN_ZONE_INIT(&rect.zone, INPUTBOX_LIST_X(4), INPUTBOX_LIST_Y(2), 32, 14);

    UICOM_DATA_TEXT_ATTR_RST(&content, TEXT_STYLE_DEFAULT);
    util_strcpy(UICOM_DATA_BUF(&content), "Input Sth:");
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
    if (!gmenu_input_getstatus())
    {
        gmenu_input_open(NULL, NULL, 0);
    }
}

static void popmenu_inputbox_update_focus(void)
{
    gmenu_content_list_movefocus(&gPopMenuInputBoxList, 0, 0);
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    u_int8 curfocus = gmenu_content_list_getfocus(&gPopMenuInputBoxList);
    
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);
    
    if (MSG_IS_ENTRY(e->sig))
    {
        inputbox_menu_pop(1);
        inputbox_menu_paint(0);
    }

    if (curfocus == IDX_INPUTBOX_BUTTON_BOX)
    {
        if (gmenu_input_default_proc(e))
        {
            popmenu_inputbox_update_focus();
            return UI_PROC_RET_FINISH;
        }
        else
        {
            popmenu_inputbox_update_focus();
        }
    }

    switch (e->sig)
    {
        case EVENT_KEY_DOWN:
        gmenu_content_list_movefocus(&gPopMenuInputBoxList, 1, 1);
        break;
        case EVENT_KEY_UP:
        gmenu_content_list_movefocus(&gPopMenuInputBoxList, -1, 1);
        break;
        case EVENT_KEY_OK:
            if ((curfocus == IDX_INPUTBOX_BUTTON_OK) || (curfocus == IDX_INPUTBOX_BUTTON_ESC))
            {
                ui_mmi_return(1);
            }
            break;
        default:
            break;
        
    }

    return UI_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    gmenu_input_close();
    UIMMI_DEBUGSM_EXIT(THIS_MENU_NAME, next, me);
}




bool_t gmenu_input_default_proc(struct EVENT_NODE_ITEM *e)
{
    u_int16 ch;
    u_int16 evt = INPUT_PROC_NULL;

    
    if (MSG_IS_CHAR(e->sig) && KEY_EVENT_IS_DIGIT(e->param))
    {
        ch = KEY_EVENT_CHAR(e->param);
        return gmenu_input_proc(INPUT_PROC_SET(INPUT_PROC_NEW, 0), &ch);
    }
    else if (EVENT_KEY_RIGHT == e->sig)
    {
        return gmenu_input_proc(INPUT_PROC_SET(INPUT_PROC_MOVE, 0), NULL);
    }
    else if (EVENT_KEY_LEFT == e->sig)
    {
        return gmenu_input_proc(INPUT_PROC_SET(INPUT_PROC_MOVE, 1), NULL);
    }
    else if (EVENT_KEY_ESC == e->sig)
    {
        return gmenu_input_proc(INPUT_PROC_SET(INPUT_PROC_DEL, 0), NULL);
    }
    else if (EVENT_KEY_OK == e->sig)
    {
        gmenu_input_proc(INPUT_PROC_SET(INPUT_PROC_NULL, INPUT_STATUS_KEY_ENTER), (p_void)1);
    }
    else if ((EVENT_KEY_UP == e->sig) || (EVENT_KEY_DOWN == e->sig))
    {
        gmenu_input_proc(INPUT_PROC_SET(INPUT_PROC_NULL, INPUT_STATUS_FOCUS_LEAVE), (p_void)1);
    }
    return FALSE;
}


