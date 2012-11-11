#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#include "global.h"

#define THIS_MENU_NAME   "ÉèÖÃÑ¡Ïî"

#define THIS_MENU_UI_CONTAINER   (gSetupOptMenuUi)
#define THIS_MENU_SM_HANDLE      (gMenuSetupOption)

#define IDX_SETUPOPT_WORK_MODE   (0)
#define IDX_SETUPOPT_PARAM_SET   (1)
#define IDX_SETUPOPT_CH_SWITCH   (2)
#define IDX_SETUPOPT_SW_MODE          (3)


#define SETUPOPTION_LIST_NUM  (4)

#define SETUPOPTION_LIST_CELL_H  (18)
#define SETUPOPTION_LIST_X(x)    ((x)+2)
#define SETUPOPTION_LIST_Y(y)    (((y)*(SETUPOPTION_LIST_CELL_H+6))+8)
#define SETUPOPTION_LIST_YY(y)   (SETUPOPTION_LIST_Y(y)+1)

struct SETUP_MENU_CTRL {
    bool_t switch_mode;
};

static struct SETUP_MENU_CTRL   gSetupMenuCtrl = {0};
static u_int8 setupoptmenu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static T_UICOM_DRAW_MODE setupoptmenu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(THIS_MENU_UI_CONTAINER, setupoptmenu_cell_zone_int, setupoptmenu_cell_data_int);

static u_int8 setupoptmenu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    struct SCREEN_ZONE cellPosTable[SETUPOPTION_LIST_NUM] = {
        {SETUPOPTION_LIST_X(0), SETUPOPTION_LIST_Y(0), 60, SETUPOPTION_LIST_CELL_H},
        {SETUPOPTION_LIST_X(0), SETUPOPTION_LIST_Y(1), 60, SETUPOPTION_LIST_CELL_H},
        {SETUPOPTION_LIST_X(63),SETUPOPTION_LIST_Y(0), 60, SETUPOPTION_LIST_CELL_H},
        {SETUPOPTION_LIST_X(63),SETUPOPTION_LIST_Y(1), 60, SETUPOPTION_LIST_CELL_H},
    };
    
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    return 1;
}


#define SETUPOPTION_LIST_BOX_W  36
#define SETUPOPTION_LIST_BOX_H  10

static T_UICOM_DRAW_MODE setupoptmenu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8 colPosIdx = 0;
    T_UICOM_EVENT type = (*p_type);
    struct SCREEN_ZONE colPosTable[1] = {
        {1,   2,  SETUPOPTION_LIST_BOX_W,SETUPOPTION_LIST_BOX_H},
    };
    
    SCREEN_BORDER_INIT(&zone->border, 1, 2, 1, 1);

    colPosIdx = 0;

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);

    if (IDX_SETUPOPT_WORK_MODE == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_GONGZUIMOSHI);
                break;
            default:
                break;
        }

    }
    else if (IDX_SETUPOPT_PARAM_SET == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_CANSHUSHIZHI);
                break;
            default:
                break;
        }

    }
    else if (IDX_SETUPOPT_CH_SWITCH == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_TONGDAOQIEHUAN);
                break;
            default:
                break;
        }
    }
    else if (IDX_SETUPOPT_SW_MODE == pos)
    {
        switch (childIdx)
        {
            case 0:
                if (gSetupMenuCtrl.switch_mode)
                {
                UICOM_DATA_FILL(item, UICOM_STR_SHOUDONG);
                }
                else
                {
                UICOM_DATA_FILL(item, UICOM_STR_ZIDONG);
                }
                zone->zone.x += 2;
                zone->border.t = 1;
                break;
            default:
                break;
        }
    }
    
    return DRAW_MODE_TEXT_ONLY;

}

static void setupoption_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&THIS_MENU_UI_CONTAINER, 0);
    }
    gmenu_content_list_draw(&THIS_MENU_UI_CONTAINER, SETUPOPTION_LIST_NUM, 0);
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
    gSetupMenuCtrl.switch_mode = 0;
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    u_int8 curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);

    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);
    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        setupoption_menu_paint(0);
        return UI_PROC_RET_FINISH;
    }

    switch (e->sig)
    {
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        case EVENT_KEY_RIGHT:
        case EVENT_KEY_LEFT:
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, (LIST_EVENT_IS_NEXT(e->sig))?1:-1, 1);
        break;
        case EVENT_KEY_OK:
        gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
        if (curfocus == IDX_SETUPOPT_PARAM_SET)
        {
            ui_mmi_enter(UI_NODE_PARAMSETUP, 0);
        }
        else if (curfocus == IDX_SETUPOPT_WORK_MODE)
        {
            ui_mmi_enter(UI_NODE_CHECKSEUP, 0);
        }
        else if (curfocus == IDX_SETUPOPT_SW_MODE)
        {
            gSetupMenuCtrl.switch_mode =  !gSetupMenuCtrl.switch_mode;
            SET_SYS_MANUAL();
            syncResAndStatusShow();
        }
        else if (curfocus == IDX_SETUPOPT_CH_SWITCH)
        {
            ui_mmi_enter(UI_NODE_CHSWITCH, 0);
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
}





