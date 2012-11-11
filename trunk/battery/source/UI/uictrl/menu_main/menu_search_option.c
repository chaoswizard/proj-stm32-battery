#include "uimmi_ctrl.h"
#include "gui_menu_item.h"
#include "global.h"

#define THIS_MENU_NAME            "查询界面"

#define THIS_MENU_SM_HANDLE      (gMenuSearchInfoSm)
#define THIS_MENU_UI_CONTAINER   (gMenuSearchInfoMenu)


#define SEARCHINFO_LIST_NUM  (3)

#define MENU_SEARCHINFO_REFRESH_MSG    (0x86)

#define   MENU_SEARCHINFO_REFRESH_TMR_DELAY    (1000)

struct MENU_SEARCHINFO_CTRL {
    SWTMR_NODE_HANDLE  refreshTmr;
    //u_int16 timesetup;
    //u_int16 timeused_m;
    //u_int16 timeused_s;
};

static struct MENU_SEARCHINFO_CTRL gMenuSearchInfoCtrl = {0};

#define IDX_SEARCHINFO_WARN           (0)
#define IDX_SEARCHINFO_CURSE          (1)
#define IDX_SEARCHINFO_TIME            (2)


#define SEARCHINFO_LIST_CELL_H  12
#define SEARCHINFO_LIST_X(x)   (x+1)
#define SEARCHINFO_LIST_Y(y)   ((y*(SEARCHINFO_LIST_CELL_H+2)))

static u_int8 searchinfo_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static T_UICOM_DRAW_MODE searchinfo_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(THIS_MENU_UI_CONTAINER, searchinfo_menu_cell_zone_int, searchinfo_menu_cell_data_int);

static u_int8 searchinfo_menu_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    u_int8  count = 0;
    struct SCREEN_ZONE cellPosTable[SEARCHINFO_LIST_NUM] = {
        {SEARCHINFO_LIST_X(0), SEARCHINFO_LIST_Y(0), 60, SEARCHINFO_LIST_CELL_H},
        {SEARCHINFO_LIST_X(65), SEARCHINFO_LIST_Y(0), 60, SEARCHINFO_LIST_CELL_H},
        {SEARCHINFO_LIST_X(0), SEARCHINFO_LIST_Y(1), 125, 49},
    };
    
    zone->border.l = 1;
    //if (IDX_SEARCHINFO_TIME == pos)
    //{
    //    zone->border.t = 1;
    //    zone->border.b = 0;
    //}
    //else
    {
        zone->border.b = 1;
        zone->border.t = 1;
    }
    zone->border.r = 1;
    
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    switch (pos)
    {
        case IDX_SEARCHINFO_WARN:
        case IDX_SEARCHINFO_CURSE:
            count =  1;
            break;
        case IDX_SEARCHINFO_TIME:
            count =  7;
            break;
        default:
            break;
    }
    
    return count;
}


#define SEARCHINFO_LIST_BOX_W  16
#define SEARCHINFO_LIST_BOX_H  12
static T_UICOM_DRAW_MODE searchinfo_menu_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8 colPosIdx = 0, focus = 0;
    T_UICOM_EVENT type = (*p_type);
    u_int8  drawMode = 0;
    
    struct SCREEN_ZONE colPosTable[3] = {
        {1,   0,  SEARCHINFO_LIST_BOX_W+2,SEARCHINFO_LIST_BOX_H},
        {36,  0,  SEARCHINFO_LIST_BOX_W,SEARCHINFO_LIST_BOX_H},
    };

    SCREEN_BORDER_INIT(&zone->border, 1, 1, 1, 1);
    switch (childIdx)
    {
        case 0:
            colPosIdx = 0;
            break;
        case 2:
            if (IDX_SEARCHINFO_TIME == pos)
            {
                colPosIdx = 0;
            }
            else
            {
                colPosIdx = 1;
            }
            break;
        case 1:
        case 3:
            colPosIdx = 1;
            break;
        default:
            break;
    }

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);

    if (IDX_SEARCHINFO_WARN == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_CHAXUNJINGGAO);
                break;
            default:
                break;
        }

    }
    else if (IDX_SEARCHINFO_CURSE == pos)
    {
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_CHAXUNQUXIAN);
                break;
            default:
                break;
        }
    }
    else if (IDX_SEARCHINFO_TIME == pos)
    {
        UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_LARGE_SOLID);
        UICOM_CLR_SLECTED((*p_type));
        switch (childIdx)
        {
            case 0:
                UICOM_DATA_FILL(item, UICOM_STR_SHEZHISHIJIAN);
                break;
            case 1:
                if (UICOM_TST_SLECTED(type))
                {
                        drawMode = DRAW_MODE_CLEAR;
                }
                sprintf(UICOM_DATA_BUF(item), "%02d 分", gAdSampConfig.mode_min);
                zone->zone.x   += 38;
                break;
            case 2:
                UICOM_DATA_FILL(item, UICOM_STR_YIYONGSHI);
                zone->zone.y   += 12;
                break;
            case 3:
                if (UICOM_TST_SLECTED(type))
                {
                        drawMode = DRAW_MODE_CLEAR;
                }
                zone->zone.x   += 4;
                zone->zone.y   += 12;
                sprintf(UICOM_DATA_BUF(item), "%02d 分 %02d 秒", 
                        g_bkpData.had_used_time/60,
                        g_bkpData.had_used_time%60);
                break;
            case 4:
                zone->zone.y   += 24;
                UICOM_DATA_FILL(item, UICOM_STR_XITONGSHIJIAN);
                break;
            case 5:
                if (UICOM_TST_SLECTED(type))
                {
                        drawMode = DRAW_MODE_CLEAR;
                }
                zone->zone.x   += 48;
                zone->zone.y   += 24;
                sprintf(UICOM_DATA_BUF(item), "%02d时%02d分%02d秒", 
                    systmtime.tm_hour, 
                    systmtime.tm_min,
                    systmtime.tm_sec);
                break;
            case 6:
                if (UICOM_TST_SLECTED(type))
                {
                        drawMode = DRAW_MODE_CLEAR;
                }
                zone->zone.x   += 36;
                zone->zone.y   += 36;
                sprintf(UICOM_DATA_BUF(item), "%04d年%02d月%02d日", 
                    systmtime.tm_year, 
                    systmtime.tm_mon,
                    systmtime.tm_mday);
                break;
                
            default:
                break;
        }
    }

    return DRAW_MODE_TEXT_ONLY | drawMode;

}

static void searchinfo_menu_paint(u_int8 curfocus, u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&THIS_MENU_UI_CONTAINER, 0);
    }
    gmenu_content_list_draw(&THIS_MENU_UI_CONTAINER, SEARCHINFO_LIST_NUM, curfocus);
}



static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(THIS_MENU_SM_HANDLE,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);




static void menu_searchinfo_refreshTmr(struct MENU_SEARCHINFO_CTRL *hdl)
{
        struct EVENT_NODE_ITEM e;

        //xprintf("TmrRefr\n");
        e.sig = EVENT_USR_MMI;
        USR_EVENT_MMI_INIT(e.param, MENU_SEARCHINFO_REFRESH_MSG);
        ui_mmi_send_msg(&e);
}

static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_SUSPEND(THIS_MENU_NAME, me, child);

     ui_mmi_stop_timer(gMenuSearchInfoCtrl.refreshTmr);
     gMenuSearchInfoCtrl.refreshTmr = SWTMR_INVALID_NODE;
}

static void menu_pub_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_RESUME(THIS_MENU_NAME, child, me);
     ui_mmi_stop_timer(gMenuSearchInfoCtrl.refreshTmr);
    gMenuSearchInfoCtrl.refreshTmr = ui_mmi_start_timer(MENU_SEARCHINFO_REFRESH_TMR_DELAY,   \
                       menu_searchinfo_refreshTmr,  (p_void)&gMenuSearchInfoCtrl, TRUE);
}
static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    UIMMI_DEBUGSM_ENTER(THIS_MENU_NAME, parent, me);
    ui_mmi_reg_suspend(menu_pub_suspend);
    ui_mmi_reg_resume(menu_pub_resume);
    
    gMenuSearchInfoCtrl.refreshTmr = ui_mmi_start_timer(MENU_SEARCHINFO_REFRESH_TMR_DELAY,   \
                       menu_searchinfo_refreshTmr,  (p_void)&gMenuSearchInfoCtrl, TRUE);
                       
}

static void searchinfo_menu_update_focus(void)
{
    gmenu_content_list_movefocus(&THIS_MENU_UI_CONTAINER, 0, 0);
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    u_int8 curfocus;
    
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);

    if (MSG_IS_ENTRY(e->sig))
    {
        systime_update(RTC_GetCounter());
        Screen_PrintClear(NULL);
        searchinfo_menu_paint(0, FALSE);
        return UI_PROC_RET_FINISH;
    }
    
    curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
    switch (e->sig)
    {
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        case EVENT_KEY_RIGHT:
        case EVENT_KEY_LEFT:
        curfocus = gmenu_content_list_getfocus(&THIS_MENU_UI_CONTAINER);
        if (curfocus == IDX_SEARCHINFO_WARN)
        {             
                gmenu_content_list_changefocus(&THIS_MENU_UI_CONTAINER,  IDX_SEARCHINFO_CURSE);
        }
        else 
        {
                gmenu_content_list_changefocus(&THIS_MENU_UI_CONTAINER,  IDX_SEARCHINFO_WARN);
        }
        break;
        case EVENT_KEY_OK:
        searchinfo_menu_update_focus();
        if (curfocus == IDX_SEARCHINFO_WARN)
        {
            ui_mmi_enter(UI_NODE_WARNINFO, 0);
        }
        else if (curfocus == IDX_SEARCHINFO_CURSE)
        {
            ui_mmi_enter(UI_NODE_SEARCHOPT, 0);
        }
        else
        {
            ui_mmi_return(1);
        }
        return UI_PROC_RET_FINISH;
        break;
        case EVENT_USR_MMI:
        {
            u_int32 myevt = USR_EVENT_MMI(e->param);

             if (MENU_SEARCHINFO_REFRESH_MSG == (myevt))
            {
                systime_update(RTC_GetCounter());
                gmenu_content_list_update(&THIS_MENU_UI_CONTAINER,  IDX_SEARCHINFO_TIME,  UICOM_STATUS_SELECTED);
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
    ui_mmi_stop_timer(gMenuSearchInfoCtrl.refreshTmr);
    gMenuSearchInfoCtrl.refreshTmr = SWTMR_INVALID_NODE;
    gmenu_input_close();
}









