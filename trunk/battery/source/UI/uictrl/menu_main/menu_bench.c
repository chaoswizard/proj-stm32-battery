#include "uimmi_ctrl.h"
#include "gui_menu_item.h"
#include "input_output.h"
#include "ad7705.h"

#define THIS_MENU_NAME  "Ó²¼þÑéÖ¤"


#define THIS_MENU_SM_HANDLE      (gBenchSm)
#define BENCH_REFRESH_MSG    (0x81)

struct BENCH_CTRL {
   SWTMR_NODE_HANDLE  refreshTmr;
    bool_t idx;
    u_int8 step;
};

static  struct BENCH_CTRL gBenchCtrl;

static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next);

DEFINE_SM_NODE_MAP(THIS_MENU_SM_HANDLE,
                                  menu_pub_enter,
                                  menu_pub_handle,
                                  menu_pub_exit);

static void bench_paint(u_int8 isClear)
{
    struct SCREEN_ZONE rect;
     if (isClear)
    {
        Screen_PrintClear(NULL);
    }
    SCREEN_ZONE_INIT(&rect, 5, 1, 120, 10);
    Screen_PrintString(&rect, "please sel bench", TEXT_STYLE_DEFAULT);
        SCREEN_ZONE_INIT(&rect, 5, 11, 120, 10);

    Screen_PrintString(&rect, "0: All Relay on", TEXT_STYLE_DEFAULT);
        SCREEN_ZONE_INIT(&rect, 5, 21, 120, 10);

    Screen_PrintString(&rect, "1: Cyc Samp", TEXT_STYLE_DEFAULT);
        SCREEN_ZONE_INIT(&rect, 5, 31, 120, 10);

    Screen_PrintString(&rect, "2: Relay 1 by 1", TEXT_STYLE_DEFAULT);
        SCREEN_ZONE_INIT(&rect, 5, 41, 120, 10);

    Screen_PrintString(&rect, "3: Samp 1 by 1", TEXT_STYLE_DEFAULT);
        SCREEN_ZONE_INIT(&rect, 5, 51, 120, 10);

    Screen_PrintString(&rect, "4: Relay&Samp 1 by 1", TEXT_STYLE_DEFAULT);
}


static void menu_pub_suspend(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
     RelayAllDown();
     ui_mmi_stop_timer(gBenchCtrl.refreshTmr);
    gBenchCtrl.refreshTmr = SWTMR_INVALID_NODE;
}

static void menu_pub_resume(SM_NODE_HANDLE me, SM_NODE_HANDLE child)
{
    UIMMI_DEBUGSM_RESUME(THIS_MENU_NAME, child, me);
}


static void menu_pub_enter(SM_NODE_HANDLE parent, SM_NODE_HANDLE me)
{
    UIMMI_DEBUGSM_ENTER(THIS_MENU_NAME, parent, me);

}
static void bench_refreshTmr(struct MAIN_MENU_CTRL *hdl)
{
        struct EVENT_NODE_ITEM e;
        
        e.sig = EVENT_USR_MMI;
        USR_EVENT_MMI_INIT(e.param, BENCH_REFRESH_MSG);
        ui_mmi_send_msg(&e);
}
static u_int8 menu_pub_handle(SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e)
{
    struct SCREEN_ZONE rect;
    u_int8 disbuf[64] ;
    u_int8 i;
    UIMMI_DEBUGSM_HANDLE(THIS_MENU_NAME, me, e);

    if (MSG_IS_ENTRY(e->sig))
    {
        Screen_PrintClear(NULL);
        bench_paint(0);
        gBenchCtrl.idx = 1;
        gBenchCtrl.step = 0;
        AD7705_stop();
    }
    else
    {
    switch (e->sig)
    {
        case EVENT_KEY_OK:
        ui_mmi_return(1);
        return UI_PROC_RET_FINISH;
        case EVENT_KEY_CHAR:
            xprintf("key = 0x%x\n", e->param.key.cfg.ch);
            if(0x34 != e->param.key.cfg.ch)
            {
                    ui_mmi_stop_timer(gBenchCtrl.refreshTmr);
                   gBenchCtrl.refreshTmr = SWTMR_INVALID_NODE;
            }
            if(0x30 == e->param.key.cfg.ch)
            {
                xprintf("All Relay on\n");
                 gBenchCtrl.step = 0;
                  RelayAllDown();
                for(i=1;i<=100;i++)
                {
                    RelayCtrl(i, RELAY_ON);
                    Screen_PrintClear(NULL);
                    SCREEN_ZONE_INIT(&rect, 20, 20, 120, 10);
                    sprintf(disbuf, "ch[%d] ON", i);
                     Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                     delay_ms(20);
                 }
                
            }
             else    if(0x31 == e->param.key.cfg.ch)
            {
                xprintf("Cyc Samp\n");
                gBenchCtrl.step = 1;
                 RelayAllDown();
                for(i=1;i<=100;i++)
                {
                    Screen_PrintClear(NULL);
                    SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                    sprintf(disbuf, "ch[%d] = %04dmv",  i,PollReadAdByCh(i)*5000/65535);
                    xprintf("%s\n",disbuf);
                     Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                     delay_ms(40);
                 }
            }
           else   if(0x32 == e->param.key.cfg.ch)
            {
                xprintf(" Relay 1 by 1\n");
                gBenchCtrl.step = 2;
                gBenchCtrl.idx = 1;
                 RelayAllDown();

               RelayStatusAllClr();
                RelayCtrl(gBenchCtrl.idx, RELAY_ON);
                Screen_PrintClear(NULL);
                SCREEN_ZONE_INIT(&rect, 20, 20, 120, 10);
                sprintf(disbuf, "ch[%d] ON",gBenchCtrl.idx);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
            }
             else    if(0x33 == e->param.key.cfg.ch)
            {
                xprintf(" Samp 1 by 1\n");
                gBenchCtrl.step = 3;
                gBenchCtrl.idx = 1;
                 RelayAllDown();
                
                Screen_PrintClear(NULL);
                SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                sprintf(disbuf, "ch[%d] = %04dmv",  gBenchCtrl.idx,PollReadAdByCh(gBenchCtrl.idx)*5000/65535);
                xprintf("%s\n",disbuf);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
            }
            else   if(0x34 == e->param.key.cfg.ch)
            {
                xprintf("Relay&Samp 1 by 1\n");
                gBenchCtrl.step = 4;
                gBenchCtrl.idx = 1;
                 RelayAllDown();

                sprintf(disbuf, "ch[%d] = %04dmv",  gBenchCtrl.idx,PollReadAdByCh(gBenchCtrl.idx)*5000/65535); 
                xprintf("%s\n",disbuf);
                
                 RelayStatusAllClr();
                 RelayCtrl(gBenchCtrl.idx, RELAY_ON);
               

                Screen_PrintClear(NULL);
                SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
                    gBenchCtrl.refreshTmr =  ui_mmi_start_timer(1000,   \
                       bench_refreshTmr,  (p_void)&gBenchCtrl, TRUE);
                
            }
            else
            {
                bench_paint(1);
            }
        break;
        case EVENT_KEY_UP:
            xprintf("up\n");
            gBenchCtrl.idx++;
            if(100 <= gBenchCtrl.idx)
            {
                gBenchCtrl.idx = 100;
            }
            if(2 == gBenchCtrl.step)
            {
                RelayStatusAllClr();
                 RelayCtrl(gBenchCtrl.idx, RELAY_ON);
                sprintf(disbuf, "ch[%d] ON", gBenchCtrl.idx);               

                Screen_PrintClear(NULL);
                SCREEN_ZONE_INIT(&rect, 20, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
            }
            else if(3 == gBenchCtrl.step)
            {
                sprintf(disbuf, "ch[%d] = %04dmv",  gBenchCtrl.idx,PollReadAdByCh(gBenchCtrl.idx)*5000/65535);
                xprintf("%s\n",disbuf); 

                Screen_PrintClear(NULL);
                 SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
            }
            else if(4 == gBenchCtrl.step)
            {
                sprintf(disbuf, "ch[%d] = %04dmv",  gBenchCtrl.idx,PollReadAdByCh(gBenchCtrl.idx)*5000/65535); 
                xprintf("%s\n",disbuf);
                xprintf("relay u\n");
                
                 RelayStatusAllClr();
                 RelayCtrl(gBenchCtrl.idx, RELAY_ON);

                Screen_PrintClear(NULL);
                 SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
            }
            else
            {
                xprintf("err step\n");
            }
            break;
       case EVENT_USR_MMI:
        {
            int myevt = USR_EVENT_MMI(e->param);

            if (BENCH_REFRESH_MSG == (myevt))
            {
                sprintf(disbuf, "ch[%d] = %04dmv",  gBenchCtrl.idx,PollReadAdByCh(gBenchCtrl.idx)*5000/65535); 

                Screen_PrintClear(NULL);
                 SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
            }
            }
            break;
        case EVENT_KEY_DOWN:
                xprintf("down\n");
                gBenchCtrl.idx--;
            if(1 >= gBenchCtrl.idx)
            {
                gBenchCtrl.idx = 1;
            }
            if(2 == gBenchCtrl.step )
            {
                RelayStatusAllClr();
                 RelayCtrl(gBenchCtrl.idx, RELAY_ON);
                sprintf(disbuf, "ch[%d] ON", gBenchCtrl.idx);               

                Screen_PrintClear(NULL);
                SCREEN_ZONE_INIT(&rect, 20, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
            }
           else if(3 == gBenchCtrl.step)
            {

                sprintf(disbuf, "ch[%d] = %04dmv",  gBenchCtrl.idx,PollReadAdByCh(gBenchCtrl.idx)*5000/65535);
                xprintf("%s\n",disbuf);                

                Screen_PrintClear(NULL);
                SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
            }
           else if(4 == gBenchCtrl.step)
           {
                 
                sprintf(disbuf, "ch[%d] = %04dmv",  gBenchCtrl.idx,PollReadAdByCh(gBenchCtrl.idx)*5000/65535);
                xprintf("%s\n",disbuf);
                xprintf("relay d\n");
                 RelayStatusAllClr();
                 RelayCtrl(gBenchCtrl.idx, RELAY_ON);             

                Screen_PrintClear(NULL);
                  SCREEN_ZONE_INIT(&rect, 10, 20, 120, 10);
                 Screen_PrintString(&rect, disbuf, TEXT_STYLE_DEFAULT);
                 delay_ms(20);
           }
           else
           {
                xprintf("err step\n");
           }
        break;
        default:
            break;
    }
    }
    
    return UI_PROC_RET_DFT;
}

static void menu_pub_exit(SM_NODE_HANDLE me, SM_NODE_HANDLE next)
{
    UIMMI_DEBUGSM_EXIT(THIS_MENU_NAME, next, me);
    RelayAllDown();
    
    ui_mmi_stop_timer(gBenchCtrl.refreshTmr);
    gBenchCtrl.refreshTmr = SWTMR_INVALID_NODE;
}





