#ifndef __UIMMI_CTRL_H__
#define __UIMMI_CTRL_H__
#include "common_define.h"
#include "SmMgr.h"
#include "EventMgr.h"
#include "SwTmrMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_MENU_DEFAULT        UI_NODE_WELCOME

#define UI_PROC_RET_DFT         SM_SET_PUBLIC_ERROR(SM_PROC_ERR_NULL)
#define UI_PROC_RET_FINISH      SM_SET_PRIVATE_ERROR(SM_PROC_ERR_NULL)

#define UI_PROC_NEED_BYPASS(s) (SM_TST_ACCESS(s))

// ui_node_name > sm_node_handle > menu_node_name
enum UI_NODE_NAME {
   UI_NODE_START = SM_INVALID_NODE, 
   UI_NODE_WELCOME,/*<0><1>��ӭ[1]*/
   UI_NODE_MAINMENU,/*<0><X><1> ����������: �б�list[1+4][5] */
   UI_NODE_CHECKSEUP,/*<0><X><1><1> ���ģʽ����edittext[2] +  button[1+2]*/
   UI_NODE_SEARCHOPT,/*<0><X><1><2> ��ѯѡ��edittext[2] + button[2]*/
   UI_NODE_SETUPOPT,/*<0><X><1><3> ����ѡ��edittext[2] + button[2]*/
   UI_NODE_CHSWITCH,/*<0><X><1><3> ͨ���л�ѡ��list[4][4] + button[2] + tips[1]*/
   UI_NODE_POP_INPUTBOX,/*�����*/
   UI_NODE_POP_YESORNO,/*ȷ�Ͽ�*/
   UI_NODE_SHOWCURVE,/*����*/
   UI_NODE_PARAMSETUP,/*��������*/
   UI_NODE_SEARCHINFO,/*��ѯ����*/
   UI_NODE_WARNINFO,/*�����ѯ*/
} ;


void ui_mmi_open(void);
void ui_mmi_start(void);
void ui_mmi_close(void);
void ui_mmi_init(void);
void ui_mmi_deinit(void);
void ui_mmi_enter(enum UI_NODE_NAME name, u_int8 closeCurrent);
void ui_mmi_return(u_int8 retLvl);
void ui_mmi_poweroff(void);
u_int8 ui_mmi_proc(void);
void ui_mmi_send_msg(struct EVENT_NODE_ITEM *e);
void ui_mmi_reg_resume(void (*resume)(SM_NODE_HANDLE child, SM_NODE_HANDLE me));
void ui_mmi_reg_suspend(void (*suspend)(SM_NODE_HANDLE me, SM_NODE_HANDLE child));
void ui_mmi_stop_timer(SWTMR_NODE_HANDLE tmrHdl);
SWTMR_NODE_HANDLE ui_mmi_start_timer(u_int32 ms, void(*callbackFunc)(void *), void *callbackArg, u_int8 isLoop);



#ifdef ENABLE_SM_DEBUG_MODE
u_int8 uimmi_debug_sm_handle(char *nameStr, SM_NODE_HANDLE me, struct EVENT_NODE_ITEM *e);
void uimmi_debug_sm_enter(char *nameStr, SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
void uimmi_debug_sm_exit(char *nameStr, SM_NODE_HANDLE next, SM_NODE_HANDLE me);
void uimmi_debug_sm_suspend(char *nameStr, SM_NODE_HANDLE me, SM_NODE_HANDLE child);
void uimmi_debug_sm_resume(char *nameStr, SM_NODE_HANDLE child, SM_NODE_HANDLE me);
#define UIMMI_DEBUGSM_HANDLE    uimmi_debug_sm_handle
#define UIMMI_DEBUGSM_ENTER     uimmi_debug_sm_enter
#define UIMMI_DEBUGSM_EXIT      uimmi_debug_sm_exit
#define UIMMI_DEBUGSM_SUSPEND   uimmi_debug_sm_suspend
#define UIMMI_DEBUGSM_RESUME    uimmi_debug_sm_resume
#else
#define UIMMI_DEBUGSM_HANDLE    
#define UIMMI_DEBUGSM_ENTER     
#define UIMMI_DEBUGSM_EXIT      
#define UIMMI_DEBUGSM_SUSPEND   
#define UIMMI_DEBUGSM_RESUME    
#endif

#ifdef __cplusplus
}
#endif

#endif

