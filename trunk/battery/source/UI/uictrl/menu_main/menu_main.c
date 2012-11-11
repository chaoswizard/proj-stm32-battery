#include "uimmi_ctrl.h"
#include "gui_menu_item.h"

#define DISPLAY_END_LINE                100   //显示终止行+1
#define DISPLAY_BASE_LINE               0  //显示基行
#define DISPLAY_BASE_INIT_LINE   DISPLAY_BASE_LINE //初始化显示基行，目前设置为与DISPLAY_BASE_LINE
#define DISPLAY_BASE_LINE_STEP   4//步长

#define THIS_MENU_NAME  "主工作界面"
//"MainFace"
#define THIS_MENU_SM_HANDLE      (gMenuMainSm)
#define THIS_MENU_UI_CONTAINER   (gMainMenuTable)

#define  MAINMENU_TAB_ROW_NUM   5
#define  MAINMENU_TAB_COL_NUM   3

#define MAINMENU_TAB_ORIGN_X   (1)
#define MAINMENU_TAB_ORIGN_Y   (1)
#define MAINMENU_TAB_W         126


#define MAINMENU_TAB_ROW_H          12
#define MAINMENU_TAB_ROW0_H         13
#define MAINMENU_TAB_COL0_W         26
#define MAINMENU_TAB_COL1_W         48
#define MAINMENU_TAB_COL2_W         50
#define MAINMENU_TAB_COL3_W         32

#define MENU_MAIN_REFRESH_MSG    (0x85)

#define   REFRESH_TMR_DELAY    (4000)

struct MAIN_MENU_CTRL {
   SWTMR_NODE_HANDLE  refreshTmr;
    bool_t ui_init;
    bool_t page_rst;
    u_int8 curChBase;
    u_int32 count;
};

static  struct MAIN_MENU_CTRL gMainInterfaceCtrl;

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(THIS_MENU_SM_HANDLE,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);

static T_UICOM_DRAW_MODE mainmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_EVENT *p_type);
static void mainmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_COUNT row, T_UICOM_COUNT col);


LDEF_MENU_CONTENT_TAB(THIS_MENU_UI_CONTAINER, mainmenu_cell_zone_init, mainmenu_cell_data_init);
  
static void menu_main_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_tab_clear_row(&THIS_MENU_UI_CONTAINER, 1, MAINMENU_TAB_ROW_NUM-1, 0);
    }
    gmenu_content_tab_draw(&THIS_MENU_UI_CONTAINER, MAINMENU_TAB_ROW_NUM, MAINMENU_TAB_COL_NUM, 0, 0);
}


static void mainmenu_cell_zone_init(struct OSD_ZONE *zone, T_UICOM_COUNT row, T_UICOM_COUNT col)
{
    T_SCREEN_PIXEL cellWidth;
  
    zone->border.l = 1;
    zone->border.t = 1;
    zone->border.b = 1;
    zone->border.r = 1;
    

    switch (col)
    {
        case 0:// 序号
            cellWidth    = MAINMENU_TAB_COL0_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X);
            break;
        case 1://当前值
            cellWidth = MAINMENU_TAB_COL1_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X + MAINMENU_TAB_COL0_W);
            break;
        case 2://工作状态 
            cellWidth = MAINMENU_TAB_COL2_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X + MAINMENU_TAB_COL0_W + MAINMENU_TAB_COL1_W);
            break;
            #if 0
        case 3://通道切换
            cellWidth = MAINMENU_TAB_COL3_W;
            zone->zone.x = (MAINMENU_TAB_ORIGN_X + MAINMENU_TAB_COL0_W + MAINMENU_TAB_COL1_W + MAINMENU_TAB_COL2_W);
            break;
            #endif
        default:
            cellWidth = 1;
            break;
    }

    zone->zone.w = cellWidth;
    if (0 == row)
    {
        zone->zone.h = MAINMENU_TAB_ROW0_H;
        zone->zone.y = (MAINMENU_TAB_ORIGN_Y);
    }
    else
    {
        zone->zone.h = MAINMENU_TAB_ROW_H;
        zone->zone.y = (MAINMENU_TAB_ORIGN_Y + MAINMENU_TAB_ROW0_H + (row - 1)*MAINMENU_TAB_ROW_H);
    }
    
}

static T_UICOM_DRAW_MODE mainmenu_cell_data_init(struct SCREEN_ZONE *zone,PUICOM_DATA item, T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_EVENT *p_type)
{
            u_int16 curCh;
            u_int32  tmp;
            u_int32 curval;
            if (gMainInterfaceCtrl.ui_init && (0 == row))
            {
                return DRAW_MODE_SKIP;
            }
            
            zone->x = 2;
            zone->y = 1;

            UICOM_CLR_SLECTED((*p_type));
            UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);
            
            if (0 == row)
            {

                switch (col)
                {
                    case 0:// 序号
                        UICOM_DATA_FILL(item, UICOM_STR_XUHAO);
                        break;
                    case 1://当前值
                        UICOM_DATA_FILL(item, UICOM_STR_DANGQIANZHI);
                        break;
                    case 2://工作状态 
                        UICOM_DATA_FILL(item, UICOM_STR_GONGZUOZHUANGTAI);
                        break;
                    case 3://通道切换
                        UICOM_DATA_FILL(item, UICOM_STR_TONGDAOQIEHUAN);
                        break;
                    default:
                        break;
                }
                return DRAW_MODE_TEXT_BOX; 
            }

            if (gMainInterfaceCtrl.page_rst )
            {
                  return DRAW_MODE_SKIP;
            }
            
           curCh = row + gMainInterfaceCtrl.curChBase - 1;

         //===========非首行============
            if (0 == col)
            {
                    sprintf(UICOM_DATA_BUF(item), "%03d",   curCh+1);
            }
            else if (1 == col)
            {
                     curval = batteryVolAdArray[curCh] *5000/65535;///(batteryVolAdArray[curCh] * 10000) >>18;//单位mv.... batteryVolAdArray[curCh] ;
                     tmp = curval/1000;                        //转为V为单位时的整数部分
                    sprintf(UICOM_DATA_BUF(item), "%d.%0.3d", tmp,  curval- (1000*tmp));
            }
            else if (2 == col)
            {
                    if  (TST_CH_WORK_STATE_TST(curCh))  //>0,放电中
                    {
                        UICOM_DATA_FILL(item, UICOM_STR_FANGDIANZHONG);
                    }
                    else
                    {
                        UICOM_SET_SLECTED((*p_type));
                        UICOM_DATA_FILL(item, UICOM_STR_FANGDIANJIESHU);
                    }
                    
                    if (curCh >=( DISPLAY_END_LINE-1)) 
                    {
                        gMainInterfaceCtrl.page_rst = 1;
                    }
            }

            
            return DRAW_MODE_TEXT_BOX; 
}


static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_SUSPEND(THIS_MENU_NAME, me, child);
    gMainInterfaceCtrl.ui_init = 0;
}

static void menu_pub_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_RESUME(THIS_MENU_NAME, child, me);
}

static void menu_main_refreshTmr(struct MAIN_MENU_CTRL *hdl)
{
        struct EVENT_NODE_ITEM e;
        
        e.sig = EVENT_USR_MMI;
        USR_EVENT_MMI_INIT(e.param, MENU_MAIN_REFRESH_MSG);
        ui_mmi_send_msg(&e);
}

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    UIMMI_DEBUGSM_ENTER(THIS_MENU_NAME, parent, me);
    
    ui_mmi_reg_suspend(menu_pub_suspend);
    ui_mmi_reg_resume(menu_pub_resume);
    
   gMainInterfaceCtrl.refreshTmr =  ui_mmi_start_timer(REFRESH_TMR_DELAY,   \
                       menu_main_refreshTmr,  (p_void)&gMainInterfaceCtrl, TRUE);
                       
    gMainInterfaceCtrl.ui_init = 0;
    gMainInterfaceCtrl.curChBase = DISPLAY_BASE_INIT_LINE;
    gMainInterfaceCtrl.page_rst = 0;
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);
    
    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        menu_main_paint(0);
        gMainInterfaceCtrl.ui_init = 1;
    }
    else
    switch (e->sig)
    {
        case EVENT_KEY_LEFT:
        case EVENT_KEY_RIGHT:
        case EVENT_KEY_UP:
        case EVENT_KEY_DOWN:
            break;
        case EVENT_USR_MMI:
        {
            u_int32 myevt = USR_EVENT_MMI(e->param);

            if (MENU_MAIN_REFRESH_MSG == (myevt))
            {
                    if ( gMainInterfaceCtrl.page_rst)
                    {
                        gMainInterfaceCtrl.curChBase = DISPLAY_BASE_INIT_LINE;
                        gMainInterfaceCtrl.page_rst = 0;
                    }
                    else if(IS_SYS_WORKING())
                    {
                        gMainInterfaceCtrl.curChBase += DISPLAY_BASE_LINE_STEP;//(MAINMENU_TAB_ROW_NUM-1); 
                    }
                    menu_main_paint(1);
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
    
    ui_mmi_stop_timer(gMainInterfaceCtrl.refreshTmr);
    gMainInterfaceCtrl.refreshTmr = SWTMR_INVALID_NODE;
}




