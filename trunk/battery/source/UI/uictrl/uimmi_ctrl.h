#ifndef __UIMMI_CTRL_H__
#define __UIMMI_CTRL_H__
#include "common_define.h"
#include "SmMgr.h"
#include "EventMgr.h"
#include "SwTmrMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

// ui_node_name > sm_node_handle > menu_node_name
enum UI_NODE_NAME {
   UI_NODE_START = SM_INVALID_NODE, 
   UI_NODE_POWERON,/*<0>poweron  */
   UI_NODE_STOPMENU,/*<0><2>停止[9]  edittext[3] + button[6+2]*/
   UI_NODE_WELCOME,/*<0><1>欢迎[1]*/
   UI_NODE_MAINMENU,/*<0><X><1> 主工作界面: 列表list[1+4][5] */
   UI_NODE_CHECKSEUP,/*<0><X><1><1> 检测模式设置edittext[2] +  button[1+2]*/
   UI_NODE_SEARCHOPT,/*<0><X><1><2> 查询选项edittext[2] + button[2]*/
   UI_NODE_SETUPOPT,/*<0><X><1><3> 设置选项edittext[2] + button[2]*/
   UI_NODE_CHSWITCH,/*<0><X><1><3> 通道切换选择list[4][4] + button[2] + tips[1]*/
} ;

enum UI_NODE_TYPE {
    UI_NODE_TYPE_NULL,
    UI_NODE_TYPE_BRANCH,
    UI_NODE_TYPE_LEAF,
};


void ui_mmi_open(void);
void ui_mmi_close(void);
void ui_mmi_init(void);
void ui_mmi_deinit(void);
void ui_mmi_enter(enum UI_NODE_NAME name, u_int8 closeCurrent);
void ui_mmi_return(u_int8 retLvl);
void ui_mmi_proc(void);
void ui_mmi_send_msg(struct EVENT_NODE_ITEM *e);
void ui_mmi_reg_resume(void (*resume)(SM_NODE_HANDLE child, SM_NODE_HANDLE me));
void ui_mmi_reg_suspend(void (*suspend)(SM_NODE_HANDLE me, SM_NODE_HANDLE child));
void ui_mmi_stop_timer(SWTMR_NODE_HANDLE tmrHdl);
SWTMR_NODE_HANDLE ui_mmi_start_timer(u_int32 ms, void(*callbackFunc)(void *), void *callbackArg, u_int8 isLoop);

void ui_mmi_debug_handle(char *nameStr, SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
void ui_mmi_debug_enter(char *nameStr, SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
void ui_mmi_debug_exit(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE next);
void ui_mmi_debug_suspend(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE child);
void ui_mmi_debug_resume(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE child);

#ifdef __cplusplus
}
#endif

#endif

