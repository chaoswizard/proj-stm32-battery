#include "uimmi_ctrl.h"
#include "gui_menu_item.h"


#define THIS_MENU_NAME   "显示曲线"
//"ShowCurve"
#define THIS_MENU_SM_HANDLE      (gMenuShowCurveSm)
#define THIS_MENU_UI_CONTAINER   (gCurveGoupMenu)

#define SHOW_GROUP_CURVE_NUM    4



#define ORIGN_CURVE_X             8
#define ORIGN_CURVE_Y             10
#define SHOW_CURVE_H             43
#define SHOW_CURVE_W             88
//---------------------------------------------------------------------
#define UNIT_MAX_W               SHOW_CURVE_W
#define UNIT_MAX_H               SHOW_CURVE_H
//---------------------------------------------------------------------
#define POS_MIN_X                ORIGN_CURVE_X
#define POS_MAX_Y               (ORIGN_CURVE_Y + UNIT_MAX_H)
#define UNIT_FACTOR_Y(y)        (y)
#define UNIT_FACTOR_X(x)        (x)/*((((x)*SHOW_CURVE_W)/SHOW_CURVE_H))*/
//----------------------------------------------------------------------
#define UNIT_TO_POS_X(pixel)    (POS_MIN_X + (UNIT_FACTOR_X(pixel)))
#define UNIT_TO_POS_Y(pixel)    (POS_MAX_Y - (UNIT_FACTOR_Y(pixel)))
#define UNIT_TO_VALUE_X(pixel)  ((pixel)*1)
#define VALUE_TO_UNIT_Y(val)    ((val)/UNIT_FACTOR_Y(1))
//---------------------------------------------------------------------
#define SHOW_CURVE_Y_RULE        UNIT_FACTOR_Y(10)
#define SHOW_CURVE_X_RULE        UNIT_FACTOR_X(10)
//---------------------------------------------------------------------

#define SHOW_GROUP_CURVE_X       99
#define SHOW_CURVE_X_NAME_X      (POS_MIN_X + UNIT_MAX_W + 4)
#define SHOW_CURVE_X_NAME_Y      (POS_MAX_Y - 4)

#define SHOW_CURVE_Y_NAME_X      1
#define SHOW_CURVE_Y_NAME_Y      1


struct tagCurvePosHistory {
    u_int8 yPos[SHOW_GROUP_CURVE_NUM][UNIT_MAX_W];
    u_int8 validLen;
};

static struct tagCurvePosHistory gCurvePosData;


static struct {
    u_int16 bitmapItemStatus;
    u_int32 pageCount;
    struct gmenu_curve_config cfg;
}gShowCurveCtrl = {0};


#define CURVEGRP_ITEM_SET_STATUS(idx)  (gShowCurveCtrl.bitmapItemStatus = UTIL_BIT_SET(gShowCurveCtrl.bitmapItemStatus, (idx)))
#define CURVEGRP_ITEM_CLR_STATUS(idx)  (gShowCurveCtrl.bitmapItemStatus = UTIL_BIT_CLR(gShowCurveCtrl.bitmapItemStatus, (idx)))
#define CURVEGRP_ITEM_CPL_STATUS(idx)  (gShowCurveCtrl.bitmapItemStatus = UTIL_BIT_CPL(gShowCurveCtrl.bitmapItemStatus, (idx)))
#define CURVEGRP_ITEM_TST_STATUS(idx)  UTIL_BIT_TST(gShowCurveCtrl.bitmapItemStatus, (idx))
#define CURVEGRP_ITEM_SET_STATUS_RANGE(idxStart, idxEnd)  (gShowCurveCtrl.bitmapItemStatus = UTIL_BIT_SET_RANGE(gShowCurveCtrl.bitmapItemStatus, idxStart, idxEnd))
#define CURVEGRP_ITEM_CLR_STATUS_RANGE(idxStart, idxEnd)  (gShowCurveCtrl.bitmapItemStatus = UTIL_BIT_CLR_RANGE(gShowCurveCtrl.bitmapItemStatus, idxStart, idxEnd))
#define CURVEGRP_ITEM_TST_STATUS_RANGE(idxStart, idxEnd)   UTIL_BIT_TST_RANGE(gShowCurveCtrl.bitmapItemStatus, idxStart, idxEnd)


static void curvegroup_menu_paint(u_int8 isClear);
static void curvemap_paint(u_int8 isRst);
static void curve_load_data(void);

void gmenu_show_curve(struct gmenu_curve_config *param, bool_t pop)
{
    memset(&gCurvePosData, 0, sizeof(gCurvePosData));
    memset(&gShowCurveCtrl, 0, sizeof(gShowCurveCtrl));
    if (param)
    {
        memcpy(&gShowCurveCtrl.cfg,  param, sizeof(struct gmenu_curve_config));
    }

    if (pop)
    {
        ui_mmi_enter(UI_NODE_SHOWCURVE, 0);
    }
    else
    {
        curvemap_paint(TRUE);
        curvegroup_menu_paint(TRUE);
    }
}


static T_UICOM_DRAW_MODE  curve_menu_datainit(T_UICOM_COUNT group, T_UICOM_COUNT *key,T_UICOM_COUNT *y);

LDEF_MENU_VALUE_MAP(gMapShowCurveMap, curve_menu_datainit);

static u_int8  curve_menu_test_map(T_UICOM_COUNT group, u_int32 x, u_int8 maxY, u_int8 rulerY)
{
    return ((group+ x)*(x))%maxY;
}


static void curve_load_data(void)
{
    u_int8 grp;
    u_int8 val;
    u_int32 key;

    if (gCurvePosData.validLen >= UNIT_MAX_W)
    {
        gCurvePosData.validLen = 0;
        gShowCurveCtrl.pageCount++;
    }

    key = gShowCurveCtrl.pageCount*UNIT_MAX_W + gCurvePosData.validLen;
    for (grp=0;grp<(gShowCurveCtrl.cfg.initCount);grp++)
    {
        if (gShowCurveCtrl.cfg.fun)
        {
            val = gShowCurveCtrl.cfg.fun(grp, key, UNIT_MAX_H, SHOW_CURVE_Y_RULE);
        }
        else 
        {
            val = curve_menu_test_map(grp, key, UNIT_MAX_H, SHOW_CURVE_Y_RULE);
        }
        gCurvePosData.yPos[grp][gCurvePosData.validLen] = val;   //填充显示点
    }
    
    gCurvePosData.validLen++;

    if (gShowCurveCtrl.cfg.maxKey)
    if (key > gShowCurveCtrl.cfg.maxKey)
    {
        gCurvePosData.validLen = 0;
        gShowCurveCtrl.pageCount = 0;
    }
}

static T_UICOM_DRAW_MODE  curve_menu_datainit(T_UICOM_COUNT group, T_UICOM_COUNT *x,T_UICOM_COUNT *y)
{
    if (group<SHOW_GROUP_CURVE_NUM)
    {
        u_int8 yofs, xofs;
        
        xofs = *x;
        yofs = gCurvePosData.yPos[group][xofs];
        if ((xofs < UNIT_MAX_W) && (yofs < UNIT_MAX_H))
        {
            *y = POS_MAX_Y - yofs;
            *x = POS_MIN_X + xofs;
            
            return DRAW_MODE_MAP_LINE;
        }
    }

    return DRAW_MODE_SKIP;
}

static void curve_menu_setname(void)
    {
    struct SCREEN_ZONE zone;
    struct UICOM_DATA tmpdata = {0};
    u_int8 tmpbuf[64] = {0};

    SCREEN_ZONE_INIT(&zone, SHOW_CURVE_X_NAME_X, SHOW_CURVE_X_NAME_Y, 32, 14);
    UICOM_DATA_TEXT_ATTR_RST(&tmpdata, TEXT_STYLE_SMALL_SOLID);
    UICOM_DATA_FILL(&tmpdata, tmpbuf);
    if (gShowCurveCtrl.cfg.curveXname)
    {
       gShowCurveCtrl.cfg.curveXname(&tmpdata);
    }
    else
    {
        sprintf(UICOM_DATA_BUF(&tmpdata), "x");
    }
    gui_osd_data_draw(&tmpdata, &zone);

    SCREEN_ZONE_INIT(&zone, SHOW_CURVE_Y_NAME_X, SHOW_CURVE_Y_NAME_Y, SHOW_CURVE_W, 14);
    UICOM_DATA_TEXT_ATTR_RST(&tmpdata, TEXT_STYLE_SMALL_SOLID);
    UICOM_DATA_FILL(&tmpdata, tmpbuf);
    if (gShowCurveCtrl.cfg.curveYname)
    {
       gShowCurveCtrl.cfg.curveYname(&tmpdata);
    }
    else
    {
       sprintf(UICOM_DATA_BUF(&tmpdata), "y=(A+x)(x)");
    }
    gui_osd_data_draw(&tmpdata, &zone);
}

static T_UICOM_DRAW_MODE curvemap_ruler_name(T_UICOM_ORDER order, u_int16 num, struct OSD_ZONE *cellzone, PUICOM_DATA item)
{
    u_int8 ofs;
    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_SMALL_SOLID);

    if (LINE_DIR_IS_X(order))
    {
        cellzone->border.t += 3;
        
        sprintf(UICOM_DATA_BUF(item), "%d", num + gShowCurveCtrl.pageCount*SHOW_CURVE_X_RULE);
        ofs = util_strsize(UICOM_DATA_BUF(item));
        if (ofs)
        {
            ofs = (ofs*8);
            ofs = (ofs+SHOW_CURVE_X_RULE)/SHOW_CURVE_X_RULE;
        }
        if (0 == ofs) ofs = 1;

        //MY_DEBUG("RULER[%d] %X %d\n", num, order, LINE_DIR_IS_END(order));
        
        if (LINE_DIR_IS_END(order) || (0==num) || ((ofs < 5)&&(0 == (num % ofs))))
        {
            UTIL_VAL_SUB(cellzone->zone.x, 2, u_int8);
        }
        else
        {
            return DRAW_MODE_SKIP;
        }
    }
    else
    {
        if (0 == num)
        {
            return DRAW_MODE_SKIP;
        }
        cellzone->zone.x = 0;
        UTIL_VAL_SUB(cellzone->zone.y, 3, u_int8);
        sprintf(UICOM_DATA_BUF(item), "%d", num);
    }
    
    return DRAW_MODE_TEXT_ONLY;
}

static void  curvemap_paint(bool_t isclear)
{
   struct OSD_ZONE osdZone;

   Screen_PrintClear(NULL);
   curve_menu_setname();
   SCREEN_BORDER_INIT(&osdZone.border, SHOW_CURVE_Y_RULE, 0, 0, SHOW_CURVE_X_RULE);
   SCREEN_ZONE_INIT(&osdZone.zone, ORIGN_CURVE_X?(ORIGN_CURVE_X-1):0, 
                ORIGN_CURVE_Y?(ORIGN_CURVE_Y-1):0, SHOW_CURVE_W+1, SHOW_CURVE_H+2);
   gmenu_ruler_draw(&osdZone, PIXEL_MODE_SET, curvemap_ruler_name);
   
   gmenu_value_map_draw(&gMapShowCurveMap, 
    gCurvePosData.validLen, (gShowCurveCtrl.cfg.initCount), gShowCurveCtrl.bitmapItemStatus);
   //--------------------------------------------------------------------
}


//==========================================================
static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(THIS_MENU_SM_HANDLE,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);


//==========================================================

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
}

static void curve_menu_update_all(void)
{
    curvemap_paint(TRUE);
    curvegroup_menu_paint(TRUE);
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);

    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        CURVEGRP_ITEM_SET_STATUS_RANGE(0,  gShowCurveCtrl.cfg.initCount-1);
        curve_menu_update_all();
        return UI_PROC_RET_FINISH;
    }
    
    if (MSG_IS_CHAR(e->sig) && KEY_EVENT_IS_DIGIT(e->param))
    {
        u_int8 keynum  = KEY_EVENT_DIGIT(e->param);
        if ((keynum > 0) && (keynum <= (gShowCurveCtrl.cfg.initCount)))
        {
            keynum -= 1;
            CURVEGRP_ITEM_CPL_STATUS(keynum);
            curvemap_paint(TRUE);
            curvegroup_menu_paint(TRUE);
        }
    }
    else
    switch (e->sig)
    {
        case EVENT_KEY_OK:
            ui_mmi_return(1);
          return UI_PROC_RET_FINISH;
          case EVENT_SYS_HW_AD:
            curve_load_data();
            curve_menu_update_all();
            return UI_PROC_RET_FINISH;
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        case EVENT_KEY_LEFT:
        case EVENT_KEY_RIGHT:
            curve_load_data();
            curve_menu_update_all();
            break;
        default:
            break;
        
    }
    
    return UI_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    UIMMI_DEBUGSM_EXIT(THIS_MENU_NAME, next, me);
    memset(&gShowCurveCtrl, 0, sizeof(gShowCurveCtrl));
}


//=========================================================
#define IDX_CURVE_GROUP_01   (0)
#define IDX_CURVE_GROUP_02   (1)
#define IDX_CURVE_GROUP_03   (2)
#define IDX_CURVE_GROUP_04   (3)


#define CURVE_GROUP_LIST_NUM  (SHOW_GROUP_CURVE_NUM)

#define CURVE_GROUP_LIST_CELL_H  (10)
#define CURVE_GROUP_LIST_CELL_W  (28)
#define CURVE_GROUP_LIST_X(x)    ((x))
#define CURVE_GROUP_LIST_Y(y)    (((y)*(CURVE_GROUP_LIST_CELL_H+2))+1)

static u_int8 curvegroup_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos);
static T_UICOM_DRAW_MODE curvegroup_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type);

LDEF_MENU_CONTENT_LIST(THIS_MENU_UI_CONTAINER, curvegroup_cell_zone_int, curvegroup_cell_data_int);

static u_int8 curvegroup_cell_zone_int(struct OSD_ZONE *zone, T_UICOM_COUNT pos)
{
    struct SCREEN_ZONE cellPosTable[CURVE_GROUP_LIST_NUM] = {
        {CURVE_GROUP_LIST_X(SHOW_GROUP_CURVE_X), CURVE_GROUP_LIST_Y(0), CURVE_GROUP_LIST_CELL_W, CURVE_GROUP_LIST_CELL_H},
        {CURVE_GROUP_LIST_X(SHOW_GROUP_CURVE_X), CURVE_GROUP_LIST_Y(1), CURVE_GROUP_LIST_CELL_W, CURVE_GROUP_LIST_CELL_H},
        {CURVE_GROUP_LIST_X(SHOW_GROUP_CURVE_X),CURVE_GROUP_LIST_Y(2), CURVE_GROUP_LIST_CELL_W, CURVE_GROUP_LIST_CELL_H},
        {CURVE_GROUP_LIST_X(SHOW_GROUP_CURVE_X),CURVE_GROUP_LIST_Y(3), CURVE_GROUP_LIST_CELL_W, CURVE_GROUP_LIST_CELL_H},
    };
    zone->border.l = 0;
    zone->border.t = 0;
    zone->border.b = 0;
    zone->border.r = 0;
    memcpy(&zone->zone, &cellPosTable[pos], sizeof(struct SCREEN_ZONE));

    return 1;
}

#define CURVE_GROUP_LIST_BOX_W  36
#define CURVE_GROUP_LIST_BOX_H  10

static T_UICOM_DRAW_MODE curvegroup_cell_data_int(struct OSD_ZONE *zone, PUICOM_DATA item, T_UICOM_COUNT pos, T_UICOM_COUNT childIdx, T_UICOM_EVENT *p_type)
{
    u_int8 colPosIdx = 0;
    T_UICOM_EVENT type = (*p_type);
    struct SCREEN_ZONE colPosTable[1] = {
        {0,   0,  CURVE_GROUP_LIST_BOX_W,CURVE_GROUP_LIST_BOX_H},
    };
    
    SCREEN_BORDER_INIT(&zone->border, 1, 0, 0, 0);

    colPosIdx = 0;

    memcpy(&zone->zone, &colPosTable[colPosIdx], sizeof(struct SCREEN_ZONE));

    UICOM_CLR_SLECTED((*p_type));
    UICOM_DATA_TEXT_ATTR_RST(item, TEXT_STYLE_DEFAULT);
    if (CURVEGRP_ITEM_TST_STATUS(pos))
    {
        UICOM_SET_SLECTED((*p_type));
    }
    if (0 == childIdx)
    {
        if (pos< SHOW_GROUP_CURVE_NUM)
        {
            if (gShowCurveCtrl.cfg.curvetitle)
            {
                gShowCurveCtrl.cfg.curvetitle(pos, item);
            }
            else
            {
                sprintf(UICOM_DATA_BUF(item), "CH%02d", 1+pos);
            }
        }
        else
        {
            UICOM_DATA_FILL(item, NULL);
        }
    }

    
    return DRAW_MODE_TEXT_ONLY;

}

static void curvegroup_menu_paint(u_int8 isClear)
{
    if (isClear)
    {
       gmenu_content_list_clear_all(&THIS_MENU_UI_CONTAINER, 0);
    }
    
    gmenu_content_list_draw(&THIS_MENU_UI_CONTAINER,(gShowCurveCtrl.cfg.initCount), 0);
}


