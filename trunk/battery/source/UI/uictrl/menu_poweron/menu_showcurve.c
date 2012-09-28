#include "uimmi_ctrl.h"
#include "gui_menu_item.h"


#define THIS_MENU_NAME   "œ‘ æ«˙œﬂ"
//"ShowCurve"
#define SHOW_CURVE_X  12
#define SHOW_CURVE_Y  2
#define SHOW_CURVE_H             56
#define SHOW_CURVE_W             SHOW_CURVE_H*2
//---------------------------------------------------------------------
#define UNIT_MAX_X()             SHOW_CURVE_W
#define UNIT_MAX_Y()             SHOW_CURVE_H
//---------------------------------------------------------------------
#define POS_MIN_X                SHOW_CURVE_X
#define POS_MAX_X                (SHOW_CURVE_X + SHOW_CURVE_W-1)
#define POS_MIN_Y                SHOW_CURVE_Y
#define POS_MAX_Y                (POS_MIN_Y + SHOW_CURVE_H-1)
#define UNIT_FACTOR_Y             1
#define UNIT_FACTOR_X             (SHOW_CURVE_W/SHOW_CURVE_H)
//----------------------------------------------------------------------
#define UNIT_TO_POS_X(pixel)    (SHOW_CURVE_X + ((pixel)*UNIT_FACTOR_X))
#define UNIT_TO_POS_Y(pixel)    (POS_MAX_Y -    ((pixel)*UNIT_FACTOR_Y))
#define UNIT_TO_VALUE_X(pixel)  ((pixel)*1)
#define VALUE_TO_UNIT_Y(val)    ((val)/UNIT_FACTOR_Y)
//---------------------------------------------------------------------
#define SHOW_CURVE_Y_RULE        UNIT_FACTOR_Y*8
#define SHOW_CURVE_X_RULE        UNIT_FACTOR_X*8
//---------------------------------------------------------------------
static struct {
    u_int8 test;
    u_int8 mode;
    struct gmenu_curve_param getName;
}gShowCurveCtrl = {0};

static T_UICOM_PAINT_FLAG  menu_showcurve_datainit(T_UICOM_COUNT group, T_UICOM_COUNT *key,T_UICOM_COUNT *y);


void gmenu_show_curve(struct gmenu_curve_param *param)
{

}

LDEF_MENU_VALUE_MAP(gMapShowCurve, menu_showcurve_datainit);

static u_int32  menu_curve_map(u_int32 x)
{
    u_int8 mode = gShowCurveCtrl.mode%23;

    if (mode < 4)
    {
        return ((x+mode)%8)*((x+mode)%8) + mode;
    }
    else if (mode < 7)
    {
        if (x%mode == 0)
        {
            if (32 == gShowCurveCtrl.test)
            {
                gShowCurveCtrl.test = mode;
            }
            else
            {
                gShowCurveCtrl.test = 32;
            }
        }
        
        return gShowCurveCtrl.test;//
    }
    else if (mode <= 18)
    {
        return (x*x*(18-mode))%(SHOW_CURVE_H-4);
    }
    else
    {
        return x*(mode-12);
    }
    
}



static T_UICOM_PAINT_FLAG  menu_showcurve_datainit(T_UICOM_COUNT group, T_UICOM_COUNT *key,T_UICOM_COUNT *y)
{
    T_UICOM_COUNT unit;
    u_int32 val;

    val    = menu_curve_map(UNIT_TO_VALUE_X(*key));
    unit   = VALUE_TO_UNIT_Y(val);
    (*y)   = UNIT_TO_POS_Y(unit); 
    (*key) = UNIT_TO_POS_X(*key);
    if ((((*key) > POS_MAX_X) || ((*key) < POS_MIN_X))\
       || (((*y) > POS_MAX_Y) || ((*y) < POS_MIN_Y)))
    {
        return PAINT_MAP_NULL;
    }
    
    return PAINT_MAP_NORAML;
}


static void  menu_show_curve_paint(bool_t isClear);

static void  menu_show_curve_paint(bool_t isClear)
{
   struct OSD_ZONE osdZone;

   SCREEN_ZONE_INIT(&osdZone.zone, SHOW_CURVE_X, SHOW_CURVE_Y, SHOW_CURVE_W, SHOW_CURVE_H);
   SCREEN_BORDER_INIT(&osdZone.border, SHOW_CURVE_Y_RULE, 0, 0, SHOW_CURVE_X_RULE);
   if (isClear)
   {
       Screen_PrintClear(&osdZone.zone);
   }
   gmenu_value_map_draw(&gMapShowCurve, &osdZone, UNIT_MAX_X(), 1, 1); 
}


//==========================================================
static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(gMenuShowCurve,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);


//==========================================================

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
}

static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    ui_mmi_debug_handle(THIS_MENU_NAME, me, e);

    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        menu_show_curve_paint(1);
    }
    else
    switch (e->sig)
    {
        case EVENT_KEY_NUM_0:
            gShowCurveCtrl.mode++;
            MY_DEBUG("Mode %d\n", gShowCurveCtrl.mode%15);
            menu_show_curve_paint(1);
            break;
        default:
            break;
        
    }
    
    return SM_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    ui_mmi_debug_exit(THIS_MENU_NAME, next, me);
}




